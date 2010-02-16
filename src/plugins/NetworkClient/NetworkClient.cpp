#include "NetworkClient.h"
#include "NetworkClient_ServerDiscoverer.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>

NetworkClient::NetworkClient()
: QObject(0)
{
	new ServerDiscoverer(this);
}

QString NetworkClient::pluginName() const
{
	return tr("Network Client");
}

QString NetworkClient::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString NetworkClient::uniqueId() const
{
	return "org.jerboaplayer.NetworkClient";
}

Q_EXPORT_PLUGIN2(Jerboa_NetworkClient, NetworkClient);
