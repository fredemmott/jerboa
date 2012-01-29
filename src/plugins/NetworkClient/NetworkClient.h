#pragma once

#include "JerboaPlugin.h"

#include <QObject>

class NetworkClient : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		NetworkClient();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
	private:
		class ServerDiscoverer;
};
