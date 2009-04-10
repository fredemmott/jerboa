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
#include "MprisTracklist.h"

#include "PlayerInterface.h"
#include "PlaylistInterface.h"

#include "MprisTracklistAdaptor.h"

#include <QDBusConnection>

MprisTracklist::MprisTracklist(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QObject* parent)
        :
                QObject(parent),
                m_player(player),
                m_playlist(playlist)
{
        new MprisTracklistAdaptor(this);
	connect(m_playlist, SIGNAL(tracksAdded(int, QList<Jerboa::TrackData>)), this, SLOT(updateTrackList()));
	connect(m_playlist, SIGNAL(tracksRemoved(int, int)), this, SLOT(updateTrackList()));
        QDBusConnection::sessionBus().registerObject("/TrackList", this);
}

void MprisTracklist::updateTrackList()
{
	emit TrackListChange(m_playlist->tracks().count());
}

QVariantMap MprisTracklist::GetMetadata(int position)
{
        if(position >= 0 && position < m_playlist->tracks().count())
        {
                return m_playlist->tracks().at(position).mprisData();
        }
        else
        {
                return QVariantMap();
        }
}

int MprisTracklist::GetCurrentTrack()
{
        return m_playlist->currentTrack();
}

int MprisTracklist::GetLength()
{
        return m_playlist->tracks().count();
}

int MprisTracklist::AddTrack(const QString& url, bool skipTo)
{
        Q_UNUSED(url);
        Q_UNUSED(skipTo);
        return -1;
}

void MprisTracklist::DelTrack(int position)
{
        if(position >= 0 && position < m_playlist->tracks().count())
        {
                m_playlist->removeTracks(position, 1);
        }
}

void MprisTracklist::SetLoop(bool looping)
{
        m_playlist->setLoopMode(looping ? Jerboa::PlaylistInterface::LoopPlaylist : Jerboa::PlaylistInterface::LoopNone);
}

void MprisTracklist::SetRandom(bool shuffleTracks)
{
        m_playlist->setShuffleMode(shuffleTracks ? Jerboa::PlaylistInterface::ShuffleTracks : Jerboa::PlaylistInterface::ShuffleNone);
}
