#include "CollectionTagFetcher.h"

#include "LastFmTagFetcher.h"

#include <QDateTime>

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

CollectionTagFetcher::CollectionTagFetcher(Jerboa::CollectionInterface* collection, QObject* parent)
: QObject(parent)
, m_tagFetcher(new LastFmTagFetcher(this))
{
	Q_ASSERT(collection);
	connect(
		m_tagFetcher,
		SIGNAL(gotTags(QMap<unsigned int, QList<LastFmTagFetcher::Tag> >)),
		this,
		SLOT(saveTags(QMap<unsigned int, QList<LastFmTagFetcher::Tag> >))
	);

	QDateTime lastValid = QDateTime::currentDateTime();
	lastValid.addDays(-5);

	QSqlQuery query;
	query.prepare("DELETE FROM TaggedFiles WHERE LastFetch < :lastValid");
	query.bindValue(":lastValid", lastValid.toTime_t());
	query.exec();

	query.exec("DELETE FROM Tags WHERE FileID NOT IN (SELECT ID FROM TaggedFiles)");

	QSet<QString> ignoredFiles;
	query.exec("SELECT FileName FROM TaggedFiles");
	for(query.first(); query.isValid(); query.next())
	{
		ignoredFiles.insert(query.value(0).toString());
	}

	QList<LastFmTagFetcher::Track> tracks;
	Q_FOREACH(const Jerboa::TrackData& track, collection->tracks())
	{
		const QString url = track.url().toString();
		if(track.url().scheme() == "file" && !ignoredFiles.contains(url))
		{
			const int id = m_urls.count();
			m_urls.append(url);
			tracks.append(LastFmTagFetcher::Track(id, track.artist(), track.album(), track.title()));
		}
	}
	m_tagFetcher->findTags(tracks);
}

void CollectionTagFetcher::saveTags(const QMap<unsigned int, QList<LastFmTagFetcher::Tag> >& tags)
{

	QSqlQuery fileQuery;
	fileQuery.prepare("INSERT INTO TaggedFiles (FileName, LastFetch) VALUES (:FileName, :LastFetch)");
	fileQuery.bindValue(":LastFetch", QDateTime::currentDateTime().toTime_t());

	QSqlQuery tagQuery;
	tagQuery.prepare("INSERT INTO Tags (FileId, Tag, Weight) VALUES (:FileId, :Tag, :Weight)");

	typedef QMap<unsigned int, QList<LastFmTagFetcher::Tag> >::ConstIterator constIterator;
	const constIterator end = tags.constEnd();
	for(
		constIterator it = tags.constBegin();
		it != end;
		++it
	)
	{
		Q_ASSERT(static_cast<unsigned int>(m_urls.count()) > it.key());
		fileQuery.bindValue(":FileName", m_urls.at(it.key()));
		fileQuery.exec();
		tagQuery.bindValue(":FileId", fileQuery.lastInsertId());
		Q_FOREACH(const LastFmTagFetcher::Tag& tag, it.value())
		{
			tagQuery.bindValue(":Tag", tag.name());
			tagQuery.bindValue(":Weight", tag.weight());
			if(!tagQuery.exec())
			{
				qDebug() <<tagQuery.lastError().text();
			}
		}
	}
	emit savedTags();
}
