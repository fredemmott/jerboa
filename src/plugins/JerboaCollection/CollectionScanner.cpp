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
, m_tagReader(tagReader)
, m_fileLister(new FileLister(this))
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
		SLOT(skipToNextFile())
	);
	connect(
		m_tagReader,
		SIGNAL(schemeNotSupported(QUrl)),
		this,
		SLOT(skipToNextFile())
	);
}

void CollectionScanner::run()
{
	QSqlDatabase::database().transaction();
	QSettings settings;
	QDir collectionDirectory(settings.value("collection/directory").toString());
	m_addedTracks.clear();
	m_modifiedFiles.clear();
	m_modifiedTracks.clear();
	m_removedFiles.clear();
	m_fileLister->start(collectionDirectory);
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
		emit finished(m_addedTracks, m_modifiedTracks, m_removedFiles);
	}
	else
	{
		m_progress = 0;
		m_total = m_filesToRead.count();
		emit progressChanged(m_progress, m_total);
		m_tagReader->readUrl(QUrl::fromLocalFile(m_filesToRead.takeFirst()));
	}
}

void CollectionScanner::processTrack(const Jerboa::TrackData& track)
{
	Q_ASSERT(m_progress + m_filesToRead.count() + 1 == m_total);

	SqlQuery query;
	QSqlField field("filename", QVariant::String);
	field.setValue(url.toLocalFile());
	QString safeFileName(QSqlDatabase::database().driver()->formatValue(field));
	query.exec(
		QString(
			"DELETE FROM `Tracks` WHERE `FileName` = %1"
		).arg(
			safeFileName
		)
	);

	if(track.isValid())
	{
		QString artist = track.artist();
		QString artistSort = track.artistRomanised();
		QString albumName = track.album();

		// Get an artist ID
		unsigned int artistID = Util::getArtistID(artist, artistSort);

		// Get an album ID
		unsigned int albumID;

		query.prepare("SELECT `ID`, `Artist` FROM `Albums` WHERE `Name` = :album");
		query.bindValue(":album", albumName);
		query.exec();
		query.first();
		if(query.isValid())
		{
			// Already have the album
			albumID = query.record().value(0).toUInt();

			// See if this artist needs to be changed to various artists
			unsigned int albumArtist = query.record().value(1).toUInt();
			if(albumArtist != artistID)
			{
				unsigned int VA = Util::getArtistID(tr("Various Artists"), tr("Various Artists"));
				query.prepare("UPDATE `Albums` SET `Artist`=? WHERE `ID`=?");
				query.addBindValue(VA);
				query.addBindValue(albumID);
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
			query.bindValue(":artist", artistID);
			query.bindValue(":name", albumName);
			query.bindValue(":sortKey", albumSort);
			query.exec();
			albumID = query.lastInsertId().toUInt();
		}

		query.prepare("INSERT INTO `Tracks` (FileName, Album, Artist, Name, TrackNumber, AlbumRG, TrackRG, SearchKey, MusicBrainzTrackID) VALUES (:fileName, :album, :artist, :title, :trackNumber, :albumRG, :trackRG, :searchKey, :mbTrackID)");
		query.bindValue(":fileName", url.toLocalFile());
		query.bindValue(":title", track.title());
		query.bindValue(":trackNumber", track.trackNumber());

		float trackRG = track.trackRG();
		float albumRG = track.albumRG();
		query.bindValue(":albumRG", albumRG);
		query.bindValue(":trackRG", trackRG);
		query.bindValue(":searchKey",  QString("%1 %2 %3 %4").arg(track.title(), albumName, artist, artistSort));
		query.bindValue(":mbTrackID", track.musicBrainzID().isNull() ? "" : track.musicBrainzID());

		query.bindValue(":artist", artistID);
		query.bindValue(":album", albumID);

		query.exec();
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
		emit finished(m_addedTracks, m_modifiedTracks, m_removedFiles);
	}
}
