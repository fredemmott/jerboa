#pragma once

#include "NetworkClient.h"

class QUdpSocket;

#include <QPair> // before QHostAddress for Qt 4.5

#include <QHostAddress>

class NetworkClient::ServerDiscoverer : public QObject
{
	Q_OBJECT;
	public:
		ServerDiscoverer(QObject* parent);
	signals:
		void foundServer(const QString& name, const QByteArray& uuid, const QHostAddress& address, quint16 tcpPort);
	private slots:
		void sendBroadcast();
		void handleResponses();
	private:
		QUdpSocket* m_socket;
};
