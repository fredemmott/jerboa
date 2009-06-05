#include "SeekToolBar.h"
#include "SeekToolBar_Implementation.h"

#include <QDebug>
#include <QtPlugin>

SeekToolBar::SeekToolBar()
	:
		QObject(0),
		m_player(0)
{
	Q_INIT_RESOURCE(SeekToolBar);
}

QObject* SeekToolBar::component(Jerboa::Plugin::ComponentType type, QObject* parent)
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

QString SeekToolBar::pluginName() const
{
	return tr("Seek Slider Tool-bar");
}

QString SeekToolBar::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString SeekToolBar::uniqueId() const
{
	return "org.jerboaplayer.SeekToolBar";
}

QSet<Jerboa::Plugin::ComponentType> SeekToolBar::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::ToolBar;
}

void SeekToolBar::addComponent(ComponentType type, QObject* component)
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

Q_EXPORT_PLUGIN2(Jerboa_SeekToolBar, SeekToolBar);
