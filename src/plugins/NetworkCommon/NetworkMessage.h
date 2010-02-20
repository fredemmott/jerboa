#pragma once

#include <QByteArray>
#include <QIODevice>

class NetworkMessage
{
	public:
		enum MessageType
		{
			UnsignedMessage,
			SignedMessage
		};
		NetworkMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType);
		QByteArray toByteArray() const;
	private:
		void updateSerialization() const; // cache

		QByteArray m_command;
		QByteArray m_payload;
		MessageType m_messageType;

		mutable QByteArray m_serialized; // used as a cache
};
