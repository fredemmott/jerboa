#include "NestedCollectionModel.h"

#include <QDebug>
#include <QtPlugin>

NestedCollectionModel::NestedCollectionModel()
	:
		QObject(0),
		m_collectionSource(0)
{
}

QObject* NestedCollectionModel::component(Jerboa::Plugin::ComponentType type, QObject* parent) const
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionModel:
			Q_ASSERT(m_collectionSource);
			qDebug() << "Asked to create collection model";
			//return new Implementation(parent);
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

QString NestedCollectionModel::pluginName() const
{
	return tr("Artist/Album/Title Collection");
}

QString NestedCollectionModel::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString NestedCollectionModel::uniqueId() const
{
	return "org.jerboaplayer.NestedCollectionModel";
}

QSet<Jerboa::Plugin::ComponentType> NestedCollectionModel::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::CollectionModel;
}

void NestedCollectionModel::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionSource:
			m_collectionSource = qobject_cast<Jerboa::CollectionInterface*>(component);
			Q_ASSERT(m_collectionSource);
			return;
		default:
			Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_NestedCollectionModel, NestedCollectionModel);
