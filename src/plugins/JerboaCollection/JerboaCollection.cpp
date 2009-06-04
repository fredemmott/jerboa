#include "JerboaCollection.h"

#include "JerboaCollection_FirstRunWizard.h"
#include "JerboaCollection_Implementation.h"

#include <QtPlugin>

QObject* JerboaCollection::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionSource:
			Q_ASSERT(m_tagReader);
			return new Implementation(m_tagReader, parent);
		case Jerboa::Plugin::FirstRunWizardPage:
			return new JerboaCollection::FirstRunWizard(qobject_cast<QWidget*>(parent));
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

JerboaCollection::JerboaCollection()
	:
		QObject(0),
		m_tagReader(0)
{
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
	return QSet<Jerboa::Plugin::ComponentType>()
		<< Jerboa::Plugin::CollectionSource
		<< Jerboa::Plugin::FirstRunWizardPage
	;
}

void JerboaCollection::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case TagReader:
			m_tagReader = qobject_cast<Jerboa::TagReader*>(component);
			Q_ASSERT(m_tagReader);
			break;
		default:
			break;
	}
	Jerboa::Plugin::addComponent(type, component);
}

Q_EXPORT_PLUGIN2(Jerboa_JerboaCollection, JerboaCollection);
