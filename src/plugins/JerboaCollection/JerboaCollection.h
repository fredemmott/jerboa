#ifndef _JERBOA_COLLECTION_H
#define _JERBOA_COLLECTION_H

#include "Plugin.h"

#include <QObject>

class JerboaCollection: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
	private:
		class Implementation;
};

#endif