#ifndef _JERBOA_PLAYER_INTERFACE_H
#define _JERBOA_PLAYER_INTERFACE_H

#include "TrackData.h"

namespace Jerboa
{
	class PlayerInterface : public QObject
	{
		Q_OBJECT
		public:
			virtual ~PlayerInterface();

			virtual qreal volume() const = 0;
			virtual qreal volumeDecibel() const = 0;
			virtual quint64 position() const = 0;

			virtual TrackData currentTrack() const = 0;
		public slots:
			virtual void play() = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;

			virtual void previous() = 0;
			virtual void next() = 0;

			virtual void setVolume(qreal) = 0;
			virtual void setVolumeDecibel(qreal) = 0;
			virtual void setPosition(quint64) = 0;
		signals:
			void currentTrackChanged(const TrackData&);
			void volumeChanged(qreal);
			void positionChanged(quint64);
		protected:
			PlayerInterface(QObject* parent);
	};
};

#endif
