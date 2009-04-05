#ifndef _STANDARD_CONTAINER_H
#define _STANDARD_CONTAINER_H

#include "Plugin.h"

#include <QObject>

class StandardContainer : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
};

#endif
