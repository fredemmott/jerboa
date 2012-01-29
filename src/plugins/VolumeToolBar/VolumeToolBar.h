#pragma once

#include "PlayerInterface.h"
#include "JerboaPlugin.h"

#include <QObject>

class VolumeToolBar : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		VolumeToolBar();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
		Jerboa::PlayerInterface* m_player;
};
