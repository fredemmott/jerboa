#include "TagsPane.h"

#include "CollectionTagFetcher.h"
#include "TagCloudModel.h"
#include "TagCloudView.h"
#include "TagDelegate.h"
#include "WeightedTag.h"

#include "CollectionInterface.h"
#include "PlaylistInterface.h"

#include <QLabel>
#include <QSqlQuery>
#include <QVBoxLayout>

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
