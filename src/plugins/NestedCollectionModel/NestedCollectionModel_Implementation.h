#ifndef _NESTED_COLLECTION_MODEL_IMPLEMENTATION_H
#define _NESTED_COLLECTION_MODEL_IMPLEMENTATION_H

#include "CollectionInterface.h"
#include "NestedCollectionModel.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QImage>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

class NestedCollectionModel::Implementation : public QAbstractItemModel
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::CollectionInterface* collection, QObject* parent);

		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		QStringList mimeTypes() const;
		QMimeData* mimeData(const QModelIndexList& indexes) const;
		Qt::ItemFlags flags(const QModelIndex& index) const;
	private slots:
		void addTracks(const QList<Jerboa::TrackData>& tracks);
		void updateTracks(const QList<Jerboa::TrackData>& tracks);
		void removeTracks(const QList<QUrl>& tracks);
	private:
		void trimEmptyParents();
		void addTracksInSameAlbum(const QList<Jerboa::TrackData>& tracks);
		void removeTracks(int first, int count);
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

				QPersistentModelIndex parent;
				Jerboa::TrackData data;
		};

		const QImage m_albumImage;
		const QImage m_trackImage;

		mutable Item m_rootItem;
		mutable QVector< QSharedPointer<Item> > m_artistItems;
		mutable QVector< QVector< QSharedPointer<Item> > > m_albumItems;
		mutable QVector< QVector< QVector< QSharedPointer<Item> > > > m_trackItems;

		Jerboa::CollectionInterface* m_collection;
		QVector<Jerboa::TrackData> m_tracks;

		QStringList m_artists;
		QList<QString> m_artistSortOrder;
		QList<QStringList> m_albumsForArtists;
		QList<QList<QList<Jerboa::TrackData> > > m_tracksForAlbums;

		static QString albumSortKey(const QString& album);
};

#endif
