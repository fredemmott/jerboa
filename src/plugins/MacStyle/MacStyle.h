#pragma once

#include "Plugin.h"

#include <QObject>

class MacStyle : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		MacStyle();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
};
