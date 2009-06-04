#include "JerboaCollection.h"

#include "JerboaCollection_FirstRunWizard.h"
#include "JerboaCollection_Implementation.h"

#include <QFile>
#include <QtPlugin>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

QObject* JerboaCollection::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::CollectionSource:
			{
				Q_ASSERT(m_tagReader);
				QSqlDatabase database;
				if(!database.tables().contains("Albums"))
				{
					QFile sql(":/JerboaCollection/tables.sql");
					sql.open(QIODevice::ReadOnly);
					QSqlQuery query;
					Q_FOREACH(const QString statement, QString(sql.readAll()).split("\n\n"))
					{
						qDebug() << "Executing query" << statement;
						if(!query.exec(statement))
						{
							qDebug() << query.lastError().text();
						}
					}
				}
				return new Implementation(m_tagReader, parent);
			}
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
	Q_INIT_RESOURCE(JerboaCollection);
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
