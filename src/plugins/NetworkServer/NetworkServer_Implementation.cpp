#include "NetworkServer_Implementation.h"

#include "NetworkCommon.h"
#include "NetworkMessage.h"

#include "Uuid.h"

#include <QDebug>
#include <QHostInfo>
#include <QSettings>
#include <QTcpServer>
#include <QUdpSocket>

NetworkServer::Implementation::Implementation(Jerboa::CollectionInterface* collection, Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist)
: QObject(0)
, m_collection(collection)
, m_player(player)
, m_playlist(playlist)
, m_discoverySocket(new QUdpSocket(this))
, m_server(new QTcpServer(this))
{
	const bool startedDiscovery = m_discoverySocket->bind(QHostAddress::Any, NetworkCommon::portNumber());
	if(!startedDiscovery)
	{
		qDebug() << "Failed to start network discovery:" << m_discoverySocket->errorString();
		return;
	}

	if(!m_server->listen())
	{
		qDebug() << "Failed to bind network socket:" << m_server->errorString();
		return;
	}

	qDebug() << "Network server started.";

	connect(m_discoverySocket, SIGNAL(readyRead()), SLOT(sendDiscoveryResponses()));


	QSettings settings;

	// Deal with strings in QSettings so the saved values are more human-readable
	QString serverUuid = settings.value("network/serverUuid").toString();
	if(serverUuid.isEmpty())
	{
		serverUuid = Mendeley::Uuid::createUuid();
		settings.setValue("network/serverUuid", serverUuid);
	}

	m_serverUuid = serverUuid.toLatin1();

	// Load up the response
	{
		QByteArray payload;
		payload = QString::number(m_serverUuid.length()).toLatin1();
		payload.append(',');
		payload.append(m_serverUuid);
		payload.append(',');
		const QByteArray host = QHostInfo::localHostName().toUtf8();
		payload.append(QString::number(host.length()).toLatin1());
		payload.append(',');
		payload.append(host);
		payload.append(',');
		const QByteArray port = QString::number(m_server->serverPort()).toLatin1();
		payload.append(QString::number(port.length()).toLatin1());
		payload.append(',');
		payload.append(port);
		payload.append(',');

		m_discoveryResponse = NetworkMessage("ANNOUNCE", payload, NetworkMessage::UnsignedMessage).toByteArray();
	}
}

void NetworkServer::Implementation::sendDiscoveryResponses()
{
	while(m_discoverySocket->hasPendingDatagrams())
	{
		// don't worry about overflow, UDP packets have reasonable maximum size
		QByteArray payload;
		payload.resize(m_discoverySocket->pendingDatagramSize());
		QHostAddress source;
		quint16 sourcePort;
		m_discoverySocket->readDatagram(payload.data(), payload.size(), &source, &sourcePort);
		if(payload == NetworkCommon::discoveryMessage())
		{
			qDebug() << "Received a discovery message from" << QString("%1:%2").arg(source.toString()).arg(sourcePort);
			// send a response
			m_discoverySocket->writeDatagram(m_discoveryResponse, source, sourcePort);
		}
	}
}
