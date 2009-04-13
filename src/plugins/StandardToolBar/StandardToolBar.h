#ifndef _STANDARD_TOOL_BAR_H
#define _STANDARD_TOOL_BAR_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QObject>

class StandardToolBar : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		StandardToolBar();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};

#endif
