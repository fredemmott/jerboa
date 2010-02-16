#include "NetworkCommon.h"

#include <QDebug>
#include <QtPlugin>

NetworkCommon::NetworkCommon()
: QObject(0)
{
}

QString NetworkCommon::pluginName() const
{
	return tr("Common Network Client/Server Plugin");
}

QString NetworkCommon::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString NetworkCommon::uniqueId() const
{
	return "org.jerboaplayer.NetworkCommon";
}

quint16 NetworkCommon::portNumber()
{
	return 61719; // pseudo-random from private range
}

QByteArray NetworkCommon::discoveryMessage()
{
	return "Jerboa Network Discovery/1.0\n";
}

Q_EXPORT_PLUGIN2(Jerboa_NetworkCommon, NetworkCommon);
