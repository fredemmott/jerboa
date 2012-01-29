#pragma once

#include "JerboaPlugin.h"

#include <QObject>

namespace Jerboa
{
	class CollectionInterface;
	class PlayerInterface;
	class PlaylistInterface;
};

class NetworkServer : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		NetworkServer();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;

		void initialize() const;

		Jerboa::CollectionInterface* m_collection;
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};
