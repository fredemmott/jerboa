#include "NetworkServer.h"
#include "NetworkServer_Implementation.h"

#include "CollectionInterface.h"
#include "PlayerInterface.h"
#include "PlaylistInterface.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>

NetworkServer::NetworkServer()
: QObject(0)
, m_collection(0)
, m_player(0)
, m_playlist(0)
{
}

QString NetworkServer::pluginName() const
{
	return tr("Network Server");
}

QString NetworkServer::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString NetworkServer::uniqueId() const
{
	return "org.jerboaplayer.NetworkServer";
}

void NetworkServer::initialize() const
{
	if(m_collection && m_player && m_playlist)
	{
		new Implementation(m_collection, m_player, m_playlist);
	}
}

void NetworkServer::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case CollectionSource:
			Q_ASSERT(qobject_cast<Jerboa::CollectionInterface*>(component));
			m_collection = static_cast<Jerboa::CollectionInterface*>(component);
			initialize();
			break;
		case Player:
			Q_ASSERT(qobject_cast<Jerboa::PlayerInterface*>(component));
			m_player = static_cast<Jerboa::PlayerInterface*>(component);
			initialize();
			break;
		case PlaylistSource:
			Q_ASSERT(qobject_cast<Jerboa::PlaylistInterface*>(component));
			m_playlist = static_cast<Jerboa::PlaylistInterface*>(component);
			initialize();
			break;
		default:
			Plugin::addComponent(type, component);
			break;
	}
}

Q_EXPORT_PLUGIN2(Jerboa_NetworkServer, NetworkServer);
