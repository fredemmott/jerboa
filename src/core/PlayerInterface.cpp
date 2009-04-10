#include "PlayerInterface.h"
#include "PlayerInterface_p.h"

#include "PlaylistInterface.h"

#include <QDebug>

namespace Jerboa
{
	PlayerInterface::PlayerInterface(PlaylistInterface* playlist, QObject* parent)
		:
			QObject(parent),
			d(new PlayerInterface::Private(playlist, this))
	{
		Q_ASSERT(playlist);
	}

	qint64 PlayerInterface::trackLength() const
	{
		return -1;
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
		qDebug() << "Asked to set volume decibel to" << value << ", but not implemented.";
		Q_UNUSED(value);
	}

	void PlayerInterface::setPosition(quint64 position)
	{
		Q_UNUSED(position);
	}

	void PlayerInterface::previous()
	{
		if(d->m_playlist->currentTrack() >= 0)
		{
			skipTo(d->m_playlist->currentTrack() - 1);
		}
	}

	void PlayerInterface::next()
	{
		const int nextTrackIndex = d->m_playlist->nextTrack();
		skipTo(nextTrackIndex);
	}

	void PlayerInterface::skipTo(int playlistIndex)
	{
		if(playlistIndex != -1)
		{
			const TrackData track = d->m_playlist->tracks().at(playlistIndex);
			setCurrentTrack(track);
			d->m_playlist->setCurrentTrack(playlistIndex);
		}
		else
		{
			stop();
			d->m_playlist->setCurrentTrack(-1);
		}
	}
}
