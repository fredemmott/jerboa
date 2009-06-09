#pragma once

#include "Plugin.h"

#include <QObject>

class Boffin: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		Boffin();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;

		void addComponent(ComponentType, QObject* component);
};
