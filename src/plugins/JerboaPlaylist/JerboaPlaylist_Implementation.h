#ifndef _JERBOA_PLAYLIST_IMPLEMENTATION_H
#define _JERBOA_PLAYLIST_IMPLEMENTATION_H

#include "JerboaPlaylist.h"
#include "PlaylistInterface.h"

class JerboaPlaylist::Implementation : public Jerboa::PlaylistInterface
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::TagReader* tagReader, QObject* parent);

		LoopMode loopMode() const;
		ShuffleMode shuffleMode() const;
		int currentTrack() const;
		int nextTrack() const;
		QList<Jerboa::TrackData> tracks() const;
	public slots:
		int appendTracks(const QList<Jerboa::TrackData>& data);
		void insertTracks(int index, const QList<Jerboa::TrackData>& data);
		void removeTracks(int index, int count);

		void setLoopMode(LoopMode);
		void setShuffleMode(ShuffleMode);
		void setCurrentTrack(int index);
	private:
		int startOfAlbum(int index) const;
		void adjustNextTrack();

		LoopMode m_loopMode;
		ShuffleMode m_shuffleMode;
		int m_currentTrack;
		int m_nextTrack;
		QList<Jerboa::TrackData> m_tracks;

};

#endif
