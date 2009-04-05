#include "CollectionWidget.h"

#include <QDebug>
#include <QTreeView>
#include <QVBoxLayout>

CollectionWidget::CollectionWidget(Jerboa::PlaylistInterface* playlist, QAbstractItemModel* collection, QWidget* parent)
	:
		QWidget(parent),
		m_playlist(playlist),
		m_treeView(new QTreeView(this))
{
	m_treeView->setModel(collection);
	m_treeView->setHeaderHidden(true);
	m_treeView->expandToDepth(0);
	m_treeView->setRootIsDecorated(false);
	m_treeView->setExpandsOnDoubleClick(false);

	connect(
		m_treeView,
		SIGNAL(doubleClicked(QModelIndex)),
		this,
		SLOT(addItemToPlaylist(QModelIndex))
	);

	setLayout(new QVBoxLayout());
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->addWidget(m_treeView);
}

void CollectionWidget::addItemToPlaylist(const QModelIndex& index)
{
	const QList<Jerboa::TrackData> tracks = index.data(Qt::UserRole).value<QList<Jerboa::TrackData> >();
	m_playlist->appendTracks(tracks);
}
