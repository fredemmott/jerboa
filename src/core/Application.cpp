/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Application.h"

#include "CollectionInterface.h"
#include "Container.h"
#include "FirstRunWizard.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#ifdef WITH_DEBUG_FEATURES
#include "modeltest.h"
#endif

#include <QAbstractItemModel>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMultiMap>
#include <QPluginLoader>
#include <QSqlDatabase>
#include <QSettings>
#include <QWidget>

namespace Jerboa
{
	void Application::setupDatabase()
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

	Application::Application(int& argc, char** argv)
	: QApplication(argc, argv)
	{
		setOrganizationName("Fred Emmott");
		setOrganizationDomain("jerboaplayer.org");
		setApplicationName("Jerboa");
		setApplicationVersion("ng");
		setQuitOnLastWindowClosed(true);

		Q_FOREACH(QObject* plugin, QPluginLoader::staticInstances())
		{
			Jerboa::Plugin* p = qobject_cast<Jerboa::Plugin*>(plugin);
			if(p)
			{
				qDebug() << qPrintable(QObject::tr("Found plugin %1 (%2)").arg(p->uniqueId()).arg(p->pluginName()));
				m_plugins.append(p);
			}
		}

		if(QCoreApplication::arguments().contains("--first-run") || QSettings().value("firstRun", true).toBool() == true)
		{
			loadFirstRunWizard();
		}
		else
		{
			loadMainWindow();
		}
	}

	void Application::loadFirstRunWizard()
	{
		FirstRunWizard* wizard = new FirstRunWizard(m_plugins);
		connect(
			wizard,
			SIGNAL(finished(int)),
			wizard,
			SLOT(deleteLater())
		);
		connect(
			wizard,
			SIGNAL(accepted()),
			this,
			SLOT(loadMainWindow())
		);
		connect(
			wizard,
			SIGNAL(rejected()),
			this,
			SLOT(quit())
		);
		wizard->show();
	}

	void Application::loadMainWindow()
	{
		QSettings().setValue("firstRun", false);
		setupDatabase();
	
		Jerboa::Container* container = 0;
	
		// Create plugins with no dependencies, and index everythign else
		// - MainWindow
		// - CollectionSource
		const QList<Jerboa::Plugin::ComponentType> wantedComponents = QList<Jerboa::Plugin::ComponentType>()
			<< Jerboa::Plugin::TagReader
			<< Jerboa::Plugin::PlaylistSource
			<< Jerboa::Plugin::PlaylistModel
			<< Jerboa::Plugin::CollectionSource
			<< Jerboa::Plugin::CollectionModel
			<< Jerboa::Plugin::Player
			<< Jerboa::Plugin::PlaylistView
			<< Jerboa::Plugin::CollectionView
			<< Jerboa::Plugin::WidgetUsedWithPlaylist
			<< Jerboa::Plugin::WidgetUsedWithCollection
			<< Jerboa::Plugin::ToolBar
		;
		QMultiMap<Jerboa::Plugin::ComponentType, Jerboa::Plugin*> componentProviders;
		const QList<Jerboa::Plugin::ComponentType> widgetComponents = QList<Jerboa::Plugin::ComponentType>()
			<< Jerboa::Plugin::PlaylistView
			<< Jerboa::Plugin::CollectionView
			<< Jerboa::Plugin::WidgetUsedWithPlaylist
			<< Jerboa::Plugin::WidgetUsedWithCollection
			<< Jerboa::Plugin::ToolBar
		;
		QMultiMap<Jerboa::Plugin::ComponentType, QWidget*> componentWidgets;
		Q_FOREACH(Plugin* p, m_plugins)
		{
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
		// Check we got the main window
		if(!container)
		{
			qFatal("Could not find a container to load.");
		}
	
		Q_FOREACH(Jerboa::Plugin::ComponentType type, wantedComponents)
		{
			Q_FOREACH(Jerboa::Plugin* p, componentProviders.values(type))
			{
				QObject* component = p->component(type, container);
#ifdef WITH_DEBUG_FEATURES
				QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>(component);
				if(model)
				{
					new ModelTest(model, this);
				}
#endif
				Q_FOREACH(Jerboa::Plugin* q, m_plugins)
				{
					q->addComponent(type, component);
				}
				if(widgetComponents.contains(type))
				{
					QWidget* widget = qobject_cast<QWidget*>(component);
					Q_ASSERT(widget);
					componentWidgets.insert(type, widget);
				}
			}
		}
	
		QWidget* mainWindow = container->widget();
	
		Q_FOREACH(Jerboa::Plugin::ComponentType type, widgetComponents)
		{
			Q_FOREACH(QWidget* widget, componentWidgets.values(type))
			{
				container->addComponent(type, widget, mainWindow);
			}
		}
	
		// Show the main window
		mainWindow->show();
	}
}
