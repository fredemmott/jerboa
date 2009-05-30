#include "NestedCollectionModel_Implementation.h"

#include "MimeData.h"

#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QPalette>
#include <QRegExp>
#include <QSet>

template<typename T> QSharedPointer<T> sharePtr(T* raw) { return QSharedPointer<T>(raw); }

NestedCollectionModel::Implementation::Implementation(Jerboa::CollectionInterface* collection, QObject* parent)
	:
		QAbstractItemModel(parent),
		m_collection(collection),
		m_tracks(collection->tracks()),
		m_albumImage(QImage(":/NestedCollectionModel/album.png")),
		m_trackImage(QImage(":/NestedCollectionModel/track.png"))
{
	// Get things in the right order for a flat tree
	qSort(m_tracks);
	int artistIndex = -1;
	int albumIndex = -1;
	QString previousArtist;
	QString previousAlbum;
	Q_FOREACH(const Jerboa::TrackData& track, m_tracks)
	{
		if(previousArtist != track.albumArtist())
		{
			++artistIndex;
			albumIndex = -1;

			m_artists.append(track.albumArtist());
			m_artistSortOrder.append(track.albumArtistRomanised().toLower());
			m_albumsForArtists.append(QStringList());
			m_tracksForAlbums.append(QList< QList<Jerboa::TrackData> >());

			previousAlbum = QString();
		}

		if(previousAlbum != track.album())
		{
			++albumIndex;

			m_albumsForArtists[artistIndex].append(track.album());
			m_tracksForAlbums[artistIndex].append(QList<Jerboa::TrackData>());
		}

		m_tracksForAlbums[artistIndex][albumIndex].append(track);

		previousArtist = track.albumArtist(); previousAlbum = track.album();
	}

	// Sort out the model items
	const int artistCount = m_artists.count();
	m_artistItems.resize(artistCount);
	m_albumItems.resize(artistCount);
	m_trackItems.resize(artistCount);
	for(int artistPosition = 0; artistPosition < artistCount; ++artistPosition)
	{
		const int albumCount = m_albumsForArtists.at(artistPosition).count();

		m_albumItems[artistPosition].resize(albumCount);
		m_trackItems[artistPosition].resize(albumCount);

		Item* artistItem = new Item();
		artistItem->type = Item::ArtistItem;
		m_artistItems[artistPosition] = sharePtr(artistItem);
		const QPersistentModelIndex artistIndex = index(artistPosition, 0);
		for(int albumPosition = 0; albumPosition < albumCount; ++albumPosition)
		{
			const int trackCount = m_tracksForAlbums.at(artistPosition).at(albumPosition).count();
			m_trackItems[artistPosition][albumPosition].resize(trackCount);

			Item* albumItem = new Item();
			albumItem->type = Item::AlbumItem;
			albumItem->parent = artistIndex;
			m_albumItems[artistPosition][albumPosition] = sharePtr(albumItem);
			const QPersistentModelIndex albumIndex = index(albumPosition, 0, artistIndex);

			for(int trackPosition = 0; trackPosition < trackCount; ++trackPosition)
			{
				Item* trackItem = new Item();
				trackItem->type = Item::TrackItem;
				trackItem->parent = albumIndex;
				trackItem->data = m_tracksForAlbums.at(artistPosition).at(albumPosition).at(trackPosition);
				m_trackItems[artistPosition][albumPosition][trackPosition] = sharePtr(trackItem);
			}
		}
	}

	connect(
		collection,
		SIGNAL(tracksAdded(QList<Jerboa::TrackData>)),
		this,
		SLOT(addTracks(QList<Jerboa::TrackData>))
	);
	connect(
		collection,
		SIGNAL(tracksRemoved(QList<QUrl>)),
		this,
		SLOT(removeTracks(QList<QUrl>))
	);
}

void NestedCollectionModel::Implementation::addTracks(const QList<Jerboa::TrackData>& _tracks)
{
	qDebug() << Q_FUNC_INFO << __LINE__ << "Adding" << _tracks.count() << "tracks";
	// Get artists/albums contiguous, and ordered by track number
	QList<Jerboa::TrackData> tracks(_tracks);
	qSort(tracks);
	const int trackCount = tracks.count();

	QString currentArtist;
	QString currentAlbum;
	int first = -1;
	int count = 0;
	for(int i = 0; i < trackCount; ++i)
	{
		const Jerboa::TrackData& track = tracks.at(i);

		// See if we've the artist/album has changed
		if(track.albumArtist() != currentArtist || track.album() != currentAlbum)
		{
			if(count != 0)
			{
				addTracksInSameAlbum(tracks.mid(first, count));
			}
			first = i;
			count = 0;
			currentArtist = track.albumArtist();
			currentAlbum = track.album();
		}
		++count;
	}
	if(count != 0)
	{
		addTracksInSameAlbum(tracks.mid(first, count));
	}
}

void NestedCollectionModel::Implementation::addTracksInSameAlbum(const QList<Jerboa::TrackData>& tracks)
{
	const Jerboa::TrackData& first = tracks.first();
	int artistPosition = m_artists.indexOf(first.albumArtist());

	if(artistPosition == -1)
	{
		// New artist
		const QString sortKey = first.albumArtistRomanised().toLower();
		// Add our sort key to the sort list
		m_artistSortOrder.append(sortKey);
		// Sort the sort list
		qSort(m_artistSortOrder);
		// Find our new index;
		artistPosition = m_artistSortOrder.indexOf(sortKey);

		beginInsertRows(QModelIndex(), artistPosition, artistPosition);

		// Add artist to its' lists
		m_artists.insert(artistPosition, first.albumArtist());

		Item* item = new Item();
		item->type = Item::ArtistItem;
		m_artistItems.insert(artistPosition, sharePtr(item));

		// Add empty lists for albums/tracks
		m_albumsForArtists.insert(artistPosition, QStringList());
		m_albumItems.insert(artistPosition, QVector< QSharedPointer<Item> >());

		m_tracksForAlbums.insert(artistPosition, QList< QList<Jerboa::TrackData> >());
		m_trackItems.insert(artistPosition, QVector< QVector< QSharedPointer<Item> > >());

		endInsertRows();
	}

	const QPersistentModelIndex artistIndex = index(artistPosition, 0);

	int albumPosition = m_albumsForArtists.value(artistPosition).indexOf(first.album());
	if(albumPosition == -1)
	{
		// New album, add it
		// Figure out the position
		QStringList albums = m_albumsForArtists.at(artistPosition);
		albums.append(first.album());
		qSort(albums);

		albumPosition = albums.indexOf(first.album());

		beginInsertRows(artistIndex, albumPosition, albumPosition);

		// Insert it into its' own lists
		m_albumsForArtists[artistPosition] = albums;
		Item* item = new Item();
		item->type = Item::AlbumItem;
		item->parent = artistIndex;
		m_albumItems[artistPosition].insert(albumPosition, sharePtr(item));

		// Expand the track list
		m_tracksForAlbums[artistPosition].insert(albumPosition, QList<Jerboa::TrackData>());
		m_trackItems[artistPosition].insert(albumPosition, QVector< QSharedPointer<Item> >());
	}

	const QPersistentModelIndex albumIndex = index(albumPosition, 0, artistIndex);

	Q_ASSERT(artistIndex.isValid());
	Q_ASSERT(albumIndex.isValid());
	
	// Okay, got everything, now work out if post-insert which of these are contiguous
	QList< QList<Jerboa::TrackData> > contiguousSets;
	QList<Jerboa::TrackData> currentSet;
	int previousIndex = -1;

	QList<Jerboa::TrackData> newTracks = m_tracksForAlbums.at(artistPosition).at(albumPosition);
	newTracks.append(tracks);
	qSort(newTracks);

	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		const int index = newTracks.indexOf(track);
		if(index != previousIndex + 1 && ! currentSet.isEmpty())
		{
			contiguousSets.append(currentSet);
		}
		currentSet.append(track);
		previousIndex = index;
	}

	if(!currentSet.isEmpty())
	{
		contiguousSets.append(currentSet);
	}

	Q_FOREACH(const QList<Jerboa::TrackData>& trackBatch, contiguousSets)
	{
		QList<Jerboa::TrackData> batchNewTracks = m_tracksForAlbums.at(artistPosition).at(albumPosition);
		batchNewTracks.append(trackBatch);
		qSort(batchNewTracks);

		const int first = batchNewTracks.indexOf(trackBatch.first());
		const int count = trackBatch.count();
		beginInsertRows(albumIndex, first, first + (count - 1));

		m_tracksForAlbums[artistPosition][albumPosition] = batchNewTracks;

		QVector< QSharedPointer<Item> >& trackItems = m_trackItems[artistPosition][albumPosition];
		trackItems.insert(first, count, QSharedPointer<Item>());
		for(int trackIndex = 0; trackIndex < count; ++ trackIndex)
		{
			Item* item = new Item();
			item->type = Item::TrackItem;
			item->parent = albumIndex;
			item->data = trackBatch.at(trackIndex);
			trackItems[trackIndex + first] = sharePtr(item);
		}
		
		endInsertRows();
	}
}

void NestedCollectionModel::Implementation::updateTracks(const QList<Jerboa::TrackData>& tracks)
{
}

void NestedCollectionModel::Implementation::removeTracks(const QList<QUrl>& urls)
{
	int first = -1;
	Jerboa::TrackData firstTrack;
	int count = 0;
	Q_FOREACH(const Jerboa::TrackData& track, m_tracks)
	{
		if(urls.contains(track.url()))
		{
			if(first != -1 && track.album() == firstTrack.album() && track.albumArtist() == firstTrack.albumArtist())
			{
				++count;
				continue;
			}

			if(first != -1)
			{
				removeTracks(first, count);
			}

			first = m_tracks.indexOf(track);
			firstTrack = track;
			count = 1;
		}
		else
		{
			if(first != -1)
			{
				removeTracks(first, count);
				count = 0;
				first = -1;
			}
		}
	}
	if(first != -1)
	{
		removeTracks(first, count);
	}
	trimEmptyParents();
}

void NestedCollectionModel::Implementation::removeTracks(int first, int count)
{
	const Jerboa::TrackData firstTrack = m_tracks.at(first);

	// Find the albumArtist node
	const int artistPosition = m_artists.indexOf(firstTrack.albumArtist());
	Q_ASSERT(artistPosition >= 0);
	const QModelIndex artistIndex = index(artistPosition, 0, QModelIndex());
	Q_ASSERT(artistIndex.isValid());

	// now the album node
	const int albumPosition = m_albumsForArtists.at(artistPosition).indexOf(firstTrack.album());
	Q_ASSERT(albumPosition >= 0);
	const QModelIndex albumIndex = index(albumPosition, 0, artistIndex);
	Q_ASSERT(albumIndex.isValid());

	// Offset between m_tracks and child
	const int childFirst = m_tracksForAlbums.at(artistPosition).at(albumPosition).indexOf(firstTrack);
	const int childLast = childFirst + count - 1;
	beginRemoveRows(albumIndex, childFirst, childLast);
	m_tracks.remove(first, count);
	m_trackItems[artistPosition][albumPosition].remove(childFirst, count);
	for(int i = 0; i < count; ++i)
	{
		m_tracksForAlbums[artistPosition][albumPosition].removeAt(childFirst);
	}
	endRemoveRows();
}

void NestedCollectionModel::Implementation::trimEmptyParents()
{
	for(int artistPosition = 0; artistPosition < rowCount(); ++artistPosition)
	{
		const QPersistentModelIndex artistIndex(index(artistPosition, 0));
		for(int albumPosition = 0; albumPosition < rowCount(artistIndex); ++albumPosition)
		{
			const QModelIndex albumIndex(index(albumPosition, 0, artistIndex));
			if(rowCount(albumIndex) == 0)
			{
				beginRemoveRows(artistIndex, albumPosition, albumPosition);
				m_albumItems[artistPosition].remove(albumPosition);
				m_albumsForArtists[artistPosition].removeAt(albumPosition);
				endRemoveRows();
				--albumPosition;
			}
		}
		if(rowCount(artistIndex) == 0)
		{
			beginRemoveRows(QModelIndex(), artistPosition, artistPosition);

			m_artistItems.remove(artistPosition);
			m_artists.removeAt(artistPosition);

			m_albumItems.remove(artistPosition);
			m_albumsForArtists.removeAt(artistPosition);

			m_trackItems.remove(artistPosition);
			m_tracksForAlbums.removeAt(artistPosition);

			endRemoveRows();
			--artistPosition;
		}
	}
}

Qt::ItemFlags NestedCollectionModel::Implementation::flags(const QModelIndex& index) const
{
	const Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
	if(index.isValid())
	{
		return defaultFlags | Qt::ItemIsDragEnabled;
	}
	else
	{
		return defaultFlags;
	}
}

QMimeData* NestedCollectionModel::Implementation::mimeData(const QModelIndexList& indexes) const
{
	QList<Jerboa::TrackData> tracks;
	Q_FOREACH(const QModelIndex& index, indexes)
	{
		QList<Jerboa::TrackData> indexTracks = index.data(Qt::UserRole).value<QList<Jerboa::TrackData> >();
		Q_FOREACH(const Jerboa::TrackData& track, indexTracks)
		{
			if(!tracks.contains(track))
			{
				tracks.append(track);
			}
		}
	}
	QList<QUrl> urls;
	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		urls.append(track.url());
	}

	Jerboa::MimeData* mimeData = new Jerboa::MimeData();
	mimeData->setText(tr("%1 Tracks").arg(tracks.count()));
	mimeData->setUrls(urls);
	mimeData->setTracks(tracks);
	return mimeData;
}

QStringList NestedCollectionModel::Implementation::mimeTypes() const
{
	return QStringList()
		<< "text/plain"
		<< "text/uri-list"
		<< "application/x-jerboa-tracks"
	;
}

int NestedCollectionModel::Implementation::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QVariant NestedCollectionModel::Implementation::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
	{
		return QVariant();
	}

	Item* item = reinterpret_cast<Item*>(index.internalPointer());
	switch(role)
	{
		case Qt::BackgroundRole:
			if(item->type == Item::ArtistItem)
			{
				return QApplication::palette().dark();
			}
			return QVariant();
		case Qt::DecorationRole:
			switch(item->type)
			{
				case Item::ArtistItem:
					return QVariant();
				case Item::AlbumItem:
					return m_albumImage;
				case Item::TrackItem:
					return m_trackImage;
			}
		case Qt::DisplayRole:
			switch(item->type)
			{
				case Item::ArtistItem:
					return m_artists.at(index.row());
				case Item::AlbumItem:
					return m_albumsForArtists.at(index.parent().row()).at(index.row());
				case Item::TrackItem:
					return item->data.title();
			}
		case Qt::FontRole:
			if(item->type == Item::ArtistItem)
			{
				QFont font;
				font.setBold(true);
				return font;
			}
			return QVariant();
		case Qt::ToolTipRole:
			{
				const int trackCount = index.data(Qt::UserRole).value<QList<Jerboa::TrackData> >().count();
				switch(item->type)
				{
					case Item::ArtistItem:
						return tr(
							"<b>%1</b><br />%2 albums<br />%3 tracks"
						).arg(
							index.data(Qt::DisplayRole).toString()
						).arg(
							index.model()->rowCount(index)
						).arg(
							trackCount
						);
					case Item::AlbumItem:
						return tr(
							"<b>%1</b><br />(%2)<br />%3 tracks"
						).arg(
							index.data(Qt::DisplayRole).toString()
						).arg(
							index.parent().data(Qt::DisplayRole).toString()
						).arg(
							trackCount
						);
					case Item::TrackItem:
						return tr(
							"<b>%1</b><br />(%2 - %3)"
						).arg(
							index.data(Qt::DisplayRole).toString()
						).arg(
							index.parent().data(Qt::DisplayRole).toString()
						).arg(
							index.parent().parent().data(Qt::DisplayRole).toString()
						);
				}
			}
		case Qt::UserRole:
			{
				QList<Jerboa::TrackData> tracks;
				if(item->type == Item::TrackItem)
				{
					tracks.append(item->data);
				}
				else
				{
					for(int i = 0; i < rowCount(index); ++i)
					{
						tracks.append(index.child(i, 0).data(Qt::UserRole).value<QList<Jerboa::TrackData> >());
					}
				}
				return QVariant::fromValue(tracks);
			}
		default:
			return QVariant();
	}
}

QModelIndex  NestedCollectionModel::Implementation::parent(const QModelIndex& index) const
{
	if(!index.isValid())
	{
		return QModelIndex();
	}

	Item* item = reinterpret_cast<Item*>(index.internalPointer());
	return item->parent;
}

int NestedCollectionModel::Implementation::rowCount(const QModelIndex& parent) const
{
	if(!parent.isValid())
	{
		// Parent is root
		return m_artists.count();
	}
	
	Item* item = reinterpret_cast<Item*>(parent.internalPointer());

	if(!parent.parent().isValid())
	{
		// Parent is an artist
		Q_ASSERT(item->type == Item::ArtistItem);

		return m_albumsForArtists.at(parent.row()).count();
	}

	if(!parent.parent().parent().isValid())
	{
		// Parent is an album
		const QModelIndex artistIndex = parent.parent();
		return m_tracksForAlbums.at(artistIndex.row()).at(parent.row()).count();
	}

	// Parent is a track
	return 0;
}

QString NestedCollectionModel::Implementation::albumSortKey(const QString& albumName)
{
	// Need to make a new Album record - this needs a sortKey
	// Eat spaces
	QString albumSort = QString(albumName.toLower()).replace(" ", "");
	// Replaces symbols with ! ("Foo: bar" comes before "Foo 2: Bar")
	albumSort.replace(QRegExp("\\W"), "!");
	// Pad numbers to six figures
	albumSort.replace(QRegExp("(\\d+)"), "00000\\1");
	albumSort.replace(QRegExp("\\d+(\\d{6})"), "\\1");
	return albumSort;
}

QModelIndex NestedCollectionModel::Implementation::index(int row, int column, const QModelIndex& parent) const
{
	if(column != 0 || row < 0)
	{
		return QModelIndex();
	}
	if(!parent.isValid())
	{
		if(row >= m_artists.count())
		{
			return QModelIndex();
		}
		return createIndex(row, 0, m_artistItems.value(row).data());
	}
	Item* parentItem = reinterpret_cast<Item*>(parent.internalPointer());
	switch(parentItem->type)
	{
		case Item::TrackItem:
			return QModelIndex();
		case Item::AlbumItem:
			{
				const int artistId = parent.parent().row();
				const int albumId = parent.row();
				const int trackId = row;
				if(
					artistId >= m_artists.count()
					||
					albumId >= m_albumsForArtists.at(artistId).count()
					||
					trackId >= m_tracksForAlbums.at(artistId).at(albumId).count()
				)
				{
					return QModelIndex();
				}
				return createIndex(row, 0, m_trackItems.value(artistId).value(albumId).value(trackId).data());
			}
		case Item::ArtistItem:
			{
				const int artistId = parent.row();
				const int albumId = row;
				if(
					artistId >= m_artists.count()
					||
					albumId >= m_albumsForArtists.at(artistId).count()
				)
				{
					return QModelIndex();
				}
				return createIndex(row, 0, m_albumItems.value(artistId).value(albumId).data());
			}
	}

}
