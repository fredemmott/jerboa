#include "CollectionWidget.h"

#include "CollectionFilter.h"

#ifdef USE_CARBON
#include "SearchLineEdit_mac.h"
typedef SearchLineEdit_mac SearchLineEdit;
#else
#include "SearchLineEdit.h"
#endif

#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>

CollectionWidget::CollectionWidget(Jerboa::PlaylistInterface* playlist, QAbstractItemModel* collection, QWidget* parent)
	:
		QWidget(parent),
		m_playlist(playlist),
		m_treeView(new QTreeView(this))
{
	connect(
		&m_timer,
		SIGNAL(timeout()),
		this,
		SLOT(updateSearch())
	);
	m_timer.setInterval(QApplication::keyboardInputInterval());
	m_timer.setSingleShot(true);
	
	m_filter = new CollectionFilter(this);
	m_filter->setSourceModel(collection);

	// UI fun

	setLayout(new QVBoxLayout());
	layout()->setContentsMargins(0, 0, 0, 0);

	m_searchBox = new SearchLineEdit(this);
	layout()->addWidget(m_searchBox);
#ifdef Q_WS_MAC
	layout()->setSpacing(0);
	m_treeView->setFrameShape(QFrame::NoFrame);
	QWidget* line = new QWidget(this);
	line->setFixedHeight(1);
	line->setAutoFillBackground(true);
	line->setStyleSheet("background-color: #919191;");

	layout()->addWidget(line);
#endif


	connect(
		m_searchBox,
		SIGNAL(textChanged(QString)),
		&m_timer,
		SLOT(start())
	);
	connect(
		m_searchBox,
		SIGNAL(returnPressed()),
		this,
		SLOT(acceptSearch())
	);

	m_treeView->setModel(m_filter);
	m_treeView->setHeaderHidden(true);
	m_treeView->expandToDepth(0);
	m_treeView->setRootIsDecorated(false);
	m_treeView->setExpandsOnDoubleClick(false);
	m_treeView->setDragEnabled(true);
	layout()->addWidget(m_treeView);

	connect(
		m_treeView,
		SIGNAL(doubleClicked(QModelIndex)),
		this,
		SLOT(addItemToPlaylist(QModelIndex))
	);
	connect(
		m_filter,
		SIGNAL(rowsInserted(QModelIndex, int, int)),
		this,
		SLOT(expandArtists())
	);
}

void CollectionWidget::expandArtists()
{
	m_treeView->expandToDepth(0);
}

void CollectionWidget::updateSearch()
{
	SearchLineEdit* searchEdit = qobject_cast<SearchLineEdit*>(m_searchBox);
	Q_ASSERT(searchEdit);
	m_filter->setFilterString(searchEdit->text());

	// Reset expansion
	m_treeView->collapseAll();
	// Expand artists
	m_treeView->expandToDepth(0);
	if(searchEdit->text().simplified().isEmpty())
	{
		// That's all, folks
		return;
	}

	// Only expand albums if only some tracks in them are selected
	QAbstractProxyModel* filterModel = qobject_cast<QAbstractProxyModel*>(m_treeView->model());
	Q_ASSERT(filterModel);
	QAbstractItemModel* sourceModel = filterModel->sourceModel();
	for(int artistRow = 0; artistRow < filterModel->rowCount(); ++artistRow)
	{
		const QModelIndex artistIndex = filterModel->index(artistRow, 0);
		for(int albumRow = 0; albumRow < filterModel->rowCount(artistIndex); ++albumRow)
		{
			const QModelIndex filteredIndex = filterModel->index(albumRow, 0, artistIndex);
			const QModelIndex sourceIndex = filterModel->mapToSource(filteredIndex);
			if(filterModel->rowCount(filteredIndex) != sourceModel->rowCount(sourceIndex))
			{
				m_treeView->expand(filteredIndex);
			}
		}
	}
}

void CollectionWidget::acceptSearch()
{
	SearchLineEdit* searchEdit = qobject_cast<SearchLineEdit*>(m_searchBox);
	m_filter->setFilterString(searchEdit->text());

	QList<Jerboa::TrackData> tracks;
	QAbstractItemModel* model = m_treeView->model();
	for(int i = 0; i < model->rowCount(); ++i)
	{
		tracks.append(model->index(i, 0).data(Qt::UserRole).value<QList<Jerboa::TrackData> >());
	}
	if(tracks.isEmpty())
	{
		return;
	}
	m_playlist->appendTracks(tracks);
	searchEdit->clear();
	updateSearch();
}

void CollectionWidget::addItemToPlaylist(const QModelIndex& index)
{
	const QList<Jerboa::TrackData> tracks = index.data(Qt::UserRole).value<QList<Jerboa::TrackData> >();
	if(tracks.isEmpty())
	{
		return;
	}
	m_playlist->appendTracks(tracks);
}
