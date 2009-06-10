#pragma once

#include "Plugin.h"

#include <QObject>

namespace Jerboa
{
	class CollectionInterface;
	class PlaylistInterface;
}

class Boffin: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		Boffin();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;

		QSet<ComponentType> components() const;
		void addComponent(ComponentType, QObject* component);
		QObject* component(ComponentType, QObject* parent);
	private:
		Jerboa::CollectionInterface* m_collection;
		Jerboa::PlaylistInterface* m_playlist;
};
