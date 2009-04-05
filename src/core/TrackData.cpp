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
#include "TrackData.h"
#include "TrackData_p.h"

#include <QDebug>
#include <QStringList>

namespace Jerboa
{
	TrackData::TrackData(
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
		const QString& musicBrainzID)
		: d(new TrackData::Private)
	{
		QStringList mimeData;
		mimeData
			   << url.toString()
			   << album
			   << albumArtist
			   << albumArtistRomanised
			   << artist
			   << artistRomanised
			   << title
			   << QString::number(trackNumber)
			   << QString::number(albumRG)
			   << QString::number(trackRG)
			   << musicBrainzID;
		d->mimeData = mimeData.join("\t");
		d->populateMembers();
		d->valid = true;
	};

	bool TrackData::operator==(const TrackData& other) const {
		if ( ! isValid() ) return ! other.isValid();
		return d->mimeData == other.d->mimeData;
	}

	TrackData::TrackData(const TrackData& other)
		: d(other.d)
	{
	}

	TrackData::TrackData(const QString& mimeData)
		: d(new TrackData::Private)
	{
		d->mimeData = mimeData;
		d->populateMembers();
		d->valid = true;
	};

	TrackData::TrackData() : d(new TrackData::Private)
	{
		d->valid = false;
	}

	TrackData::~TrackData() {}

	bool TrackData::isValid() const { return d->valid; };

	QUrl TrackData::url() const { Q_ASSERT(d->valid);  return d->url; };

	QString TrackData::album() const { Q_ASSERT(d->valid);  return d->album; };
	QString TrackData::albumArtist() const { Q_ASSERT(d->valid);  return d->albumArtist; };
	QString TrackData::albumArtistRomanised() const { Q_ASSERT(d->valid);  return d->albumArtistRomanised; };
	QString TrackData::artist() const { Q_ASSERT(d->valid);  return d->artist; };
	QString TrackData::artistRomanised() const { Q_ASSERT(d->valid);  return d->artistRomanised; };
	QString TrackData::title() const { Q_ASSERT(d->valid);  return d->title; };
	quint8 TrackData::trackNumber() const { Q_ASSERT(d->valid);  return d->trackNumber; };
	qreal TrackData::albumReplayGain() const { Q_ASSERT(d->valid);  return d->albumRG; };
	qreal TrackData::trackReplayGain() const { Q_ASSERT(d->valid);  return d->trackRG; };
	const QString& TrackData::musicBrainzId() const { Q_ASSERT(d->valid);  return d->musicBrainzID; };

	QString TrackData::mimeData() const { Q_ASSERT(d->valid);  return d->mimeData; };

	QVariantMap TrackData::mprisData() const
	{
		if(! isValid() ) return QVariantMap();
		QVariantMap r;
		r["location"] = url().toString();
		r["title"] = title();
		r["artist"] = artist();
		r["album"] = album();
		r["tracknumber"] = QString::number(trackNumber());
		r["mb track id"] = musicBrainzId();
		return r;
	}
};
