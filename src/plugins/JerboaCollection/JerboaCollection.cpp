#include "JerboaCollection.h"

#include "JerboaCollection_Implementation.h"

#include <QtPlugin>

QObject* JerboaCollection::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionSource:
			return new Implementation(parent);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString JerboaCollection::pluginName() const
{
	return tr("Jerboa Collection");
}

QString JerboaCollection::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString JerboaCollection::uniqueId() const
{
	return "org.jerboaplayer.JerboaCollection";
}

QSet<Jerboa::Plugin::ComponentType> JerboaCollection::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::CollectionSource;
}

Q_EXPORT_PLUGIN2(Jerboa_JerboaCollection, JerboaCollection);
