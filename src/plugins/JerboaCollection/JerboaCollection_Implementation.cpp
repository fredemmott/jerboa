#include "JerboaCollection_Implementation.h"

#include "CollectionScanner.h"
#include "TrackData.h"

#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

void JerboaCollection::Implementation::monitorDirectory(const QDir& directory)
{
	if(!m_watcher->directories().contains(directory.absolutePath()))
	{
		m_watcher->addPath(directory.absolutePath());
	}
	Q_FOREACH(const QString& subdirectory, directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		monitorDirectory(directory.absolutePath() + "/" + subdirectory);
	}
}

JerboaCollection::Implementation::Implementation(Jerboa::TagReader* tagReader, QObject* parent)
	:
		CollectionInterface(parent),
		m_directory(QSettings().value("collection/directory").toString()),
		m_collectionScanner(new CollectionScanner(tagReader, this))
{
	connect(
		m_collectionScanner,
		SIGNAL(finished(QList<Jerboa::TrackData>,QList<Jerboa::TrackData>,QStringList)),
		this,
		SLOT(applyChanges(QList<Jerboa::TrackData>,QList<Jerboa::TrackData>,QStringList))
	);
	m_watcher = new QFileSystemWatcher(this);
	monitorDirectory(m_directory);
	connect(
		m_watcher,
		SIGNAL(directoryChanged(QString)),
		this,
		SLOT(rescanTree())
	);

	m_collectionScanner->run();

	QSqlQuery query;
	query.setForwardOnly(true);
	const bool success = query.exec(
		"SELECT FileName, Album, AlbumArtist, AlbumArtistRomanised, Artist, ArtistRomanised, Title, TrackNumber, AlbumRG, TrackRG, MBID FROM TrackData"
	);
	if(!success)
	{
		qFatal("Couldn't read database: %s", qPrintable(query.lastError().text()));
	}
	if(query.size() != -1)
	{
		m_tracks.reserve(query.size() + 10);
	}
	for(query.first(); query.isValid(); query.next())
	{
		m_tracks.append(
			Jerboa::TrackData(
				QUrl::fromLocalFile(query.value(0).toString()),
				query.value(1).toString(), // Album
				query.value(2).toString(), // AlbumArtist
				query.value(3).toString(), // AlbumArtistRomanised
				query.value(4).toString(), // Artist
				query.value(5).toString(), // ArtistRomanised,
				query.value(6).toString(), // Title
				query.value(7).value<quint8>(), // TrackNumber
				query.value(8).value<qreal>(), // AlbumRG
				query.value(9).value<qreal>(), // TrackRG
				query.value(10).toString() // MBID
			)
		);
	}
}

void JerboaCollection::Implementation::rescanTree()
{
	m_collectionScanner->run();
	monitorDirectory(m_directory);
}

void JerboaCollection::Implementation::applyChanges(const QList<Jerboa::TrackData>& added, const QList<Jerboa::TrackData>& modified, const QStringList& removed)
{
	// Added
	Q_FOREACH(const Jerboa::TrackData& track, added)
	{
		Q_ASSERT(track.isValid());
		Q_ASSERT(!m_tracks.contains(track));
		m_tracks.append(track);
	}
	if(!added.isEmpty())
	{
		emit tracksAdded(added);
	}

	// Modified
	Q_FOREACH(const Jerboa::TrackData& track, modified)
	{
		const int index = m_tracks.indexOf(track);
		Q_ASSERT(index >= 0);
		m_tracks.replace(index, track);
	}
	if(!modified.isEmpty())
	{
		emit tracksModified(modified);
	}

	// Removed
	QList<QUrl> removedUrls;
	Q_FOREACH(const Jerboa::TrackData& track, m_tracks)
	{
		if(removed.contains(track.url().toLocalFile()))
		{
			m_tracks.remove(m_tracks.indexOf(track));
			removedUrls.append(track.url());
		}
	}
	if(!removedUrls.isEmpty())
	{
		emit tracksRemoved(removedUrls);
	}
}

QVector<Jerboa::TrackData> JerboaCollection::Implementation::tracks() const
{
	return m_tracks;
}
