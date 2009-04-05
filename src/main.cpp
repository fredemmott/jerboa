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
#include "core/modeltest.h"
#include "PlaylistInterface.h"
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

	QList<Jerboa::Plugin*> plugins;
	// Create plugins with no dependencies, and index everythign else
	// - MainWindow
	// - CollectionSource
	const QList<Jerboa::Plugin::ComponentType> wantedComponents = QList<Jerboa::Plugin::ComponentType>()
		<< Jerboa::Plugin::PlaylistSource
		<< Jerboa::Plugin::PlaylistModel
		<< Jerboa::Plugin::CollectionSource
		<< Jerboa::Plugin::CollectionModel
		<< Jerboa::Plugin::Player
		<< Jerboa::Plugin::PlaylistView
		<< Jerboa::Plugin::CollectionView
	;
	QMultiMap<Jerboa::Plugin::ComponentType, Jerboa::Plugin*> componentProviders;
	const QList<Jerboa::Plugin::ComponentType> widgetComponents = QList<Jerboa::Plugin::ComponentType>()
		<< Jerboa::Plugin::PlaylistView
		<< Jerboa::Plugin::CollectionView
	;
	QMap<Jerboa::Plugin::ComponentType, QWidget*> componentWidgets;
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
			}
			Q_FOREACH(Jerboa::Plugin::ComponentType component, wantedComponents)
			{
				if(p->components().contains(component))
				{
					componentProviders.insert(component, p);
				}
			}
		}
	}
	// Check we got the main window
	if(!container)
	{
		qFatal("Could not find a container to load.");
	}

	Q_FOREACH(Jerboa::Plugin::ComponentType type, wantedComponents)
	{
		Q_FOREACH(Jerboa::Plugin* p, componentProviders.values(type))
		{
			QObject* component = p->component(type, &app);
			QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>(component);
			if(model)
			{
				new ModelTest(model, &app);
			}
			Q_FOREACH(Jerboa::Plugin* q, plugins)
			{
				q->addComponent(type, component);
			}
			if(widgetComponents.contains(type))
			{
				QWidget* widget = qobject_cast<QWidget*>(component);
				Q_ASSERT(widget);
				componentWidgets.insert(type, widget);
			}
			break;
		}
	}

	QWidget* mainWindow = container->widget();
	
	for(
		QMap<Jerboa::Plugin::ComponentType, QWidget*>::ConstIterator it = componentWidgets.constBegin();
		it != componentWidgets.constEnd();
		++it
	)
	{
		container->addComponent(it.key(), it.value(), mainWindow);
	}

	// Show the main window
	mainWindow->show();

	return app.exec();
}
