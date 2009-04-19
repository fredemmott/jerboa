#ifndef _FILE_SYSTEM_TAB_H
#define _FILE_SYSTEM_TAB_H

#include "Plugin.h"

#include <QAbstractItemModel>
#include <QObject>

class FileSystemTab: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
};

#endif
