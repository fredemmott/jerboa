#ifndef _COLLECTION_TREE_VIEW_H
#define _COLLECTION_TREE_VIEW_H

#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QAbstractItemModel>
#include <QObject>

class CollectionTreeView: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
		void addComponent(ComponentType, QObject* component);
	private:
		Jerboa::PlaylistInterface* m_playlist;
		QAbstractItemModel* m_collectionModel;
};

#endif
