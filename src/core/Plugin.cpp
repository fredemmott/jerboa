#include "Plugin.h"

#include <QMetaObject>
#include <QMetaEnum>

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

	QObject* Plugin::component(ComponentType componentType, QObject*)
	{
		qFatal("Plugin '%s' was asked to create a component of type %d", qPrintable(uniqueId()), componentType);
		return 0; // -Wall -Werror
	}
};

QDebug operator<<(QDebug debug, Jerboa::Plugin::ComponentType type)
{
	const QMetaObject metaObject = Jerboa::Plugin::staticMetaObject;
	const int index = metaObject.indexOfEnumerator("ComponentType");
	const QMetaEnum enumerator = metaObject.enumerator(index);
	debug.nospace() << enumerator.valueToKey(type) << " (" << static_cast<int>(type) << ")";
	return debug.space();
}
