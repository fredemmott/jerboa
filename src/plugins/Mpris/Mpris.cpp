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
#include "Mpris.h"

#include <QtDBus>
#include <QtPlugin>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>

#include "MprisIdentification.h"
#include "MprisPlayer.h"
#include "MprisTracklist.h"

Mpris::Mpris()
	:
		QObject(),
		m_playlist(0),
		m_player(0)
{
	qDBusRegisterMetaType<MprisState>();
	qDBusRegisterMetaType<MprisSpecVersion>();
}

QString Mpris::pluginName() const
{
	return tr("MPRIS D-Bus Interface");
}

QString Mpris::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString Mpris::uniqueId() const
{
	return "org.jerboaplayer.Mpris";
}

void Mpris::addComponent(ComponentType type, QObject* component)
{
	if(type == PlaylistSource || type == Player)
	{
		if(type == PlaylistSource)
		{
			m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
			Q_ASSERT(m_playlist);
		}
		else
		{
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
		}
		if(m_player && m_playlist)
		{
			createInstance();
		}
	}
}

void Mpris::createInstance()
{
	QDBusConnection::sessionBus().registerService("org.mpris." + QCoreApplication::applicationName().toLower());

	new MprisIdentification(QCoreApplication::instance());
	new MprisPlayer(m_player, m_playlist, QCoreApplication::instance());
	new MprisTracklist(m_player, m_playlist, QCoreApplication::instance());
}

Q_EXPORT_PLUGIN2(Jerboa_Mpris, Mpris);
