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
#include "TrackData_p.h"

#include <QStringList>

namespace Jerboa
{
	void TrackData::Private::populateMembers()
	{
		QStringList mimeData = QString::fromUtf8(this->mimeData).split("\t");
		Q_ASSERT(mimeData.size() == 11);
		url = QUrl::fromEncoded(mimeData[0].trimmed().toUtf8());
		album = mimeData[1];
		albumArtist = mimeData[2];
		albumArtistRomanised = mimeData[3];
		artist = mimeData[4];
		artistRomanised = mimeData[5];
		title = mimeData[6];
		trackNumber = mimeData[7].toUInt();
		albumRG = mimeData[8].toFloat();
		trackRG = mimeData[9].toFloat();
		musicBrainzID = mimeData[10].trimmed();
	}
}
