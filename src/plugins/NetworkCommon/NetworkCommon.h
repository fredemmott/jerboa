#pragma once

#include "Plugin.h"

#include <QObject>

class NetworkCommon: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		NetworkCommon();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;

		static quint16 portNumber();
		static QByteArray discoveryMessage();
};
