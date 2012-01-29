
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
#ifndef _REPLAY_GAIN_H_
#define _REPLAY_GAIN_H_

#include "TrackData.h"
#include "JerboaPlugin.h"

class ReplayGain: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		ReplayGain();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	private slots:
		void adjustReplayGain(const Jerboa::TrackData&);
	private:
		enum ReplayGainMode {
			/// Normalise volume across a whole album; relative volume differences are kept
			AlbumMode,
			/// Normalise each track separately - relative differences between tracks in the album are lost
			TrackMode
		};

		Jerboa::PlayerInterface* m_player;

		bool m_usingReplayGain;

		qreal m_defaultGain;
		qreal m_gain;
		ReplayGainMode m_mode;
		bool m_enabled;
};

#endif
