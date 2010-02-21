#include "NestedCollectionModel_Implementation.h"

#include "MimeData.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QRegExp>
#include <QSet>

inline uint qHash(const QUrl& url)
{
	return qHash(url.toString());
}

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
		m_tracksFromUrls.insert(track.url(), track);

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
			Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());
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
		SIGNAL(tracksModified(QList<Jerboa::TrackData>)),
		this,
		SLOT(updateTracks(QList<Jerboa::TrackData>))
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
		Q_ASSERT(track.isValid());
		m_tracksFromUrls.insert(track.url(), track);

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

	int i = 0;
	m_tracks.reserve(m_tracks.count() + tracks.count() );
	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		while(i < m_tracks.count() && m_tracks.at(i) < track)
		{
			++i;
		}
		m_tracks.insert(i, track);
	}
}

int NestedCollectionModel::Implementation::findOrCreateArtist(const Jerboa::TrackData& track, bool* created)
{
	int artistPosition = m_artists.indexOf(track.albumArtist());

	if(artistPosition == -1)
	{
		// New artist
		const QString sortKey = track.albumArtistRomanised().toLower();
		// Add our sort key to the sort list
		m_artistSortOrder.append(sortKey);
		// Sort the sort list
		qSort(m_artistSortOrder);
		// Find our new index;
		artistPosition = m_artistSortOrder.indexOf(sortKey);

		beginInsertRows(QModelIndex(), artistPosition, artistPosition);

		// Add artist to its' lists
		m_artists.insert(artistPosition, track.albumArtist());

		Item* item = new Item();
		item->type = Item::ArtistItem;
		m_artistItems.insert(artistPosition, sharePtr(item));

		// Add empty lists for albums/tracks
		m_albumsForArtists.insert(artistPosition, QStringList());
		m_albumItems.insert(artistPosition, QVector< QSharedPointer<Item> >());

		m_tracksForAlbums.insert(artistPosition, QList< QList<Jerboa::TrackData> >());
		m_trackItems.insert(artistPosition, QVector< QVector< QSharedPointer<Item> > >());

		endInsertRows();

		if(created)
		{
			*created = true;
		}
	}
	else if(created)
	{
		*created = false;
	}

	return artistPosition;
}

int NestedCollectionModel::Implementation::findOrCreateAlbum(const Jerboa::TrackData& track, int artistPosition, bool* created)
{
	if(artistPosition == -1)
	{
		artistPosition = findOrCreateArtist(track);
	}
	Q_ASSERT(artistPosition != -1);
	const QPersistentModelIndex artistIndex = index(artistPosition, 0);
	Q_ASSERT(artistIndex.isValid());

	int albumPosition = m_albumsForArtists.value(artistPosition).indexOf(track.album());
	if(albumPosition == -1)
	{
		// New album, add it
		// Figure out the position
		QStringList albums = m_albumsForArtists.at(artistPosition);
		albums.append(track.album());
		qSort(albums.begin(), albums.end(), Jerboa::TrackData::albumLessThan);

		albumPosition = albums.indexOf(track.album());

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
		Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());

		endInsertRows();

		if(created)
		{
			*created = true;
		}
	}
	else if(created)
	{
		*created = false;
	}
	return albumPosition;
}

void NestedCollectionModel::Implementation::addTracksInSameAlbum(const QList<Jerboa::TrackData>& tracks)
{
	const Jerboa::TrackData& first = tracks.first();

	int artistPosition = findOrCreateArtist(first);
	Q_ASSERT(artistPosition != -1);
	const QPersistentModelIndex artistIndex = index(artistPosition, 0);
	Q_ASSERT(artistIndex.isValid());

	const int albumPosition = findOrCreateAlbum(first, artistPosition);
	Q_ASSERT(albumPosition != -1);
	const QPersistentModelIndex albumIndex = index(albumPosition, 0, artistIndex);
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
			currentSet.clear();
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
		Q_ASSERT(first >= 0);

		beginInsertRows(albumIndex, first, first + (count - 1));

		m_tracksForAlbums[artistPosition][albumPosition] = batchNewTracks;

		Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());

		QVector< QSharedPointer<Item> >& trackItems = m_trackItems[artistPosition][albumPosition];
		Q_ASSERT(first <= trackItems.count());
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

void NestedCollectionModel::Implementation::updateTracks(const QList<Jerboa::TrackData>& _tracks)
{
	if(_tracks.isEmpty())
	{
		return;
	}

	QList<Jerboa::TrackData> tracks(_tracks);
	qSort(tracks);

	QList< QList<Jerboa::TrackData> > contiguousSets;
	QList<Jerboa::TrackData> currentSet;

	QString previousAlbum;
	int previousIndex = -1;

	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		const int index = m_tracks.indexOf(m_tracksFromUrls.value(track.url()));
		Q_ASSERT(index != -1);
		if(
			(
				index != previousIndex + 1
				||
				previousAlbum != track.album()
			)
			&&
			!currentSet.isEmpty()
		)
		{
			contiguousSets.append(currentSet);
			currentSet.clear();
		}
		currentSet.append(track);
		previousIndex = index;
		previousAlbum = track.album();
	}
	if(!currentSet.isEmpty())
	{
		contiguousSets.append(currentSet);
	}
	Q_FOREACH(const QList<Jerboa::TrackData>& set, contiguousSets)
	{
		updateContiguousTracks(set);
	}
}

void NestedCollectionModel::Implementation::updateContiguousTracks(const QList<Jerboa::TrackData>& tracks)
{
	const Jerboa::TrackData& first = tracks.first();

	bool newArtist;
	bool newAlbum;

	const int artistPosition = findOrCreateArtist(first, &newArtist);
	Q_ASSERT(artistPosition != -1);
	const QModelIndex artistIndex = index(artistPosition, 0, QModelIndex());
	Q_ASSERT(artistIndex.isValid());

	const int albumPosition = findOrCreateAlbum(first, artistPosition, &newAlbum);
	Q_ASSERT(albumPosition != -1);
	const QModelIndex albumIndex = index(albumPosition, 0, artistIndex);
	Q_ASSERT(albumIndex.isValid());

	QHash<QUrl, Jerboa::TrackData> newTracks;
	QList<QUrl> urls;
	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		newTracks.insert(track.url(), track);
		urls.append(track.url());
	}

	if(newArtist || newAlbum)
	{
		// Remove and insert
		removeTracks(urls);
		addTracks(tracks);
	}
	else
	{
		// Update
	
		// Replace the data
		QList<Jerboa::TrackData>& albumTracks = m_tracksForAlbums[artistPosition][albumPosition];
		Q_FOREACH(const Jerboa::TrackData& oldTrack, albumTracks)
		{
			if(newTracks.contains(oldTrack.url()))
			{
				const int index = albumTracks.indexOf(oldTrack);
				albumTracks.replace(index, newTracks.value(oldTrack.url()));
			}
		}
	
		const int firstPosition = albumTracks.indexOf(first);
		const int lastPosition = albumTracks.indexOf(tracks.last());
	
		for(int i = firstPosition; i <= lastPosition; ++i)
		{
			m_trackItems[artistPosition][albumPosition][i]->data = albumTracks.at(i);
		}

		Q_FOREACH(const Jerboa::TrackData& track, m_tracks)
		{
			if(urls.contains(track.url()))
			{
				m_tracks.remove(m_tracks.indexOf(track));
			}
		}
		m_tracks += tracks.toVector();
		qSort(m_tracks);
	
		emit dataChanged(index(firstPosition, 0, albumIndex), index(lastPosition, 0, albumIndex));
	}
	Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());
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
	Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());
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

				m_trackItems[artistPosition].remove(albumPosition);
				m_tracksForAlbums[artistPosition].removeAt(albumPosition);
				endRemoveRows();
				--albumPosition;
			}
			Q_ASSERT(m_trackItems.at(artistPosition).at(albumPosition).count() == m_tracksForAlbums.at(artistPosition).at(albumPosition).count());
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
							"<b>%1</b><br />(%2 - %3)<br /><font size='90%'>%4</font>"
						).arg(
							index.data(Qt::DisplayRole).toString()
						).arg(
							index.parent().data(Qt::DisplayRole).toString()
						).arg(
							index.parent().parent().data(Qt::DisplayRole).toString()
						).arg(
							item->data.url().toString()
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
	return QModelIndex();
}
