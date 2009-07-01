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
#include "MprisIdentification.h"

#include "MprisIdentificationAdaptor.h"

#include <QCoreApplication>
#include <QtDBus>

MprisIdentification::MprisIdentification(QObject* parent)
	: QObject(parent)
{
	new MprisIdentificationAdaptor(this);
	QDBusConnection::sessionBus().registerObject("/", this);
}

QString MprisIdentification::Identity() const
{
	return QString("%1 %2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion());
}

void MprisIdentification::Quit()
{
	QCoreApplication::quit();
}

MprisSpecVersion MprisIdentification::MprisVersion() const
{
	MprisSpecVersion r;
	r.major = 1;
	r.minor = 0;
	return r;
}

QDBusArgument &operator<<(QDBusArgument& a, const MprisSpecVersion& v)
{
	a.beginStructure();
	a << v.major << v.minor;
	a.endStructure();
	return a;
}

const QDBusArgument &operator>>(const QDBusArgument& a, MprisSpecVersion& v)
{
	a.beginStructure();
	a >> v.major >> v.minor;
	a.endStructure();
	return a;
}
