#include "NetworkClient.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>

NetworkClient::NetworkClient()
: QObject(0)
{
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
