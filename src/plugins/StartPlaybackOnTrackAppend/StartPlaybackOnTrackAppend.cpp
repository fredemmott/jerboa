#include "StartPlaybackOnTrackAppend.h"

#include <QtPlugin>

StartPlaybackOnTrackAppend::StartPlaybackOnTrackAppend()
: QObject(0)
, m_player(0)
, m_playlist(0)
{
}

QString StartPlaybackOnTrackAppend::pluginName() const
{
	return tr("Autostart playback");
}

QString StartPlaybackOnTrackAppend::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString StartPlaybackOnTrackAppend::uniqueId() const
{
	return "org.jerboaplayer.StartPlaybackOnTrackAppend";
}

void StartPlaybackOnTrackAppend::addComponent(ComponentType type, QObject* component)
{
	if(type == Jerboa::Plugin::PlaylistSource)
	{
		m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
		Q_ASSERT(m_playlist);
		connect(
			m_playlist,
			SIGNAL(tracksAdded(int, QList<Jerboa::TrackData>)),
			this,
			SLOT(tracksAdded(int, QList<Jerboa::TrackData>))
		);
	}
	else if(type == Jerboa::Plugin::Player)
	{
		m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
		Q_ASSERT(m_player);
	}
	Jerboa::Plugin::addComponent(type, component);
}

void StartPlaybackOnTrackAppend::tracksAdded(int index, const QList<Jerboa::TrackData>& tracks)
{
	Q_ASSERT(m_playlist);
	Q_ASSERT(m_player);
	if(index + tracks.count() == m_playlist->tracks().count() && m_player->state() != Jerboa::PlayerInterface::Playing)
	{
		m_player->skipTo(index);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_StartPlaybackOnTrackAppend, StartPlaybackOnTrackAppend);
