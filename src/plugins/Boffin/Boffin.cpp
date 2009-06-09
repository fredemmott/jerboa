#include "Boffin.h"

#include "CollectionTagFetcher.h"

#include <QFile>
#include <QtPlugin>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

Boffin::Boffin()
: QObject(0)
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
		new CollectionTagFetcher(qobject_cast<Jerboa::CollectionInterface*>(component), this);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_Boffin, Boffin);
