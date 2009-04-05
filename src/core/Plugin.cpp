#include "Plugin.h"

#include <QDebug>

namespace Jerboa
{
	QUrl Plugin::pluginWebsite() const
	{
		return QUrl();
	}

	void Plugin::addComponent(ComponentType, QObject*)
	{
	}

	QSet<Plugin::ComponentType> Plugin::components() const
	{
		return QSet<ComponentType>();
	}

	QObject* Plugin::component(ComponentType componentType, QObject*) const
	{
		qFatal("Plugin '%s' was asked to create a component of type %d", qPrintable(uniqueId()), componentType);
	}
};
