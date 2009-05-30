/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "CollectionScanner.h"

#include "FileLister.h"
#include "TrackData.h"

#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>

CollectionScanner::CollectionScanner(Jerboa::TagReader* tagReader, QObject* parent)
: QObject(parent)
, m_tagReader(tagReader->instance(this))
, m_fileLister(new FileLister(this))
, m_inProgress(false)
{
	connect(
		m_fileLister,
		SIGNAL(finished(QStringList)),
		this,
		SLOT(haveFileList(QStringList))
	);
	connect(
		m_tagReader,
		SIGNAL(finished(Jerboa::TrackData)),
		this,
		SLOT(processTrack(Jerboa::TrackData))
	);
	connect(
		m_tagReader,
		SIGNAL(notFound(QUrl)),
		this,
		SLOT(skipToNextFile(QUrl))
	);
	connect(
		m_tagReader,
		SIGNAL(schemeNotSupported(QUrl)),
		this,
		SLOT(skipToNextFile(QUrl))
	);
}

void CollectionScanner::skipToNextFile(const QUrl& url)
{
	qDebug() << "Skipping" << url;
	if(!m_filesToRead.isEmpty())
	{
		++m_progress;
		m_tagReader->readUrl(QUrl::fromLocalFile(m_filesToRead.takeFirst()));
	}
	else
	{
		QSettings settings;
		settings.setValue("collection/lastScanned", QDateTime::currentDateTime());
		QSqlDatabase::database().commit();
		finish();
	}
}

void CollectionScanner::run()
{
	if(m_inProgress)
	{
		m_reRun = true;
		return;
	}
	m_reRun = false;
	m_inProgress = true;

	QSqlDatabase::database().transaction();
	QSettings settings;
	QDir collectionDirectory(settings.value("collection/directory").toString());
	m_addedTracks.clear();
	m_modifiedFiles.clear();
	m_modifiedTracks.clear();
	m_removedFiles.clear();
	m_fileLister->start(collectionDirectory);
}

void CollectionScanner::finish()
{
	qDebug() << "Emitting finished with" << m_addedTracks.count() << "new tracks";
	emit finished(m_addedTracks, m_modifiedTracks, m_removedFiles);
	m_inProgress = false;
	if(m_reRun)
	{
		run();
	}
}

void CollectionScanner::haveFileList(const QStringList& files)
{
	QSettings settings;
	QDateTime lastScanned = settings.value("collection/lastscanned", QDateTime::fromTime_t(0)).toDateTime();

	m_filesToRead = files;

	QSqlQuery query;
	QSqlQuery innerQuery;
	query.exec("SELECT FileName, ID FROM Tracks");
	for(query.first(); query.isValid(); query.next())
	{
		QString fileName = query.value(0).toString();
		if(!QFile::exists(fileName))
		{
			m_removedFiles.append(fileName);
			--m_total;
			innerQuery.exec(
				QString(
					"DELETE FROM `Tracks` WHERE `ID` = %1"
				).arg(
					query.value(1).value<quint32>()
				)
			);
		}
		else
		{
			if(QFileInfo(fileName).lastModified() < lastScanned)
			{
				m_filesToRead.removeOne(fileName);
			}
			else
			{
				if(!m_filesToRead.contains(fileName))
				{
					m_filesToRead.append(fileName);
				}
				m_modifiedFiles.append(fileName);
			}
		}
	}

	if(m_filesToRead.isEmpty())
	{
		finish();
	}
	else
	{
		m_progress = 0;
		m_total = m_filesToRead.count();
		m_tagReader->readUrl(QUrl::fromLocalFile(m_filesToRead.takeFirst()));
		emit progressChanged(m_progress, m_total);
	}
}

void CollectionScanner::processTrack(const Jerboa::TrackData& track)
{
	qDebug() << Q_FUNC_INFO << track.url();
	Q_ASSERT(m_progress + m_filesToRead.count() + 1 == m_total);

	QSqlQuery query;
	query.prepare("DELETE FROM Tracks WHERE FileName = :fileName");
	query.bindValue(":fileName", track.url().toLocalFile());
	query.exec();

	if(track.isValid())
	{
		QString artist = track.artist();
		QString artistSort = track.artistRomanised();
		QString albumName = track.album();

		// Get an artist ID
		unsigned int artistId(this->artistId(artist, artistSort));

		// Get an album ID
		unsigned int albumId;

		query.prepare("SELECT ID, Artist FROM Albums WHERE Name = :album");
		query.bindValue(":album", albumName);
		query.exec();
		query.first();
		if(query.isValid())
		{
			// Already have the album
			albumId = query.record().value(0).toUInt();

			// See if this artist needs to be changed to various artists
			unsigned int albumArtist = query.record().value(1).toUInt();
			if(albumArtist != artistId)
			{
				unsigned int variousArtists(this->artistId(tr("Various Artists"), tr("Various Artists")));
				query.prepare("UPDATE Albums SET Artist=? WHERE ID=?");
				query.addBindValue(variousArtists);
				query.addBindValue(albumId);
				query.exec();
			}
		}
		else
		{
			// Need to make a new Album record - this needs a sortKey
			// Eat spaces
			QString albumSort = QString(albumName).replace(" ", "");
			// Replaces symbols with ! ("Foo: bar" comes before "Foo 2: Bar")
			albumSort.replace(QRegExp("\\W"), "!");
			// Pad numbers to six figures
			albumSort.replace(QRegExp("(\\d+)"), "00000\\1");
			albumSort.replace(QRegExp("\\d+(\\d{6})"), "\\1");

			// Create the record
			query.prepare("INSERT INTO `Albums` (`Artist`, `Name`, `SortKey`) VALUES (:artist, :name, :sortKey)");
			query.bindValue(":artist", artistId);
			query.bindValue(":name", albumName);
			query.bindValue(":sortKey", albumSort);
			query.exec();
			albumId = query.lastInsertId().toUInt();
		}

		query.prepare("INSERT INTO `Tracks` (FileName, Album, Artist, Name, TrackNumber, AlbumRG, TrackRG, SearchKey, MusicBrainzTrackID) VALUES (:fileName, :album, :artist, :title, :trackNumber, :albumRG, :trackRG, :searchKey, :mbTrackID)");
		query.bindValue(":fileName", track.url().toLocalFile());
		query.bindValue(":title", track.title());
		query.bindValue(":trackNumber", track.trackNumber());

		query.bindValue(":albumRG", track.albumReplayGain());
		query.bindValue(":trackRG", track.trackReplayGain());
		query.bindValue(":searchKey",  QString("%1 %2 %3 %4").arg(track.title(), albumName, artist, artistSort));
		query.bindValue(":mbTrackID", track.musicBrainzId().isNull() ? "" : track.musicBrainzId());

		query.bindValue(":artist", artistId);
		query.bindValue(":album", albumId);

		query.exec();

		if(m_modifiedFiles.contains(track.url().toLocalFile()))
		{
			m_modifiedTracks.append(track);
		}
		else
		{
			m_addedTracks.append(track);
		}
	}

	emit progressChanged(++m_progress, m_total);
	if(!m_filesToRead.isEmpty())
	{
		m_tagReader->readUrl(QUrl::fromLocalFile(m_filesToRead.takeFirst()));
	}
	else
	{
		QSettings settings;
		settings.setValue("collection/lastScanned", QDateTime::currentDateTime());
		QSqlDatabase::database().commit();
		finish();
	}
}

unsigned int CollectionScanner::artistId(const QString& artist, const QString& artistSort) const
{
	QSqlQuery query;
	query.prepare("SELECT ID FROM Artists WHERE Name = :artist");
	query.bindValue(":artist", artist);
	query.exec();
	query.first();
	if(query.isValid())
	{
		// We already have this artist
		return query.record().value(0).toUInt();
	}
	else
	{
		// Need a new artist - firstly, get a romanised form of their name from the sortkey
		const QStringList artistRomanisedBackward = artistSort.split(", ");
		QStringList artistRomanisedForward;

		// Reversed
		QStringListIterator it(artistRomanisedBackward);
		it.toBack();
		while(it.hasPrevious())
		{
			artistRomanisedForward.append(it.previous());
		}
		const QString artistRomanised = artistRomanisedForward.join(" ");

		// Now, insert it into the database
		query.prepare("INSERT INTO `Artists` (`Name`, `RomanisedName`, `SortKey`) VALUES (:name, :roman, :sort)");
		query.bindValue(":name", artist);
		query.bindValue(":roman", artistRomanised);
		query.bindValue(":sort", artistSort);
		query.exec();
		return query.lastInsertId().toUInt();
	}
}
