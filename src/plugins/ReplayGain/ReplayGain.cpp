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
#include "ReplayGain.h"

#include "PlayerInterface.h"

#include <QtPlugin>
#include <QSettings>

ReplayGain::ReplayGain()
	:
		QObject(0),
		m_player(0),
		m_usingReplayGain(true),
		m_enabled(true)
{
}

QString ReplayGain::pluginName() const
{
	return tr("ReplayGain");
}

QString ReplayGain::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString ReplayGain::uniqueId() const
{
	return "org.jerboaplayer.ReplayGain";
}

void ReplayGain::addComponent(ComponentType type, QObject* component)
{
	if(type == Player)
	{
		QSettings settings;
		m_gain = settings.value("replaygain/gain", "8").toString().toFloat();
		m_defaultGain = settings.value("replaygain/defaultReplayGain", "-3").toString().toFloat();
		m_mode = static_cast<ReplayGainMode>(settings.value("replaygain/mode", AlbumMode).toInt());
		m_enabled = settings.value("replaygain/enabled", true).toBool();
	
		m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
		Q_ASSERT(m_player);

		if(m_enabled)
		{
			connect(
				m_player,
				SIGNAL(currentTrackChanged(const Jerboa::TrackData&)),
				this,
				SLOT(adjustReplayGain(const Jerboa::TrackData&))
			);
		}
		m_player->setVolumeDecibel(m_gain);

	}
}

void ReplayGain::adjustReplayGain(const Jerboa::TrackData& track)
{
	Q_ASSERT(m_player);
	if(!(track.isValid() && m_enabled))
	{
		return;
	}

	qreal volume;

	if(m_mode == AlbumMode)
	{
		volume = m_gain - track.albumReplayGain();
	}
	else
	{
		volume = m_gain - track.trackReplayGain();
	}

	if(volume > 9000)
	{
		if(!m_usingReplayGain)
		{
			return;
		}
		m_usingReplayGain = false;
		volume = m_gain - m_defaultGain;
		m_player->setVolumeDecibel(volume);
		return;
	}
	m_usingReplayGain = true;
	m_player->setVolumeDecibel(volume);
}

Q_EXPORT_PLUGIN2(Jerboa_ReplayGain, ReplayGain);
