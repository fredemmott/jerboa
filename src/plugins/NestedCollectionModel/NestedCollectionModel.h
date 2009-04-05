#ifndef _NESTED_COLLECTION_MODEL_H
#define _NESTED_COLLECTION_MODEL_H

#include "CollectionInterface.h"
#include "Plugin.h"

#include <QObject>

class NestedCollectionModel: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		NestedCollectionModel();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
		Jerboa::CollectionInterface* m_collectionSource;
};

#endif
