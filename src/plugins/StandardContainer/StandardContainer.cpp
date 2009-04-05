#include "StandardContainer.h"

#include "MainWindow.h"

#include <QDebug>
#include <QtPlugin>

QObject* StandardContainer::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::Container:
			return const_cast<StandardContainer*>(this);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

void StandardContainer::addComponent(Plugin::ComponentType componentType, QWidget* component, QWidget* widget) const
{
	MainWindow* mainWindow = qobject_cast<MainWindow*>(widget);
	Q_ASSERT(mainWindow);
	mainWindow->addComponent(componentType, component);
}

QWidget* StandardContainer::widget() const
{
	return new MainWindow(m_player, m_playlist, 0);
}

QString StandardContainer::pluginName() const
{
	return tr("Standard Container");
}

QString StandardContainer::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString StandardContainer::uniqueId() const
{
	return "org.jerboaplayer.StandardContainer";
}

QSet<Jerboa::Plugin::ComponentType> StandardContainer::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::Container;
}

void StandardContainer::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Player:
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
			break;
		case PlaylistSource:
			m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
			Q_ASSERT(m_playlist);
			break;
		default:
			break;
	}
}

Q_EXPORT_PLUGIN2(Jerboa_StandardContainer, StandardContainer);
