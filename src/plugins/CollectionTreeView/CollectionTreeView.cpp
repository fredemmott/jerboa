#include "CollectionTreeView.h"

#include <QTreeView>
#include <QtPlugin>

QObject* CollectionTreeView::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionView:
			{
				Q_ASSERT(m_collectionModel);
				QTreeView* treeView = new QTreeView(qobject_cast<QWidget*>(parent));
				treeView->setModel(m_collectionModel);
				treeView->setHeaderHidden(true);
				treeView->expandToDepth(0);
				treeView->setRootIsDecorated(false);
				treeView->setExpandsOnDoubleClick(false);
				return treeView;
			}
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString CollectionTreeView::pluginName() const
{
	return tr("Collection Tree");
}

QString CollectionTreeView::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString CollectionTreeView::uniqueId() const
{
	return "org.jerboaplayer.CollectionTreeView";
}

QSet<Jerboa::Plugin::ComponentType> CollectionTreeView::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::CollectionView;
}

void CollectionTreeView::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionModel:
			m_collectionModel = qobject_cast<QAbstractItemModel*>(component);
			Q_ASSERT(m_collectionModel);
			break;
		default:
			Jerboa::Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_CollectionTreeView, CollectionTreeView);
