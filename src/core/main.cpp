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

#include "Plugin.h"

#include <QApplication>
#include <QDebug>
#include <QPluginLoader>
#include <QWidget>

int main(int argc, char** argv)
{
	QApplication app(argc,argv);

	app.setOrganizationName("Jerboa");
	app.setOrganizationDomain("jerboaplayer.org");
	app.setApplicationName("Jerboa");
	app.setApplicationVersion("ng");
	app.setQuitOnLastWindowClosed(true);

	QWidget* mainWindow = 0;
	Q_FOREACH(QObject* plugin, QPluginLoader::staticInstances())
	{
		Jerboa::Plugin* p = qobject_cast<Jerboa::Plugin*>(plugin);
		if(p && p->components().contains(Jerboa::Plugin::Container))
		{
			mainWindow = qobject_cast<QWidget*>(p->component(Jerboa::Plugin::Container, 0));
			break;
		}
	}

	if(mainWindow)
	{
		mainWindow->show();
	}
	else
	{
		qFatal("Could not find a container to load.");
	}

	return app.exec();
}
