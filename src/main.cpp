/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CollectionInterface.h"
#include "Container.h"
#include "StaticPlugins.h"
#include "Plugin.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QDebug>
#include <QMultiMap>
#include <QPluginLoader>
#include <QSqlDatabase>
#include <QSettings>
#include <QWidget>

void setupDatabase()
{
	QSettings settings;
	settings.beginGroup("database");
	QSqlDatabase db = QSqlDatabase::addDatabase(settings.value("type").toString());
	db.setHostName(settings.value("host").toString());
	db.setDatabaseName(settings.value("name").toString());
	db.setUserName(settings.value("user").toString());
	db.setPassword(settings.value("password").toString());
	if(!db.open())
	{
		qFatal("Couldn't connect to database.");
	}
}

int main(int argc, char** argv)
{
	QApplication app(argc,argv);

	app.setOrganizationName("Fred Emmott");
	app.setOrganizationDomain("jerboaplayer.org");
	app.setApplicationName("Jerboa");
	app.setApplicationVersion("ng");
	app.setQuitOnLastWindowClosed(true);

	setupDatabase();

	Jerboa::Container* container = 0;
	QWidget* mainWindow = 0;
	Jerboa::CollectionInterface* collectionSource = 0;
	QAbstractItemModel* collectionModel = 0;
	QWidget* collectionView = 0;
	QMultiMap<Jerboa::Plugin::ComponentType, Jerboa::Plugin*> componentProviders;

	QList<Jerboa::Plugin*> plugins;
	Q_FOREACH(QObject* plugin, QPluginLoader::staticInstances())
	{
		Jerboa::Plugin* p = qobject_cast<Jerboa::Plugin*>(plugin);
		if(p)
		{
			plugins.append(p);
			if(p->components().contains(Jerboa::Plugin::Container))
			{
				container = qobject_cast<Jerboa::Container*>(p->component(Jerboa::Plugin::Container, 0));
				Q_ASSERT(container);
				mainWindow = container->widget();
			}
			if(p->components().contains(Jerboa::Plugin::CollectionSource))
			{
				collectionSource = qobject_cast<Jerboa::CollectionInterface*>(
					p->component(Jerboa::Plugin::CollectionSource, &app)
				);
			}
			if(p->components().contains(Jerboa::Plugin::CollectionModel))
			{
				componentProviders.insert(Jerboa::Plugin::CollectionModel, p);
			}
			if(p->components().contains(Jerboa::Plugin::CollectionView))
			{
				componentProviders.insert(Jerboa::Plugin::CollectionView, p);
			}
		}
	}
	if(!mainWindow)
	{
		qFatal("Could not find a container to load.");
	}

	Q_ASSERT(collectionSource);
	Q_FOREACH(Jerboa::Plugin* p, plugins)
	{
		p->addComponent(Jerboa::Plugin::CollectionSource, collectionSource);
	}
	Q_FOREACH(Jerboa::Plugin* p, componentProviders.values(Jerboa::Plugin::CollectionModel))
	{
		collectionModel = qobject_cast<QAbstractItemModel*>(
			p->component(Jerboa::Plugin::CollectionModel, &app)
		);
		break;
	}

	Q_ASSERT(collectionModel);
	Q_FOREACH(Jerboa::Plugin* p, plugins)
	{
		p->addComponent(Jerboa::Plugin::CollectionModel, collectionModel);
	}
	Q_FOREACH(Jerboa::Plugin* p, componentProviders.values(Jerboa::Plugin::CollectionView))
	{
		collectionView= qobject_cast<QWidget*>(
			p->component(Jerboa::Plugin::CollectionView, mainWindow)
		);
		break;
	}
	Q_ASSERT(collectionView);
	container->addComponent(Jerboa::Plugin::CollectionView, collectionView, mainWindow);

	mainWindow->show();

	return app.exec();
}
