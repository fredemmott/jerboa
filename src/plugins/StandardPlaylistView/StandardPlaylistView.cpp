#include "StandardPlaylistView.h"

#include <QDebug>
#include <QHeaderView>
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
				treeView->header()->setResizeMode(0, QHeaderView::Stretch);
				treeView->setRootIsDecorated(false);
				connect(
					treeView,
					SIGNAL(doubleClicked(QModelIndex)),
					this,
					SLOT(skipTo(QModelIndex))
				);
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

void StandardPlaylistView::skipTo(const QModelIndex& index)
{
	m_player->skipTo(index.row());
}

void StandardPlaylistView::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::Player:
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
			break;
		case Jerboa::Plugin::PlaylistModel:
			m_playlistModel = qobject_cast<QAbstractItemModel*>(component);
			Q_ASSERT(m_playlistModel);
			break;
		default:
			Jerboa::Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_StandardPlaylistView, StandardPlaylistView);
