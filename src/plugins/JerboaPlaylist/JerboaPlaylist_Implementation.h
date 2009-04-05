#ifndef _JERBOA_PLAYLIST_IMPLEMENTATION_H
#define _JERBOA_PLAYLIST_IMPLEMENTATION_H

#include "JerboaPlaylist.h"
#include "PlaylistInterface.h"

class JerboaPlaylist::Implementation : public Jerboa::PlaylistInterface
{
	Q_OBJECT;
	public:
		Implementation(QObject* parent = 0);

		LoopMode loopMode() const;
		ShuffleMode shuffleMode() const;
		int currentTrack() const;
		int nextTrack() const;
		QList<Jerboa::TrackData> tracks() const;
	public slots:
		int appendTrack(const Jerboa::TrackData& data);
		void insertTrack(int index, const Jerboa::TrackData& data);
		void removeTrack(int index);

		void setLoopMode(LoopMode);
		void setShuffleMode(ShuffleMode);
		void setCurrentTrack(int index);
	private:
		void adjustNextTrack();

		LoopMode m_loopMode;
		ShuffleMode m_shuffleMode;
		int m_currentTrack;
		int m_nextTrack;
		QList<Jerboa::TrackData> m_tracks;

};

#endif
