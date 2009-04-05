#ifndef _JERBOA_COLLECTION_INTERFACE_H
#define _JERBOA_COLLECTION_INTERFACE_H

#include "TrackData.h"

#include <QObject>

namespace Jerboa
{
	class CollectionInterface : public QObject
	{
		Q_OBJECT;
		public:
			virtual ~CollectionInterface();
			virtual QList<TrackData> tracks() const = 0;
		protected:
			CollectionInterface(QObject* parent);
		signals:
			void tracksRemoved(const QList<TrackData>& tracks);
			void tracksAdded(const QList<TrackData>& tracks);
	};
}

#endif
