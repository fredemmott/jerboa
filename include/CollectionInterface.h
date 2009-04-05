#ifndef _JERBOA_COLLECTION_INTERFACE_H
#define _JERBOA_COLLECTION_INTERFACE_H

#include "TrackData.h"

#include <QObject>
#include <QVector>

namespace Jerboa
{
	class CollectionInterface : public QObject
	{
		Q_OBJECT;
		public:
			virtual ~CollectionInterface();
			virtual QVector<TrackData> tracks() const = 0;
		protected:
			CollectionInterface(QObject* parent);
		signals:
			void tracksRemoved(const QVector<TrackData>& tracks);
			void tracksAdded(const QVector<TrackData>& tracks);
	};
}

#endif