#include "JerboaPlaylist_Implementation.h"

#include <QDateTime>

JerboaPlaylist::Implementation::Implementation(QObject* parent)
	:
		Jerboa::PlaylistInterface(parent),
		m_loopMode(LoopNone),
		m_shuffleMode(ShuffleNone),
		m_currentTrack(-1),
		m_nextTrack(-1)
{
	qsrand(QDateTime::currentDateTime().toTime_t());
}

Jerboa::PlaylistInterface::LoopMode JerboaPlaylist::Implementation::loopMode() const
{
	return m_loopMode;
}

Jerboa::PlaylistInterface::ShuffleMode JerboaPlaylist::Implementation::shuffleMode() const
{
	return m_shuffleMode;
}

void JerboaPlaylist::Implementation::setLoopMode(LoopMode mode)
{
	m_loopMode = mode;
	adjustNextTrack();
}

void JerboaPlaylist::Implementation::setShuffleMode(ShuffleMode mode)
{
	m_shuffleMode = mode;
	adjustNextTrack();
}

int JerboaPlaylist::Implementation::currentTrack() const
{
	return m_currentTrack;
}

int JerboaPlaylist::Implementation::nextTrack() const
{
	return m_nextTrack;
}

QList<Jerboa::TrackData> JerboaPlaylist::Implementation::tracks() const
{
	return m_tracks;
}

int JerboaPlaylist::Implementation::appendTrack(const Jerboa::TrackData& data)
{
	Q_ASSERT(data.isValid());
	m_tracks.append(data);
	adjustNextTrack();
	return m_tracks.count() - 1;
}

void JerboaPlaylist::Implementation::insertTrack(int index, const Jerboa::TrackData& data)
{
	Q_ASSERT(data.isValid());
	m_tracks.insert(index, data);
	if(m_currentTrack >= index)
	{
		++m_currentTrack;
	}
	adjustNextTrack();
}

void JerboaPlaylist::Implementation::removeTrack(int index)
{
	m_tracks.removeAt(index);
	if(m_currentTrack == index)
	{
		m_currentTrack = -1;
	}
	else if (m_currentTrack > index)
	{
		--m_currentTrack;
	}
	adjustNextTrack();
}

void JerboaPlaylist::Implementation::setCurrentTrack(int index)
{
	Q_ASSERT(index < m_tracks.count());
	m_currentTrack = index;
	adjustNextTrack();
}

void JerboaPlaylist::Implementation::adjustNextTrack()
{
	if(m_tracks.isEmpty())
	{
		m_nextTrack = -1;
		return;
	}
	if(m_loopMode == LoopTrack && m_currentTrack < m_tracks.count())
	{
		m_nextTrack == m_currentTrack;
		return;
	}

	switch(m_shuffleMode)
	{
		case ShuffleNone:
			if(m_currentTrack + 1 < m_tracks.count())
			{
				m_nextTrack = m_currentTrack + 1;
			}
			else if(m_loopMode == LoopPlaylist)
			{
				m_nextTrack = 0;
			}
			else
			{
				m_nextTrack = -1;
			}
			return;
		case ShuffleTracks:
			// not perfect, but good enough
			m_nextTrack = qrand() % m_tracks.count();
			return;
		case ShuffleAlbums:
			{
				// Pick a random track
				int currentTrack = qrand() % m_tracks.count();
				// Backtrack until we find the first track in an album
				while(true)
				{
					Q_ASSERT(currentTrack >= 0);
					if(currentTrack == 0 || m_tracks.at(currentTrack).album() != m_tracks.at(currentTrack - 1).album())
					{
						m_nextTrack = currentTrack;
						return;
					}
					--currentTrack;
				}
			}
			return;
	}
	emit trackListChanged();
}
