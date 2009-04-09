#include "MimeData.h"

namespace Jerboa
{
	void MimeData::setTracks(const QList<TrackData>& tracks)
	{
		m_tracks = tracks;
	}

	QList<TrackData> MimeData::tracks() const
	{
		return m_tracks;
	}
}
