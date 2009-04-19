#include "JerboaPlaylist.h"

#include "JerboaPlaylist_Implementation.h"

#include <QtPlugin>

JerboaPlaylist::JerboaPlaylist(QObject* parent)
	:
		QObject(parent),
		m_tagReader(0)
{
}

QObject* JerboaPlaylist::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistSource:
			return new Implementation(m_tagReader, parent);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString JerboaPlaylist::pluginName() const
{
	return tr("Jerboa Playlist");
}

QString JerboaPlaylist::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString JerboaPlaylist::uniqueId() const
{
	return "org.jerboaplayer.JerboaPlaylist";
}

QSet<Jerboa::Plugin::ComponentType> JerboaPlaylist::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::PlaylistSource;
}

void JerboaPlaylist::addComponent(ComponentType type, QObject* component)
{
	if(type == TagReader)
	{
		m_tagReader = qobject_cast<Jerboa::TagReader*>(component);
		Q_ASSERT(m_tagReader);
	}
	Plugin::addComponent(type, component);
}

Q_EXPORT_PLUGIN2(Jerboa_JerboaPlaylist, JerboaPlaylist);
