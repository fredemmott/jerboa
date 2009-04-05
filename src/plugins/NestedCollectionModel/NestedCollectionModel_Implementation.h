#ifndef _NESTED_COLLECTION_MODEL_IMPLEMENTATION_H
#define _NESTED_COLLECTION_MODEL_IMPLEMENTATION_H

#include "CollectionInterface.h"
#include "NestedCollectionModel.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QStringList>

class NestedCollectionModel::Implementation : public QAbstractItemModel
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::CollectionInterface* collection, QObject* parent);

		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		QModelIndex index(int row, int column, const QModelIndex& parent) const;
		QModelIndex parent(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent) const;
	private:
		class Item
		{
			public:
				enum Type
				{
					ArtistItem,
					AlbumItem,
					TrackItem
				};

				Type type;

				QModelIndex parent;
				Jerboa::TrackData data;
		};

		Jerboa::CollectionInterface* m_collection;
		QVector<Jerboa::TrackData> m_tracks;

		QStringList m_artists;
		QList<QStringList> m_albumsForArtists;
		QList<QList<QList<Jerboa::TrackData> > > m_tracksForAlbums;
};

#endif
