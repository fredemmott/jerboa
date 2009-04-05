#include "PhononPlayer_Implementation.h"

PhononPlayer::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		Jerboa::PlayerInterface(playlist, parent)
{
	m_state = Stopped;

	m_output = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	m_player = new Phonon::MediaObject(this);
	Phonon::createPath(m_player, m_output);
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
	emit currentTrackChanged(track);
	setState(Loading);
	m_player->play();
}

void PhononPlayer::Implementation::setState(State state)
{
	m_state = state;
	emit stateChanged(state);
}

void PhononPlayer::Implementation::play()
{
}

void PhononPlayer::Implementation::pause()
{
}

void PhononPlayer::Implementation::stop()
{
}
