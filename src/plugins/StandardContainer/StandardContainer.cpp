#include "StandardContainer.h"

#include <QtPlugin>

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
