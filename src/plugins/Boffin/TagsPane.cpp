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
	QStringList tags = m_view->currentTags();
	if(!tags.isEmpty())
	{
		QSqlQuery query;
		query.exec("DROP TABLE IF EXISTS MatchingFiles;");
		query.exec("CREATE TEMPORARY TABLE MatchingFiles (FileId INTEGER NOT NULL, Weight FLOAT NOT NULL DEFAULT 0);");
		query.exec("INSERT INTO MatchingFiles (FileId) SELECT ID FROM TaggedFiles");
		query.prepare("UPDATE MatchingFiles SET Weight = Weight + (SELECT Weight FROM Tags Where Tag = :tag AND FileId = MatchingFiles.FileId)");
		Q_FOREACH(const QString& tag, tags)
		{
			query.bindValue(":tag", tag);
			query.exec();
		}

		query.exec("DELETE FROM MatchingFiles WHERE Weight = 0");

		QSet<QUrl> urls;
		query.exec("SELECT DISTINCT FileName FROM MatchingFiles INNER JOIN TaggedFiles ON MatchingFiles.FileId = TaggedFiles.ID WHERE MatchingFiles.Weight >= (SELECT AVG(MatchingFiles.Weight)) ORDER BY Weight ASC");
		for(query.first(); query.isValid(); query.next())
		{
			urls.insert(QUrl(query.value(0).toString()));
		}

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
