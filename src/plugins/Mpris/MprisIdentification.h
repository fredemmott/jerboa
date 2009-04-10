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
#ifndef _MPRIS_IDENTIFICATION_H
#define _MPRIS_IDENTIFICATION_H

#include <QDBusAbstractAdaptor>
#include <QDBusArgument>

struct MprisSpecVersion
{
	quint16 major;
	quint16 minor;
};
QDBusArgument &operator<<(QDBusArgument&, const MprisSpecVersion&);
const QDBusArgument &operator>>(const QDBusArgument&, MprisSpecVersion&);
Q_DECLARE_METATYPE(MprisSpecVersion);

class MprisIdentification : public QObject
{
	Q_OBJECT;
	Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer");
	public:
		MprisIdentification(QObject* parent = NULL);
	public slots:
		QString Identity() const;
		void Quit();
		MprisSpecVersion MprisVersion() const;
};

#endif
