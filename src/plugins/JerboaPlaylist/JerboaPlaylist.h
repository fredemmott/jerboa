#ifndef _JERBOA_PLAYLIST_H
#define _JERBOA_PLAYLIST_H

#include "Plugin.h"

#include <QObject>

class JerboaPlaylist: public QObject, public Jerboa::Plugin
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
