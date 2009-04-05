#include "StandardContainer.h"

#include "MainWindow.h"

#include <QtPlugin>

QObject* StandardContainer::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::Container:
			Q_ASSERT(qobject_cast<QWidget*>(parent) || !parent);
			///@FIXME: needs a player interface passing
			return new MainWindow(0, qobject_cast<QWidget*>(parent));
		default:
			return Jerboa::Plugin::component(type, parent);
	}
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

Q_EXPORT_PLUGIN2(Jerboa_StandardContainer, StandardContainer);
