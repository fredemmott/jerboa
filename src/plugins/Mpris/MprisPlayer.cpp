/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "MprisPlayer.h"

#include "Types.h"
#include "MprisPlayerAdaptor.h"

MprisPlayer::MprisPlayer(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		QObject(parent),
		m_player(player),
		m_playlist(playlist)
{
	new MprisPlayerAdaptor(this);

	connect(m_player, SIGNAL(stateChanged(Jerboa::PlayerInterface::State)), this, SLOT(updateStatus()));
	connect(m_player, SIGNAL(stateChanged(Jerboa::PlayerInterface::State)), this, SLOT(updateCaps()));
	connect(m_player, SIGNAL(currentTrackChanged(Jerboa::TrackData)), this, SLOT(updateStatus()));
	connect(m_player, SIGNAL(currentTrackChanged(Jerboa::TrackData)), this, SLOT(updateCaps()));
	connect(m_player, SIGNAL(currentTrackChanged(Jerboa::TrackData)), this, SLOT(updateTrack()));
	connect(m_playlist, SIGNAL(loopModeChanged(Jerboa::PlaylistInterface::LoopMode)), this, SLOT(updateStatus()));
	connect(m_playlist, SIGNAL(loopModeChanged(Jerboa::PlaylistInterface::LoopMode)), this, SLOT(updateCaps()));
	connect(m_playlist, SIGNAL(shuffleModeChanged(Jerboa::PlaylistInterface::ShuffleMode)), this, SLOT(updateStatus()));
	connect(m_playlist, SIGNAL(shuffleModeChanged(Jerboa::PlaylistInterface::ShuffleMode)), this, SLOT(updateCaps()));

	QDBusConnection::sessionBus().registerObject("/Player", this);
}

void MprisPlayer::Play() { m_player->play(); }
void MprisPlayer::Pause() { m_player->pause(); }
void MprisPlayer::Stop() { m_player->stop(); }
void MprisPlayer::Next() { m_player->next(); }
void MprisPlayer::Prev() { m_player->previous(); }

void MprisPlayer::Repeat(bool repeatCurrentTrack)
{
	if(repeatCurrentTrack)
	{
		m_playlist->setLoopMode(Jerboa::PlaylistInterface::LoopTrack);
	}
	else
	{
		m_playlist->setLoopMode(Jerboa::PlaylistInterface::LoopNone);
	}
}

QVariantMap MprisPlayer::GetMetadata() const
{
	if(m_playlist->currentTrack() == -1)
	{
		return QVariantMap();
	}
	else
	{
		return m_playlist->tracks().at(m_playlist->currentTrack()).mprisData();
	}
}

int MprisPlayer::GetCaps() const
{
	int caps = NONE;
	const Jerboa::PlayerInterface::State state = m_player->state();

	if(m_playlist->nextTrack() != -1)
	{
		caps |= CAN_GO_NEXT;
	}
	if(m_playlist->currentTrack() > 0)
	{
		caps |= CAN_GO_PREV;
	}
	if(state == Jerboa::PlayerInterface::Playing)
	{
		caps |= CAN_PAUSE;
	}
	if(
		state == Jerboa::PlayerInterface::Paused
		||
		(
			state == Jerboa::PlayerInterface::Stopped
			&&
			m_playlist->currentTrack() != -1
		)
	)
	{
		caps |= CAN_PLAY;
	}
	if(m_player->features() & Jerboa::PlayerInterface::Position)
	{
		caps |= CAN_SEEK;
	}
	caps |= CAN_PROVIDE_METADATA;
	caps |= CAN_HAS_TRACKLIST;

	return caps;
}

int MprisPlayer::PositionGet() const
{
	return m_player->position();
}

void MprisPlayer::PositionSet(int v)
{
	m_player->setPosition(v);
}

MprisState MprisPlayer::GetStatus() const
{
	MprisState state;
	switch(m_player->state())
	{
		case Jerboa::PlayerInterface::Playing:
			state.playState = 0;
			break;
		case Jerboa::PlayerInterface::Paused:
			state.playState = 1;
			break;
		default:
			state.playState = 2;
	}
	if(m_playlist->shuffleMode() == Jerboa::PlaylistInterface::ShuffleNone)
	{
		state.randomState = 0;
	}
	else
	{
		state.randomState = 1;
	}
	if(m_playlist->loopMode() == Jerboa::PlaylistInterface::LoopTrack)
	{
		state.repeatState = 1;
	}
	else
	{
		state.repeatState = 0;
	}
	if(m_playlist->loopMode() == Jerboa::PlaylistInterface::LoopNone && m_playlist->shuffleMode() == Jerboa::PlaylistInterface::ShuffleNone)
	{
		state.isInfinite = 0;
	}
	else
	{
		state.isInfinite = 1;
	}
	return state;
}

void MprisPlayer::VolumeSet(int v)
{
	m_player->setVolume(static_cast<qreal>(v)/100);
}

int MprisPlayer::VolumeGet() const
{
	return m_player->volume() * 100;
}

void MprisPlayer::updateStatus()
{
	emit StatusChange(GetStatus());
}

void MprisPlayer::updateCaps()
{
	emit CapsChange(GetCaps());
}

void MprisPlayer::updateTrack()
{
	emit TrackChange(GetMetadata());
}

QDBusArgument &operator<<(QDBusArgument& a, const MprisState& s)
{
	a.beginStructure();
	a << s.playState << s.randomState << s.repeatState << s.isInfinite;
	a.endStructure();
	return a;
}

const QDBusArgument &operator>>(const QDBusArgument& a, MprisState& s)
{
	a.beginStructure();
	a >> s.playState >> s.randomState >> s.repeatState >> s.isInfinite;
	a.endStructure();
	return a;
}
