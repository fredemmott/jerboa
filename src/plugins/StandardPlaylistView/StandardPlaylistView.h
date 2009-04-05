#ifndef _STANDARD_PLAYLIST_VIEW_H
#define _STANDARD_PLAYLIST_VIEW_H

#include "Plugin.h"

#include <QAbstractItemModel>
#include <QObject>

class StandardPlaylistView: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
		void addComponent(ComponentType, QObject* component);
	private:
		QAbstractItemModel* m_playlistModel;
};

#endif
