#include "JerboaPlaylist.h"

#include "JerboaPlaylist_Implementation.h"

#include <QtPlugin>

QObject* JerboaPlaylist::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistSource:
			return new Implementation(parent);
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

Q_EXPORT_PLUGIN2(Jerboa_JerboaPlaylist, JerboaPlaylist);
