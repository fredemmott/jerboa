#include "VolumeToolBar.h"
#include "VolumeToolBar_Implementation.h"

#include <QDebug>
#include <QtPlugin>

VolumeToolBar::VolumeToolBar()
	:
		QObject(0),
		m_player(0)
{
//	Q_INIT_RESOURCE(VolumeToolBar);
}

QObject* VolumeToolBar::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::ToolBar:
			Q_ASSERT(m_player);
			return new Implementation(m_player, qobject_cast<QWidget*>(parent));
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString VolumeToolBar::pluginName() const
{
	return tr("Volume Slider Tool-bar");
}

QString VolumeToolBar::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString VolumeToolBar::uniqueId() const
{
	return "org.jerboaplayer.VolumeToolBar";
}

QSet<Jerboa::Plugin::ComponentType> VolumeToolBar::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::ToolBar;
}

void VolumeToolBar::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::Player:
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
			return;
		default:
			Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_VolumeToolBar, VolumeToolBar);
