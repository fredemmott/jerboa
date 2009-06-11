#include "TagsPane.h"

#include "CollectionTagFetcher.h"
#include "TagCloudModel.h"
#include "TagCloudView.h"
#include "TagDelegate.h"
#include "WeightedTag.h"

#include "CollectionInterface.h"
#include "PlaylistInterface.h"

#include <QDebug>
#include <QHash>
#include <QLabel>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>

#define qexec(x) if(!query.exec(x)) { qDebug() << "Failed to execute" << (x) << query.lastError().text() << __LINE__; }

static inline uint qHash(const QUrl& url)
{
	return qHash(url.toString());
}

TagsPane::TagsPane(Jerboa::CollectionInterface* collection, Jerboa::PlaylistInterface* playlist, QWidget* parent)
: QWidget(parent)
, m_collection(collection)
, m_playlist(playlist)
, m_model(new TagCloudModel(this))
, m_view(new TagCloudView(this))
{
	Q_ASSERT(collection);
	Q_ASSERT(playlist);

	m_view->setModel(m_model);
	m_view->setItemDelegate(new TagDelegate(this));

	setWindowTitle(tr("Tags"));

	QVBoxLayout* layout = new QVBoxLayout(this);

	m_addButton = new QPushButton(tr("Add Tracks"), this);
	layout->addWidget(m_addButton);

	layout->addWidget(m_view);

	QLabel* label = new QLabel(this);
	label->setText("<a href='http://www.last.fm'><img src=':/Boffin/lastfm.png' /></a>");
	label->setAlignment(Qt::AlignHCenter);
	label->setOpenExternalLinks(true);
	layout->addWidget(label);

	CollectionTagFetcher* fetcher = new CollectionTagFetcher(collection, this);

	connect(
		fetcher,
		SIGNAL(savedTags()),
		this,
		SLOT(readTags())
	);
	connect(
		fetcher,
		SIGNAL(savedTags()),
		fetcher,
		SLOT(deleteLater())
	);
	connect(
		m_addButton,
		SIGNAL(clicked()),
		this,
		SLOT(addTracks())
	);
}

void TagsPane::addTracks()
{
	const QStringList tags = m_view->currentTags();
	if(!tags.isEmpty())
	{
		QSqlQuery query;
		query.exec("DROP TABLE IF EXISTS CurrentTags");
		query.exec("CREATE TEMPORARY TABLE CurrentTags (Tag TEXT NOT NULL)");
		query.prepare("INSERT INTO CurrentTags VALUES (:tag)");
		Q_FOREACH(const QString& tag, tags)
		{
			query.bindValue(":tag", tag);
			query.exec();
		}

		query.exec("DROP TABLE IF EXISTS Results");
		qexec("CREATE TEMPORARY TABLE Results (FileName TEXT NOT NULL, Weight FLOAT NOT NULL)");
		qexec(
			"INSERT INTO Results SELECT "
				"FileName, "
				"SUM(Weight) AS TotalWeight "
			"FROM Tags "
			"JOIN TaggedFiles "
				"ON Tags.FileId = TaggedFiles.ID "
			"JOIN CurrentTags "
				"ON Tags.Tag = CurrentTags.Tag "
			"GROUP BY FileId "
			"ORDER BY TotalWeight DESC"
		);
		const int wanted = query.numRowsAffected() / 2;
		qexec(QString("SELECT FileName FROM Results ORDER BY Weight DESC LIMIT %1").arg(QString::number(wanted)));

		QSet<QUrl> urls;
		for(query.first(); query.isValid(); query.next())
		{
			urls.insert(QUrl(query.value(0).toString()));
		}

		query.exec("DROP TABLE Results");
		query.exec("DROP TABLE CurrentTags");

		QHash<QUrl, Jerboa::TrackData> urlTracks;
		Q_FOREACH(const Jerboa::TrackData& track, m_collection->tracks())
		{
			if(urls.contains(track.url()))
			{
				urlTracks.insert(track.url(), track);
			}
		}
		QList<Jerboa::TrackData> tracks;
		Q_FOREACH(const QUrl& url, urls)
		{
			if(urlTracks.contains(url))
			{
				tracks.append(urlTracks.value(url));
			}
			else
			{
				qDebug() << "NOT FOUND:" << url;
			}
		}
		
		query.exec("DROP TABLE MatchingFiles");

		if(!tracks.isEmpty())
		{
			m_playlist->appendTracks(tracks);
		}
	}
}

void TagsPane::readTags()
{
	const int count = 200;
	QList<WeightedTag> tags;
	QSqlQuery query;
	query.exec("SELECT Tag, Weight FROM TopTags LIMIT " + QString::number(count));
	for(query.first(); query.isValid(); query.next())
	{
		tags.append(
			WeightedTag(
				query.value(0).toString(),
				query.value(1).value<qreal>()
			)
		);
	}
	m_model->setTags(tags);
}
