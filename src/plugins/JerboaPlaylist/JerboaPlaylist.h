#ifndef _JERBOA_PLAYLIST_H
#define _JERBOA_PLAYLIST_H

#include "JerboaPlugin.h"
#include "TagReader.h"

#include <QObject>

class JerboaPlaylist: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		JerboaPlaylist(QObject* parent = 0);
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
		void addComponent(ComponentType, QObject* component);
	private:
		class Implementation;
		Jerboa::TagReader* m_tagReader;
};

#endif
