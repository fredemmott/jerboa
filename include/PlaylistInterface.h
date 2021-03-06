#ifndef _PLAYLIST_INTERFACE_H
#define _PLAYLIST_INTERFACE_H

#include "TagReader.h"
#include "TrackData.h"

#include <QList>
#include <QObject>

namespace Jerboa
{
	class PlaylistInterface : public QObject
	{
		Q_OBJECT;
		public:
			virtual ~PlaylistInterface();

			/// List of supported looping modes.
			enum LoopMode
			{
				/// No looping at all.
				LoopNone,
				/// Loop the current track.
				LoopTrack,
				/// Loop the current album.
				LoopAlbum,
				/// Loop the entire playlist.
				LoopPlaylist
			};

			/// List of supported shuffle modes.
			enum ShuffleMode
			{
				/// No shuffling.
				ShuffleNone,
				///  Shuffle next track.
				ShuffleTracks,
				/** Shuffle albums.
				 * Plays continuously until the end of an album, then jumps to a random
				 * track in the playlist. The player then goes backwards until it gets
				 * to a track which is in a different album, then goes forwards one more.
				 */
				ShuffleAlbums
			};

			virtual LoopMode loopMode() const = 0;
			virtual ShuffleMode shuffleMode() const = 0;

			virtual int currentTrack() const = 0;
			virtual int nextTrack() const = 0;
			virtual QList<TrackData> tracks() const = 0;
		public slots:
			/** Appends a track.
			 * @returns the position in the playlist, or -1 on failure.
			 */
			virtual int appendTracks(const QList<Jerboa::TrackData>& data) = 0;
			/** Insert tracks into the playlist.
			 * An index of -1 should be taken as "append".
			 */
			virtual void insertTracks(int index, const QList<Jerboa::TrackData>& data) = 0;
			/** Insert tracks into the playlist.
			 * The default implementation will recurse directories,
			 * and use the active TagReader plugin to resolve tags.
			 */
			virtual void insertTracks(int index, const QList<QUrl>& urls);
			/// Remove tracks from the playlist.
			virtual void removeTracks(int index, int count) = 0;
			/** Remove all items from the playlist.
			 * Default implementation calls removeTracks(0, tracks().count())
			 */
			virtual void clear();

			virtual void setLoopMode(LoopMode) = 0;
			virtual void setShuffleMode(ShuffleMode) = 0;
			virtual void setCurrentTrack(int index) = 0;
		signals:
			void loopModeChanged(Jerboa::PlaylistInterface::LoopMode);
			void shuffleModeChanged(Jerboa::PlaylistInterface::ShuffleMode);

			void tracksAdded(int index, const QList<Jerboa::TrackData>& data);
			void tracksRemoved(int index, int count);

			void positionChanged(int index);

			void dataChanged();
		protected:
			PlaylistInterface(Jerboa::TagReader* tagReader, QObject* parent);
		private:
			class Private;
			Private* d;
	};
};

#endif
