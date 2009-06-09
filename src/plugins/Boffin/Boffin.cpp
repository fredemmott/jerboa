#include "Boffin.h"

#include "TagsPane.h"

#include "CollectionInterface.h"

#include <QFile>
#include <QtPlugin>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

Boffin::Boffin()
: QObject(0)
, m_collection(0)
{
	Q_INIT_RESOURCE(Boffin);
}

QString Boffin::pluginName() const
{
	return tr("Tags");
}

QString Boffin::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString Boffin::uniqueId() const
{
	return "org.jerboaplayer.Boffin";
}

void Boffin::addComponent(ComponentType type, QObject* component)
{
	if(type == Jerboa::Plugin::CollectionSource)
	{
		QSqlDatabase database = QSqlDatabase::database();
		if(!database.tables().contains("Tags"))
		{
			QSqlQuery query;
			QFile sql(":/Boffin/tables.sql");
			sql.open(QIODevice::ReadOnly);
			Q_FOREACH(const QString statement, QString(sql.readAll()).split("\n\n"))
			{
				query.exec(statement);
			}
		}
		m_collection = qobject_cast<Jerboa::CollectionInterface*>(component);
		Q_ASSERT(m_collection);
	}
}

QSet<Jerboa::Plugin::ComponentType> Boffin::components() const
{
	return QSet<ComponentType>() << WidgetUsedWithPlaylist;
}

QObject* Boffin::component(ComponentType type, QObject* parent)
{
	switch(type)
	{
		case WidgetUsedWithPlaylist:
			Q_ASSERT(m_collection);
			return new TagsPane(m_collection, qobject_cast<QWidget*>(parent));
		default:
			return Plugin::component(type, parent);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_Boffin, Boffin);
