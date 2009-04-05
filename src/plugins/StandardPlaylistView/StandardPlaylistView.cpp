#include "StandardPlaylistView.h"

#include <QTreeView>
#include <QtPlugin>

QObject* StandardPlaylistView::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistView:
			{
				Q_ASSERT(m_playlistModel);
				QTreeView* treeView = new QTreeView(qobject_cast<QWidget*>(parent));
				treeView->setModel(m_playlistModel);
				return treeView;
			}
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString StandardPlaylistView::pluginName() const
{
	return tr("Playlist Tree");
}

QString StandardPlaylistView::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString StandardPlaylistView::uniqueId() const
{
	return "org.jerboaplayer.StandardPlaylistView";
}

QSet<Jerboa::Plugin::ComponentType> StandardPlaylistView::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::PlaylistView;
}

void StandardPlaylistView::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::PlaylistModel:
			m_playlistModel = qobject_cast<QAbstractItemModel*>(component);
			Q_ASSERT(m_playlistModel);
			break;
		default:
			Jerboa::Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_StandardPlaylistView, StandardPlaylistView);
