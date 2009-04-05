#include "PhononPlayer.h"

#include "PhononPlayer_Implementation.h"

#include <QtPlugin>

QObject* PhononPlayer::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::Player:
			return new Implementation(m_playlist, parent);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString PhononPlayer::pluginName() const
{
	return tr("Phonon Player");
}

QString PhononPlayer::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString PhononPlayer::uniqueId() const
{
	return "org.jerboaplayer.PhononPlayer";
}

QSet<Jerboa::Plugin::ComponentType> PhononPlayer::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::Player;
}

void PhononPlayer::addComponent(ComponentType type, QObject* component)
{
	if(type == Jerboa::Plugin::PlaylistSource)
	{
		m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
		Q_ASSERT(m_playlist);
	}
	Jerboa::Plugin::addComponent(type, component);
}

Q_EXPORT_PLUGIN2(Jerboa_PhononPlayer, PhononPlayer);
