#include "NetworkServer_Implementation.h"

#include "Uuid.h"

#include <QDebug>
#include <QHostInfo>
#include <QSettings>
#include <QTcpServer>
#include <QUdpSocket>

const quint16 PORT_NUMBER = 61719; // pseudo-random from private range
const QByteArray DISCOVERY_MESSAGE = "Jerboa Network Discovery/1.0\n";

NetworkServer::Implementation::Implementation(Jerboa::CollectionInterface* collection, Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist)
: QObject(0)
, m_collection(collection)
, m_player(player)
, m_playlist(playlist)
, m_discoverySocket(new QUdpSocket(this))
, m_server(new QTcpServer(this))
{
	const bool startedDiscovery = m_discoverySocket->bind(QHostAddress::Any, PORT_NUMBER);
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

		m_discoveryResponse = createMessage("ANNOUNCE", payload, UnsignedMessage);
	}
}

QByteArray NetworkServer::Implementation::createMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType) const
{
	// netstrings-influenced
	// CommandLength,Command,PayloadLength,Payload,{U|S,timestamp-length,timestamp,message-uuid,signature};
	//
	// Examples:
	// 3,foo,0,,U;
	// 3,bar,3,baz,S,10,1266356591,{fc8d8c6c-2ede-4bb8-8685-2cc47cdd8b2b},0000000000000000000000000000000000000000
	//
	// Where:
	// - timestamp is the number of seconds since 1970-01-01T00:00:00 UTC
	// - message-uuid is a per-message UUID to prevent replays
	// - signature is an HMAC-SHA1 of everything up to and including the previous comma
	QByteArray out;
	Q_ASSERT(messageType == UnsignedMessage);
	Q_UNUSED(messageType);

	out.append(QString::number(command.length()).toLatin1());
	out.append(',');
	out.append(command);
	out.append(',');
	out.append(QString::number(payload.length()).toLatin1());
	out.append(',');
	out.append(payload);
	out.append(",U;");
	return out;
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
		if(payload == DISCOVERY_MESSAGE)
		{
			qDebug() << "Received a discovery message from" << QString("%1:%2").arg(source.toString()).arg(sourcePort);
			// send a response
			m_discoverySocket->writeDatagram(m_discoveryResponse, source, sourcePort);
		}
	}
}
