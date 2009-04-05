#ifndef _JERBOA_COLLECTION_IMPLEMENTATION_H
#define _JERBOA_COLLECTION_IMPLEMENTATION_H

#include "CollectionInterface.h"
#include "JerboaCollection.h"

class JerboaCollection::Implementation : public Jerboa::CollectionInterface
{
	Q_OBJECT
	public:
		Implementation(QObject* parent);
		QVector<Jerboa::TrackData> tracks() const;
	private:
		QVector<Jerboa::TrackData> m_tracks;
};

#endif
