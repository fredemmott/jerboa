#pragma once

#include <QByteArray>
#include <QIODevice>
#include <QList>

class NetworkMessage
{
	public:
		enum MessageType
		{
			UnsignedMessage,
			SignedMessage
		};
		NetworkMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType);
		NetworkMessage(const QByteArray& command, const QList<QByteArray>& payload, MessageType messageType);
		NetworkMessage(const QByteArray& serialized);

		QByteArray command() const;
		QByteArray payload() const;
		QList<QByteArray> payloadList() const;

		QByteArray toByteArray() const;
	private:
		void updateSerialization() const; // cache

		QByteArray m_command;
		QByteArray m_payload;
		mutable QList<QByteArray> m_payloadList; // cache
		MessageType m_messageType;

		mutable QByteArray m_serialized; // cache
};
