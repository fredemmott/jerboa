#include "TagsPane.h"

#include "CollectionInterface.h"

#include "CollectionTagFetcher.h"
#include "TagCloudModel.h"
#include "TagCloudView.h"
#include "TagDelegate.h"
#include "WeightedTag.h"

#include <QSqlQuery>
#include <QVBoxLayout>

TagsPane::TagsPane(Jerboa::CollectionInterface* collection, QWidget* parent)
: QWidget(parent)
, m_model(new TagCloudModel(this))
, m_view(new TagCloudView(this))
{
	m_view->setModel(m_model);
	m_view->setItemDelegate(new TagDelegate(this));

	setWindowTitle(tr("Tags"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_view);

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
	//const int count = 50;
	QList<WeightedTag> tags;
	QSqlQuery query;
	query.exec("SELECT Tag, Weight FROM TopTags"/* LIMIT " + QString::number(count) */);
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
