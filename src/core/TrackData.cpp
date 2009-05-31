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
#include <QRegExp>
#include <QStringList>
#include <QTime>

inline QString albumSortKey(const QString& albumName)
{
	// Need to make a new Album record - this needs a sortKey
	// Eat spaces
	QString albumSort = QString(albumName.toLower()).replace(" ", "");
	// Replaces symbols with ! ("Foo: bar" comes before "Foo 2: Bar")
	albumSort.replace(QRegExp("\\W"), "!");
	// Pad numbers to six figures
	albumSort.replace(QRegExp("(\\d+)"), "00000\\1");
	albumSort.replace(QRegExp("\\d+(\\d{6})"), "\\1");
	return albumSort;
}

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
		: d(new TrackDataPrivate)
	{
		QStringList mimeData;
		mimeData
			   << url.toEncoded()
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

	bool TrackData::operator<(const TrackData& other) const
	{
		const Jerboa::TrackData& a = *this;
		const Jerboa::TrackData& b = other;

		const QString aArtistLower = a.albumArtistRomanised().toLower();
		const QString bArtistLower = b.albumArtistRomanised().toLower();
	
		if(aArtistLower < bArtistLower)
		{
			return true;
		}
		if(bArtistLower < aArtistLower)
		{
			return false;
		}
	
		const QString aAlbumLower = albumSortKey(a.album());
		const QString bAlbumLower = albumSortKey(b.album());
//		const QString aAlbumLower = a.album().toLower();	
//		const QString bAlbumLower = b.album().toLower();	
		if(aAlbumLower < bAlbumLower)
		{
			return true;
		}
		if(bAlbumLower < aAlbumLower)
		{
			return false;
		}
	
		if(a.trackNumber() < b.trackNumber())
		{
			return true;
		}
	
		return false;
	}

	bool TrackData::operator==(const TrackData& other) const
	{
		if(!isValid())
		{
			return ! other.isValid();
		}
		return d->url == other.d->url;
	}

	bool TrackData::operator!=(const TrackData& other) const
	{
		return ! (other == *this);
	}

	TrackData::TrackData(const TrackData& other)
		: d(other.d)
	{
	}

	TrackData::TrackData(const QString& mimeData)
		: d(new TrackDataPrivate)
	{
		d->mimeData = mimeData;
		d->populateMembers();
		d->valid = true;
	};

	TrackData::TrackData() : d(new TrackDataPrivate)
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
