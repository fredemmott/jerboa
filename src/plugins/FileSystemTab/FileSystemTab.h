#ifndef _FILE_SYSTEM_TAB_H
#define _FILE_SYSTEM_TAB_H

#include "Plugin.h"

#include <QObject>

class QTreeView;

class FileSystemTab: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
	private slots:
		void scrollToSelection();
	private:
		QTreeView* m_view;
};

#endif
