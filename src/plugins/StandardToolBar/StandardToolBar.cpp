#include "StandardToolBar.h"
#include "StandardToolBar_Implementation.h"

#include <QDebug>
#include <QtPlugin>

StandardToolBar::StandardToolBar()
	:
		QObject(0),
		m_player(0),
		m_playlist(0)
{
	Q_INIT_RESOURCE(StandardToolBar);
}

QObject* StandardToolBar::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::ToolBar:
			Q_ASSERT(m_player);
			Q_ASSERT(m_playlist);
			return new Implementation(m_player, m_playlist, qobject_cast<QWidget*>(parent));
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString StandardToolBar::pluginName() const
{
	return tr("Main Tool-bar");
}

QString StandardToolBar::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString StandardToolBar::uniqueId() const
{
	return "org.jerboaplayer.StandardToolBar";
}

QSet<Jerboa::Plugin::ComponentType> StandardToolBar::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::ToolBar;
}

void StandardToolBar::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::Player:
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
			return;
		case Jerboa::Plugin::PlaylistSource:
			m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
			Q_ASSERT(m_playlist);
			return;
		default:
			Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_StandardToolBar, StandardToolBar);
