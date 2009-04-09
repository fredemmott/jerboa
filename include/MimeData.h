#ifndef _JERBOA_MIME_DATA_H
#define _JERBOA_MIME_DATA_H

#include "TrackData.h"

#include <QList>
#include <QMimeData>

namespace Jerboa
{
	class MimeData : public QMimeData
	{
		Q_OBJECT;
		public:
			void setTracks(const QList<TrackData>& tracks);
			QList<TrackData> tracks() const;
		private:
			QList<TrackData> m_tracks;
	};
}

#endif
