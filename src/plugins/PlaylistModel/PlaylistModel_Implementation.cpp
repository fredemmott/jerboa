#include "PlaylistModel_Implementation.h"

#include "MimeData.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QMutableListIterator>
#include <QPalette>
#include <QRegExp>

#define HTML_ESCAPE(x) QString(x).replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;")

PlaylistModel::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		QAbstractItemModel(parent),
		m_currentTrack(-1),
		m_playlist(playlist),
		m_tracks(playlist->tracks())
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
		m_playlist->insertTracks(row, data->urls());
		return true;
	}
	return false;
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
