#include "PhononPlayer_Implementation.h"

#include <QDebug>

PhononPlayer::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		Jerboa::PlayerInterface(playlist, parent),
		m_playlist(playlist)
{
	m_state = Stopped;

	m_output = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	m_player = new Phonon::MediaObject(this);
	m_player->setTransitionTime(0);
	
	Phonon::createPath(m_player, m_output);

	connect(
		m_player,
		SIGNAL(aboutToFinish()),
		this,
		SLOT(queueNextTrack())
	);
	connect(
		m_player,
		SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		this,
		SLOT(handlePhononStateChange(Phonon::State, Phonon::State))
	);
	connect(
		m_player,
		SIGNAL(finished()),
		this,
		SLOT(stopAndClear())
	);
}

void PhononPlayer::Implementation::stopAndClear()
{
	setCurrentTrack(Jerboa::TrackData());
}

qreal PhononPlayer::Implementation::volume() const
{
	return m_output->volume();
}

qint64 PhononPlayer::Implementation::trackLength() const
{
	if(m_currentTrack.isValid())
	{
		return m_player->totalTime();
	}
	else
	{
		return -1;
	}
}

qreal PhononPlayer::Implementation::volumeDecibel() const
{
	return m_output->volumeDecibel();
}

void PhononPlayer::Implementation::setVolume(qreal value)
{
	m_output->setVolume(value);
	emit volumeChanged(value);
}

void PhononPlayer::Implementation::setVolumeDecibel(qreal value)
{
	m_output->setVolumeDecibel(value);
	emit volumeChanged(volume());
}

void PhononPlayer::Implementation::queueNextTrack()
{
	const int nextTrackIndex = m_playlist->nextTrack();
	if(nextTrackIndex != -1)
	{
		const Jerboa::TrackData& track = m_playlist->tracks().at(nextTrackIndex);
		m_currentTrack = track;
		if(track.url().scheme() == "file")
		{
			m_player->enqueue(Phonon::MediaSource(track.url().toLocalFile()));
		}
		else
		{
			m_player->enqueue(Phonon::MediaSource(track.url()));
		}
	}
}

Jerboa::PlayerInterface::State PhononPlayer::Implementation::state() const
{
	return m_state;
}

Jerboa::TrackData PhononPlayer::Implementation::currentTrack() const
{
	return m_currentTrack;
}

void PhononPlayer::Implementation::setCurrentTrack(const Jerboa::TrackData& track)
{
	m_player->stop();
	m_currentTrack = track;
	emit currentTrackChanged(m_currentTrack);
	if(track.isValid())
	{
		// Oh, oh, oh, FAIL
		if(track.url().scheme() == "file")
		{
			m_player->setCurrentSource(Phonon::MediaSource(track.url().toLocalFile()));
		}
		else
		{
			m_player->setCurrentSource(Phonon::MediaSource(track.url()));
		}
		setState(Loading);
		m_player->play();
	}
	else
	{
		m_player->setCurrentSource(Phonon::MediaSource());
		m_player->stop();
		setState(Stopped);
	}
}

void PhononPlayer::Implementation::setState(State state)
{
	m_state = state;
	if(state == Playing)
	{
		const Jerboa::TrackData& playlistTrack(
			m_playlist->currentTrack() == -1
			? Jerboa::TrackData()
			: m_playlist->tracks().at(m_playlist->currentTrack())
		);
		if(playlistTrack != m_currentTrack)
		{
			const int nextTrackId = m_playlist->nextTrack();
			if(nextTrackId != -1)
			{
				const Jerboa::TrackData& nextTrack = m_playlist->tracks().at(nextTrackId);
				if(m_currentTrack == nextTrack)
				{
					m_playlist->setCurrentTrack(nextTrackId);
				}
			}
		}
	}
	emit stateChanged(state);
}

void PhononPlayer::Implementation::play()
{
	m_player->play();
}

void PhononPlayer::Implementation::pause()
{
	m_player->pause();
}

void PhononPlayer::Implementation::stop()
{
	m_player->stop();
}

void PhononPlayer::Implementation::handlePhononStateChange(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);
	switch(newState)
	{
		case Phonon::LoadingState:
		case Phonon::BufferingState:
			emit currentTrackChanged(m_currentTrack);
			setState(Loading);
			return;
		case Phonon::PlayingState:
			emit currentTrackChanged(m_currentTrack);
			setState(Playing);
			return;
		case Phonon::PausedState:
			setState(Paused);
			return;
		case Phonon::ErrorState:
			qDebug() << "Error string:" << m_player->errorString() << m_player->errorType() << m_currentTrack.isValid() << newState << m_output->name() << m_output->isMuted() << m_output->volume() << m_output->volumeDecibel() << m_output->isValid() << m_output->outputDevice().name();
		default:
			setState(Stopped);
	}
}
