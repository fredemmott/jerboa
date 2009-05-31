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
#ifndef _JERBOA_TRACKDATA_H
#define _JERBOA_TRACKDATA_H

#include "TrackData_p.h"

#include <QSharedDataPointer>
#include <QString>
#include <QUrl>
#include <QVariant> 

namespace Jerboa
{
	/// Represents a track and all the information about it.
	class TrackData
	{
		public:
			/// Construct a TrackData object from the specified data.
			TrackData(
				const QUrl& url,
				const QString& album,
				const QString& albumArtist,
				const QString& albumArtistRomanised,
				const QString& artist,
				const QString& artistRomanised,
				const QString& title,
				quint8 trackNumber,
				qreal albumRG,
				qreal trackRG,
				const QString& musicBrainzID
			);

			/** Construct a TrackData object from mime data.
			 * This should be in the text/x-jerboa-trackdata format.
			 * \see mimeData
			 */
			TrackData(const QString& mimeData);

			/// Copy constructor.
			TrackData(const TrackData &);

			/** Creates an invalid TrackData object.
			 * An object created with this constructor will have failed Q_ASSERTs on all of the members
			 * except \ref isValid, \ref operator==, \ref operator=, and \ref toMpris, unless it is
			 * reinitialised with operator=.
			 */
			TrackData();
			~TrackData();

			/** Compares two TrackData objects for equality.
			 * Two TrackData objects are equal if they are both invalid, or if they both have
			 * identical mime data.
			 */
			bool operator==(const TrackData& other) const;

			/// Opposite of operator==
			bool operator!=(const TrackData& other) const;

			/// Sort TrackData objects by albumArtistRomanised().toLower(), album(), and trackNumber()
			bool operator<(const TrackData& other) const;

			/// If the TrackData object is valid.
			bool isValid() const;
			QUrl url() const;
			QString album() const;
			QString albumArtist() const;
			QString albumArtistRomanised() const;
			QString artist() const;
			QString artistRomanised() const;
			QString title() const;
			quint8 trackNumber() const;

			qreal albumReplayGain() const;
			qreal trackReplayGain() const;
			const QString& musicBrainzId() const;

			/** Mime data for this track.
			 * This is in the format text/x-jerboa-trackdata
			 */
			QString mimeData() const;

			/** MPRIS-formatted metadata.
			 * This returns all the data for this track in an a{sv} format, suitable for use with
			 * MPRIS's Player::GetMetadata call.
			 *
			 * If the TrackData object is invalid, it returns an empty map.
			 */
			QVariantMap mprisData() const;

			/// Human-sensible album sorting
			static bool albumLessThan(const QString& a, const QString& b);
		private:
			QSharedDataPointer<TrackDataPrivate> d;
	};
};

Q_DECLARE_METATYPE(Jerboa::TrackData);
Q_DECLARE_METATYPE(QList<Jerboa::TrackData>);

#endif
