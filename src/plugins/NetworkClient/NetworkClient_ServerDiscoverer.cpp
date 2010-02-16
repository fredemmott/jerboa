#include "NetworkClient_ServerDiscoverer.h"

#include "../NetworkCommon/NetworkCommon.h"

#include <QUdpSocket>

NetworkClient::ServerDiscoverer::ServerDiscoverer(QObject* parent)
: QObject(parent)
, m_socket(new QUdpSocket(this))
{
	m_socket->bind();
	connect(m_socket, SIGNAL(readyRead()), SLOT(handleResponses()));
	sendBroadcast();
}

void NetworkClient::ServerDiscoverer::sendBroadcast()
{
	m_socket->writeDatagram(NetworkCommon::discoveryMessage(), QHostAddress::Broadcast, NetworkCommon::portNumber());
}

void NetworkClient::ServerDiscoverer::handleResponses()
{
	while(m_socket->hasPendingDatagrams())
	{
		// don't worry about overflow, UDP packets have reasonable maximum size
		QByteArray payload;
		payload.resize(m_socket->pendingDatagramSize());
		QHostAddress source;
		quint16 sourcePort;
		m_socket->readDatagram(payload.data(), payload.size(), &source, &sourcePort);
		if(sourcePort != NetworkCommon::portNumber())
		{
			qDebug() << "Got a stray UDP packet from" << QString("%1:%2").arg(source.toString()).arg(sourcePort);
			continue;
		}
		else
		{
			qDebug() << "Got a response:" << payload;
		}
	}
}
