#ifndef _START_PLAYBACK_ON_TRACK_APPEND
#define _START_PLAYBACK_ON_TRACK_APPEND

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "JerboaPlugin.h"

#include <QObject>

class StartPlaybackOnTrackAppend: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		StartPlaybackOnTrackAppend();
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	private slots:
		void tracksAdded(int index, const QList<Jerboa::TrackData>& tracks);
	private:
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};

#endif
