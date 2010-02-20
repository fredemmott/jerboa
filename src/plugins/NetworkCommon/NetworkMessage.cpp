#include "NetworkMessage.h"

NetworkMessage::NetworkMessage(const QByteArray& command, const QByteArray& payload, MessageType messageType)
{
	m_command = command;
	m_payload = payload;
	m_messageType = messageType;
}

void NetworkMessage::updateSerialization() const
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
	QByteArray& out = m_serialized;
	m_serialized.clear();
	Q_ASSERT(m_messageType == UnsignedMessage);

	out.append(QString::number(m_command.length()).toLatin1());
	out.append(',');
	out.append(m_command);
	out.append(',');
	out.append(QString::number(m_payload.length()).toLatin1());
	out.append(',');
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
