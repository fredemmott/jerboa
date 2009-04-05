#include "PlayerInterface.h"

#include "PlaylistInterface.h"

namespace Jerboa
{
	PlayerInterface::PlayerInterface(PlaylistInterface* playlist, QObject* parent)
		:
			QObject(parent),
			m_playlist(playlist)
	{
		Q_ASSERT(playlist);
	}

	PlayerInterface::~PlayerInterface()
	{
	}

	PlayerInterface::Features PlayerInterface::features() const
	{
		return Features();
	}

	qreal PlayerInterface::volume() const
	{
		return 0;
	}

	qreal PlayerInterface::volumeDecibel() const
	{
		return 0;
	}

	quint64 PlayerInterface::position() const
	{
		return 0;
	}

	void PlayerInterface::setVolume(qreal value)
	{
		Q_UNUSED(value);
	}

	void PlayerInterface::setVolumeDecibel(qreal value)
	{
		Q_UNUSED(value);
	}

	void PlayerInterface::setPosition(quint64 position)
	{
		Q_UNUSED(position);
	}

	void PlayerInterface::next()
	{
		const int nextTrackIndex = m_playlist->nextTrack();
		skipTo(nextTrackIndex);
	}

	void PlayerInterface::skipTo(int playlistIndex)
	{
		if(playlistIndex != -1)
		{
			const TrackData track = m_playlist->tracks().at(playlistIndex);
			setCurrentTrack(track);
			m_playlist->setCurrentTrack(playlistIndex);
		}
		else
		{
			stop();
			m_playlist->setCurrentTrack(-1);
		}
	}
}
