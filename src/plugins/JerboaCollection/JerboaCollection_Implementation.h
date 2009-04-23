#ifndef _JERBOA_COLLECTION_IMPLEMENTATION_H
#define _JERBOA_COLLECTION_IMPLEMENTATION_H

#include "CollectionInterface.h"
#include "TagReader.h"
#include "JerboaCollection.h"

class CollectionScanner;

class JerboaCollection::Implementation : public Jerboa::CollectionInterface
{
	Q_OBJECT
	public:
		Implementation(Jerboa::TagReader* tagReader, QObject* parent);
		QVector<Jerboa::TrackData> tracks() const;
	private slots:
		void applyChanges(const QList<Jerboa::TrackData>& added, const QList<Jerboa::TrackData>& modified, const QStringList& removed);
	private:
		QVector<Jerboa::TrackData> m_tracks;

		CollectionScanner* m_collectionScanner;
};

#endif
