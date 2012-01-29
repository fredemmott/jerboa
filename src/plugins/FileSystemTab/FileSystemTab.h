#ifndef _FILE_SYSTEM_TAB_H
#define _FILE_SYSTEM_TAB_H

#include "PlaylistInterface.h"
#include "JerboaPlugin.h"

#include <QObject>

class QModelIndex;
class QTreeView;

class FileSystemTab: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		FileSystemTab(QObject* parent = 0);
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
		void addComponent(ComponentType, QObject* component);
	private slots:
		void scrollToSelection();
		void addPathToPlaylist(const QModelIndex& index);
	private:
		Jerboa::PlaylistInterface* m_playlist;
		QTreeView* m_view;
};

#endif
