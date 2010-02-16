#include "NetworkServer.h"

#include <QByteArray>

class QTcpServer;
class QUdpSocket;

namespace Jerboa
{
	class CollectionInterface;
	class PlayerInterface;
	class PlaylistInterface;
};

class NetworkServer::Implementation : public QObject
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::CollectionInterface* collection, Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist);
	private slots:
		void sendDiscoveryResponses();
	private:
		enum MessageType
		{
			UnsignedMessage,
			SignedMessage
		};
		QByteArray createMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType) const;

		Jerboa::CollectionInterface* m_collection;
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;

		QByteArray m_serverUuid;
		QByteArray m_discoveryResponse;

		QUdpSocket* m_discoverySocket;
		QTcpServer* m_server;
};
