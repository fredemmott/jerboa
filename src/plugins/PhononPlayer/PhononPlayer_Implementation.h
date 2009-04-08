#ifndef _PHONON_PLAYER_INTERNAL_H
#define _PHONON_PLAYER_INTERNAL_H

#include "PhononPlayer.h"
#include "PlayerInterface.h"

#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/mediaobject.h>

class PhononPlayer::Implementation : public Jerboa::PlayerInterface
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent);

		State state() const;
		Jerboa::TrackData currentTrack() const;

		qreal volume() const;
		qreal volumeDecibel() const;
		qint64 trackLength() const;
	public slots:
		void play();
		void pause();
		void stop();

		void setVolume(qreal);
		void setVolumeDecibel(qreal);
	protected:
		void setCurrentTrack(const Jerboa::TrackData& track);
	private slots:
		void handlePhononStateChange(Phonon::State newState, Phonon::State oldState);
		void queueNextTrack();
	private:
		void setState(State);
		State m_state;
		Jerboa::TrackData m_currentTrack;

		Phonon::AudioOutput* m_output;
		Phonon::MediaObject* m_player;
		Jerboa::PlaylistInterface* m_playlist;
};

#endif
