#include "NetworkMessage.h"

#include <QDebug>

NetworkMessage::NetworkMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType)
{
	m_command = command;
	m_payload = payload;
	m_messageType = messageType;
}

NetworkMessage::NetworkMessage(const QByteArray& command, const QList<QByteArray>& payload, MessageType messageType)
{
	m_command = command;
	m_messageType = messageType;

	m_payloadList = payload;

	Q_FOREACH(const QByteArray& item, payload)
	{
		m_payload.append(QByteArray::number(item.length()));
		m_payload.append(':');
		m_payload.append(item);
		m_payload.append(',');
	}
}

NetworkMessage::NetworkMessage(const QByteArray& serialized)
{
	const int endOfCommandLength = serialized.indexOf(':');
	const int commandLength = serialized.left(endOfCommandLength).toInt();

	const int startOfCommand = endOfCommandLength + 1;
	m_command = serialized.mid(startOfCommand, commandLength);
	const int endOfCommand = startOfCommand + commandLength;

	const int startOfPayloadLength = endOfCommand + 1;
	const int endOfPayloadLength = serialized.indexOf(':', startOfPayloadLength);
	const int payloadLength = serialized.mid(startOfPayloadLength, endOfPayloadLength - startOfPayloadLength).toInt();

	const int startOfPayload = endOfPayloadLength + 1;
	m_payload = serialized.mid(startOfPayload, payloadLength);

	m_messageType = UnsignedMessage;
}

QByteArray NetworkMessage::command() const
{
	return m_command;
}

QList<QByteArray> NetworkMessage::payloadList() const
{
	if(m_payloadList.isEmpty())
	{
		// "size:data,"
		int left = 0;
		while(left < m_payload.size())
		{
			const QByteArray chunk = m_payload.mid(left);
			const int separatorPosition = chunk.indexOf(':');
			const QByteArray lengthChunk = chunk.left(separatorPosition);
			const int length = lengthChunk.toInt();
			qDebug() << Q_FUNC_INFO << "Length:" << lengthChunk << length;
			Q_ASSERT(chunk.size() >= length + lengthChunk.size() + 2);
			const QByteArray data = chunk.mid(separatorPosition + 1, length);
			Q_ASSERT(data.length() == length);
			left += lengthChunk.size() + 1 + length + 1;
			m_payloadList.append(data);
		}
	}
	return m_payloadList;
}

void NetworkMessage::updateSerialization() const
{
	// netstrings-influenced
	// CommandLength:Command,PayloadLength:Payload,{U|S,timestamp-length:timestamp,message-uuid,signature};
	//
	// Examples:
	// 3:foo,0:,U;
	// 3:bar,3:baz,S,10:1266356591,{fc8d8c6c-2ede-4bb8-8685-2cc47cdd8b2b},0000000000000000000000000000000000000000
	//
	// Where:
	// - timestamp is the number of seconds since 1970-01-01T00:00:00 UTC
	// - message-uuid is a per-message UUID to prevent replays
	// - signature is an HMAC-SHA1 of everything up to and including the previous comma
	QByteArray& out = m_serialized;
	m_serialized.clear();
	Q_ASSERT(m_messageType == UnsignedMessage);

	out.append(QByteArray::number(m_command.length()));
	out.append(':');
	out.append(m_command);
	out.append(',');
	out.append(QByteArray::number(m_payload.length()));
	out.append(':');
	out.append(m_payload);
	out.append(",U;");
}

QByteArray NetworkMessage::toByteArray() const
{
	if(m_serialized.isNull())
	{
		updateSerialization();
	}
	return m_serialized;
}
