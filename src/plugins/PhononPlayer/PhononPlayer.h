#ifndef _PHONON_PLAYER_H
#define _PHONON_PLAYER_H

#include "PlaylistInterface.h"
#include "JerboaPlugin.h"

#include <QObject>

class PhononPlayer: public QObject, public Jerboa::Plugin
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
		class Implementation;
		Jerboa::PlaylistInterface* m_playlist;
};

#endif
