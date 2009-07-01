/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _JERBOA_TYPES_H
#define _JERBOA_TYPES_H

#include <QFlags>
#include <QMap>
#include <QObject>

/// All-containing namespace for Jerboa.
namespace Jerboa
{
	/// List of supported looping modes.
	enum LoopMode
	{
		/// No looping at all.
		LoopNone,
		/// Loop the current track.
		LoopTrack,
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
	
	/// Possible ReplayGain modes
	enum ReplayGainMode {
		/// Normalise volume across a whole album; relative volume differences are kept
		AlbumMode,
		/// Normalise each track separately - relative differences between tracks in the album are lost
		TrackMode
	};

	/** List of component types.
	 * A plugin can provide zero or more components, each implementing
	 * one of these interfaces. These should be returned by
	 * Plugin::components()
	 */
	enum ComponentInterface
	{
		/** A music player.
		 * This should be qobject_cast'able to a PlayerInterface.
		 */
		PlayerCompoment,

		/** A playlist.
		 * This should be qobject_cast'able to a PlaylistInterface.
		 */
		PlaylistComponent
	};
	/// A component list
	typedef QMap<ComponentInterface, QObject*> ComponentMap;

	/** List of possible actions on the player.
	 * The values specified  are compatible with MPRIS.
	 * All of these are supported at some time or another, in varying
	 * combinations.
	 *
	 * There is also a QFlags<Action> class, `Actions'.
	 */
	enum Action
	{
		/// Base action.
		NoAction            = 0,
		/// Skip to next track.
		SkipNextAction      = 1 << 0,
		/// Skip previous track.
		SkipPreviousAction  = 1 << 1,
		/// Pause track
		PauseAction         = 1 << 2,
		/// Play/unpause track
		PlayAction          = 1 << 3,
		/** Seek to a position in the current track.
		 * This is not used internally, and exists only because it's in
		 * MPRIS.
		 */
		SeekAction          = 1 << 4,
		/** Get metadata for the current track.
		 * This is not used internally, and exists only because it's in
		 * MPRIS.
		 */
		GetMetadataAction   = 1 << 5,
		/** Get a list of tracks in the current playlist.
		 * This is not used internally, and exists only because it's in
		 * MPRIS.
		 */
		GetTracklistAction  = 1 << 6,
		/** Stop the current track.
		 * This is only used internally, and is *NOT* part of MPRIS.
		 */
		StopAction          = 1 << 7
	};

	Q_DECLARE_FLAGS(Actions, Action);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Jerboa::Actions);
#endif
