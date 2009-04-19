#include "PlaylistModel_Implementation.h"

#include "MimeData.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QMutableListIterator>
#include <QPalette>
#include <QRegExp>

#define HTML_ESCAPE(x) QString(x).replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;")

PlaylistModel::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, Jerboa::TagReader* tagReader, QObject* parent)
	:
		QAbstractItemModel(parent),
		m_playlist(playlist),
		m_tagReader(tagReader),
		m_tracks(playlist->tracks()),
		m_currentTrack(-1)
{
	connect(
		playlist,
		SIGNAL(tracksAdded(int, QList<Jerboa::TrackData>)),
		this,
		SLOT(addTracks(int, QList<Jerboa::TrackData>))
	);
	connect(
		playlist,
		SIGNAL(tracksRemoved(int, int)),
		this,
		SLOT(removeTracks(int, int))
	);
	connect(
		playlist,
		SIGNAL(positionChanged(int)),
		this,
		SLOT(highlightCurrentTrack(int))
	);
	connect(
		tagReader,
		SIGNAL(notFound(QUrl)),
		this,
		SLOT(tagReaderError())
	);
	connect(
		tagReader,
		SIGNAL(schemeNotSupported(QUrl)),
		this,
		SLOT(tagReaderError())
	);
	connect(
		tagReader,
		SIGNAL(finished(Jerboa::TrackData)),
		this,
		SLOT(addTrackFromUrlDrop(Jerboa::TrackData))
	);
}

bool PlaylistModel::Implementation::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(parent);
	Q_ASSERT(!parent.isValid());
	Q_UNUSED(column);
	if(action == Qt::IgnoreAction)
	{
		return true;
	}

	const Jerboa::MimeData* jerboaData = qobject_cast<const Jerboa::MimeData*>(data);
	if(jerboaData)
	{
		m_playlist->insertTracks(row, jerboaData->tracks());
		return true;
	}
	if(data->hasUrls())
	{
		m_droppedUrls.enqueue(UrlDrop(qMax(0, row), data->urls()));
		if(m_droppedUrls.count() == 1)
		{
			loadNextUrl();
		}
		return true;
	}
	return false;
}

void PlaylistModel::Implementation::tagReaderError()
{
	incrementUrlPositions();
	loadNextUrl();
}

void PlaylistModel::Implementation::addTrackFromUrlDrop(const Jerboa::TrackData& track)
{
	UrlDrop* drop = &m_droppedUrls[0];
	Q_ASSERT(drop->urls.head() == track.url());
	drop->urls.dequeue();
	m_playlist->insertTracks(qBound(0, drop->position, m_playlist->tracks().count()), QList<Jerboa::TrackData>() << track);
	incrementUrlPositions();
	loadNextUrl();
}

void PlaylistModel::Implementation::incrementUrlPositions()
{
	QMutableListIterator<UrlDrop> iterator(m_droppedUrls);
	while(iterator.hasNext())
	{
		UrlDrop* drop = &iterator.next();
		if(drop->urls.isEmpty())
		{
			iterator.remove();
		}
		else
		{
			drop->position++;
		}
	}
}

void PlaylistModel::Implementation::loadNextUrl()
{
	if(m_droppedUrls.isEmpty())
	{
		return;
	}
	Q_ASSERT(!m_droppedUrls.first().urls.isEmpty());
	m_tagReader->readUrl(m_droppedUrls.head().urls.head());
}

QStringList PlaylistModel::Implementation::mimeTypes() const
{
	QStringList mimeTypes;
	mimeTypes
		<< "application/x-jerboa-tracks"
		<< "text/uri-list"
	;
	return mimeTypes;
}

Qt::ItemFlags PlaylistModel::Implementation::flags(const QModelIndex& index) const
{
	const Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
	if(!index.isValid())
	{
		return defaultFlags | Qt::ItemIsDropEnabled;
	}
	return defaultFlags;
}

Qt::DropActions PlaylistModel::Implementation::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

int PlaylistModel::Implementation::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 3;
}

QVariant PlaylistModel::Implementation::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(orientation);
	if(role != Qt::DisplayRole)
	{
		return QVariant();
	}
	switch(section)
	{
		case 0:
			return tr("Title");
		case 1:
			return tr("Artist");
		case 2:
			return tr("Album");
		default:
			return QVariant();
	}
}

void PlaylistModel::Implementation::highlightCurrentTrack(int newCurrentTrack)
{
	if(m_currentTrack != -1)
	{
		emit dataChanged(index(m_currentTrack, 0), index(m_currentTrack, 2));
	}

	m_currentTrack = newCurrentTrack;

	if(m_currentTrack != -1)
	{
		emit dataChanged(index(m_currentTrack, 0), index(m_currentTrack, 2));
	}
}

QVariant PlaylistModel::Implementation::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
	{
		return QVariant();
	}
	if(index.row() < 0 || index.row() >= m_tracks.count())
	{
		return QVariant();
	}
	const Jerboa::TrackData& track = m_tracks.at(index.row());
	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 0:
				return track.title();
			case 1:
				return track.artist();
			case 2:
				return track.album();
			default:
				return QVariant();
		}
	}
	else if(role == Qt::ToolTipRole)
	{
		return QString(
			"<b>%1</b><br/><i>%2 &mdash; %3</i><br/>"
			"<font size='90%'>%4</font>"
		).arg(
			HTML_ESCAPE(track.title())
		).arg(
			HTML_ESCAPE(track.artist())
		).arg(
			HTML_ESCAPE(track.album())
		).arg(
			HTML_ESCAPE(
				track.url().scheme() == "file"
					? QDir::toNativeSeparators(track.url().toLocalFile())
					: track.url().toString()
			)
		);
	}

	if(index.row() == m_playlist->currentTrack())
	{
		if(role == Qt::FontRole)
		{
			QFont font;
			font.setBold(true);
			font.setItalic(true);
			return font;
		}
		else if(role == Qt::BackgroundRole)
		{
			return QApplication::palette().alternateBase();
		}
	}
	return QVariant();
}

QModelIndex  PlaylistModel::Implementation::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

int PlaylistModel::Implementation::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid())
	{
		return 0;
	}
	else
	{
		return m_tracks.count();
	}
}

QModelIndex PlaylistModel::Implementation::index(int row, int column, const QModelIndex& parent) const
{
	if(column < 0 || column > 2 || row < 0 || row >= m_tracks.count() || parent.isValid())
	{
		return QModelIndex();
	}
	return createIndex(row, column);
}

PlaylistModel::Implementation::UrlDrop::UrlDrop(int _position, const QList<QUrl>& _urls)
{
	position = _position;
	Q_FOREACH(const QUrl& url, _urls)
	{
		urls.enqueue(url);
	}
}

void PlaylistModel::Implementation::addTracks(int index, const QList<Jerboa::TrackData>& data)
{
	Q_ASSERT(!data.isEmpty());
	beginInsertRows(QModelIndex(), index, index + data.count() - 1);
	for(int i = 0; i < data.count(); ++i)
	{
		const Jerboa::TrackData& track(data.at(i));
		Q_ASSERT(track.isValid());
		m_tracks.insert(index + i, track);
	}
	endInsertRows();
}

void PlaylistModel::Implementation::removeTracks(int index, int count)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(count > 0);
	beginRemoveRows(QModelIndex(), index, index + count - 1);
	for(int i = 0; i < count; ++i)
	{
		m_tracks.removeAt(index);
	}
	endRemoveRows();
}
