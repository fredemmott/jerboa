#ifndef _STANDARD_CONTAINER_H
#define _STANDARD_CONTAINER_H

#include "Container.h"
#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QObject>

class StandardContainer : public Jerboa::Container, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		StandardContainer();
		// Container
		virtual void addComponent(Plugin::ComponentType componentType, QWidget* component, QWidget* widget) const;
		virtual QWidget* widget() const;

		// Plugin
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
		void addComponent(ComponentType type, QObject* component);
	private:
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};

#endif
