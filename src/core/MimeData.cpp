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

	bool MimeData::hasFormat(const QString& mimeType) const
	{
		if(mimeType == "application/x-jerboa-tracks")
		{
			return true;
		}
		return QMimeData::hasFormat(mimeType);
	}
}
