#include "TagsPane.h"

#include "CollectionTagFetcher.h"
#include "TagCloudModel.h"
#include "TagCloudView.h"
#include "TagDelegate.h"
#include "WeightedTag.h"

#include "CollectionInterface.h"
#include "PlaylistInterface.h"

#include <QHash>
#include <QLabel>
#include <QPushButton>
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
		query.exec("CREATE TEMPORARY TABLE MatchingFiles (FileId NOT NULL);");
		query.exec("INSERT INTO MatchingFiles (FileId) SELECT ID FROM TaggedFiles");
		query.prepare("DELETE FROM MatchingFiles WHERE FileId NOT IN (SELECT FileId FROM Tags WHERE Tag = :tag);");
		Q_FOREACH(const QString& tag, tags)
		{
			query.bindValue(":tag", tag);
			query.exec();
		}

		QSet<QUrl> urls;
		query.exec("SELECT DISTINCT FileName FROM MatchingFiles INNER JOIN TaggedFiles ON MatchingFiles.FileId = TaggedFiles.ID");
		for(query.first(); query.isValid(); query.next())
		{
			urls.insert(QUrl(query.value(0).toString()));
		}

		QList<Jerboa::TrackData> tracks;
		Q_FOREACH(const Jerboa::TrackData& track, m_collection->tracks())
		{
			if(urls.contains(track.url()))
			{
				tracks.append(track);
			}
		}
		m_playlist->appendTracks(tracks);
		
		query.exec("DROP TABLE MatchingFiles");
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
