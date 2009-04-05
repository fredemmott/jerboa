#include "NestedCollectionModel_Implementation.h"

NestedCollectionModel::Implementation::Implementation(Jerboa::CollectionInterface* collection, QObject* parent)
	:
		QAbstractItemModel(parent),
		m_collection(collection),
		m_tracks(collection->tracks())
{
	QHash<QString, QHash<QString, QList<Jerboa::TrackData> > > artistAlbumTracks;
	///@todo SORTING
	Q_FOREACH(const Jerboa::TrackData& data, m_tracks)
	{
		artistAlbumTracks[data.albumArtist()][data.album()].append(data);
	}

	Q_FOREACH(const QString& albumArtist, artistAlbumTracks.keys())
	{
		m_artists.append(albumArtist);
		const int artistIndex = m_artists.count() - 1;
		const QStringList albums = artistAlbumTracks.value(albumArtist).keys();
		m_albumsForArtists.append(albums);
		m_tracksForAlbums.append(QList<QList<Jerboa::TrackData> >());
		Q_FOREACH(const QString& album, albums)
		{
			m_tracksForAlbums[artistIndex].append(artistAlbumTracks.value(albumArtist).value(album));
		}
	}
}

int NestedCollectionModel::Implementation::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QVariant NestedCollectionModel::Implementation::data(const QModelIndex& index, int role) const
{
	if(role != Qt::DisplayRole)
	{
		return QVariant();
	}

	Item* item = reinterpret_cast<Item*>(index.internalPointer());
	switch(item->type)
	{
		case Item::ArtistItem:
			return m_artists.at(index.row());
		case Item::AlbumItem:
			return m_albumsForArtists.at(index.parent().row()).at(index.row());
		case Item::TrackItem:
			return item->data.title();
	}

	return QVariant();
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
	if(column != 0)
	{
		return QModelIndex();
	}
	if(!parent.isValid())
	{
		if(row >= m_artists.count())
		{
			return QModelIndex();
		}
		Item* item = new Item();
		item->type = Item::ArtistItem;
		return createIndex(row, 0, item);
	}
	Item* parentItem = reinterpret_cast<Item*>(parent.internalPointer());
	Item* item;
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
				item = new Item();
				item->type = Item::TrackItem;
				item->parent = parent;
				item->data = m_tracksForAlbums.at(artistId).at(albumId).at(trackId);
				return createIndex(row, 0, item);
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
				item = new Item();
				item->type = Item::AlbumItem;
				item->parent = parent;
				return createIndex(row, 0, item);
			}
	}

}
