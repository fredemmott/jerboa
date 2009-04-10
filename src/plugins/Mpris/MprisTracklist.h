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
#ifndef _MPRIS_TRACKLIST_H
#define _MPRIS_TRACKLIST_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"

#include <QVariant>
#include <QString>

class MprisTracklist : public QObject
{
        Q_OBJECT;
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer");
        public:
                MprisTracklist(Jerboa::PlayerInterface*, Jerboa::PlaylistInterface*, QObject* parent = NULL);
        public slots:
                QVariantMap GetMetadata(int position);
                int GetCurrentTrack();
                int GetLength();
                int AddTrack(const QString& url, bool skipTo);
                void DelTrack(int position);
                void SetLoop(bool looping);
                void SetRandom(bool shuffleTracks);
        signals:
                void TrackListChange(int length);
	private slots:
		void updateTrackList();
        private:
                Jerboa::PlayerInterface* m_player;
                Jerboa::PlaylistInterface* m_playlist;
};

#endif
