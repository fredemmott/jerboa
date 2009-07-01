/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _JERBOA_MPRIS_PLAYER
#define _JERBOA_MPRIS_PLAYER

#include "PlayerInterface.h"
#include "PlaylistInterface.h"

#include <QDBusAbstractAdaptor>
#include <QDBusArgument>
#include <QVariant>

// Defined in MPRIS spec
struct MprisState
{
	int playState; // 0->playing, 1->paused, 2->stopped
	int randomState; // 0->linear, 1->shuffle
	int repeatState; // 0->linear, 1->loop track
	int isInfinite; // 0->end at end of playlist, 1->loop playlist (or some kind of shuffle)
};

QDBusArgument &operator<<(QDBusArgument&, const MprisState&);
const QDBusArgument &operator>>(const QDBusArgument&, MprisState&);
Q_DECLARE_METATYPE(MprisState);

class MprisPlayer : public QObject
{
	Q_OBJECT;
	Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer");
	public:
		MprisPlayer(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QObject* parent);
	public slots:
		void Play();
		void Pause();
		void Stop();
		void Next();
		void Prev();
		void Repeat(bool);
		int GetCaps() const;
		MprisState GetStatus() const;
		QVariantMap GetMetadata() const;
		int VolumeGet() const;
		void VolumeSet(int);
		int PositionGet() const;
		void PositionSet(int);
	signals:
		void StatusChange(const MprisState&);
		void CapsChange(int);
		void TrackChange(const QVariantMap&);
	private slots:
		void updateStatus();
		void updateCaps();
		void updateTrack();
	private:
		enum Capabilities // as defined in spec
		{
			NONE = 0,
			CAN_GO_NEXT = 1 << 0,
			CAN_GO_PREV = 1 << 1,
			CAN_PAUSE = 1 << 2,
			CAN_PLAY = 1 << 3,
			CAN_SEEK = 1 << 4,
			CAN_PROVIDE_METADATA = 1 << 5,
			CAN_HAS_TRACKLIST = 1 << 6
		};

		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};
#endif
