#include "PlaylistModel.h"
#include "PlaylistModel_Implementation.h"

#include <QDebug>
#include <QtPlugin>

PlaylistModel::PlaylistModel()
	:
		QObject(0),
		m_playlistSource(0)
{
}

QObject* PlaylistModel::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistModel:
			Q_ASSERT(m_playlistSource);
			return new Implementation(m_playlistSource, parent);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString PlaylistModel::pluginName() const
{
	return tr("Artist/Album/Title Playlist");
}

QString PlaylistModel::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString PlaylistModel::uniqueId() const
{
	return "org.jerboaplayer.PlaylistModel";
}

QSet<Jerboa::Plugin::ComponentType> PlaylistModel::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::PlaylistModel;
}

void PlaylistModel::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistSource:
			m_playlistSource = qobject_cast<Jerboa::PlaylistInterface*>(component);
			Q_ASSERT(m_playlistSource);
			return;
		default:
			Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_PlaylistModel, PlaylistModel);
