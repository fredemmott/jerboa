#ifndef _JERBOA_PLAYER_INTERFACE_H
#define _JERBOA_PLAYER_INTERFACE_H

#include "TrackData.h"

#include <QFlags>

namespace Jerboa
{
	class PlaylistInterface;
	class PlayerInterface : public QObject
	{
		Q_OBJECT
		public:
			enum State
			{
				Loading,
				Playing,
				Paused,
				Stopped
			};
			enum Feature
			{
				Volume,
				VolumeDecibel,
				Position
			};
			Q_DECLARE_FLAGS(Features, Feature);

			virtual ~PlayerInterface();

			virtual Features features() const;

			virtual qreal volume() const;
			virtual qreal volumeDecibel() const;
			virtual quint64 position() const;

			virtual State state() const = 0;
			virtual TrackData currentTrack() const = 0;
		public slots:
			virtual void play() = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;

			void next();
			void skipTo(int playlistIndex);

			virtual void setVolume(qreal);
			virtual void setVolumeDecibel(qreal);
			virtual void setPosition(quint64);
		signals:
			void stateChanged(Jerboa::PlayerInterface::State);
			void currentTrackChanged(const Jerboa::TrackData&);
			void volumeChanged(qreal);
			void positionChanged(quint64);
		protected:
			virtual void setCurrentTrack(const TrackData& track) = 0;
			PlayerInterface(PlaylistInterface*, QObject* parent);
		private:
			class Private;
			Private* d;
	};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Jerboa::PlayerInterface::Features);

#endif
