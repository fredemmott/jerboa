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
#ifndef _JERBOA_TRACKDATA_P_H
#define _JERBOA_TRACKDATA_P_H

#include <QSharedData>
#include <QString>
#include <QUrl>

namespace Jerboa
{
	class TrackDataPrivate : public QSharedData
	{
		public:
			// Functions
			void populateMembers();

			// Members
			bool valid;

			QUrl url;
			QString mimeData;
			QString musicBrainzID;

			qreal albumRG;
			qreal trackRG;

			QString album;
			QString albumArtist;
			QString albumArtistRomanised;
			QString artist;
			QString artistRomanised;
			QString title;
			quint8 trackNumber;
	};
}
#endif
