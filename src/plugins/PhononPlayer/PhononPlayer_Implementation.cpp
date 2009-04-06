#include "PhononPlayer_Implementation.h"

PhononPlayer::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		Jerboa::PlayerInterface(playlist, parent),
		m_playlist(playlist)
{
	m_state = Stopped;

	m_output = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	m_player = new Phonon::MediaObject(this);
	m_player->setTransitionTime(0);
	
	connect(
		m_player,
		SIGNAL(aboutToFinish()),
		this,
		SLOT(queueNextTrack())
	);

	Phonon::createPath(m_player, m_output);

	connect(
		m_player,
		SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		this,
		SLOT(handlePhononStateChange(Phonon::State, Phonon::State))
	);
}

void PhononPlayer::Implementation::queueNextTrack()
{
	const int nextTrackIndex = m_playlist->nextTrack();
	if(nextTrackIndex != -1)
	{
		const Jerboa::TrackData& track = m_playlist->tracks().at(nextTrackIndex);
		m_currentTrack = track;
		m_player->enqueue(Phonon::MediaSource(track.url()));
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
	m_player->setCurrentSource(Phonon::MediaSource(track.url()));
	setState(Loading);
	m_player->play();
}

void PhononPlayer::Implementation::setState(State state)
{
	m_state = state;
	if(state == Playing)
	{
		const Jerboa::TrackData& playlistTrack = m_playlist->tracks().at(m_playlist->currentTrack());
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
			setState(Playing);
			return;
		case Phonon::PausedState:
			setState(Paused);
			return;
		default:
			setState(Stopped);
	}
}
