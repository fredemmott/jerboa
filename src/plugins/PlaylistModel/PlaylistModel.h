#ifndef _PLAYLIST_MODEL_H
#define _PLAYLIST_MODEL_H

#include "PlaylistInterface.h"
#include "Plugin.h"
#include "TagReader.h"

#include <QObject>

class PlaylistModel: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		PlaylistModel();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
		Jerboa::PlaylistInterface* m_playlistSource;
};

#endif
