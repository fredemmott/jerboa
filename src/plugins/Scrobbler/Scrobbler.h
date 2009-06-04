
/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _SCROBBLER_H
#define _SCROBBLER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#include "PlayerInterface.h"
#include "TrackData.h"
#include "Plugin.h"

class Scrobbler : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		Scrobbler();
		~Scrobbler();

		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);

		QObject* component(ComponentType, QObject* parent);
		QSet<ComponentType> components() const;
	private slots:
		void loginFinished();
		void submitFinished();
		void error(QNetworkReply::NetworkError code);
		void playerStateChanged(Jerboa::PlayerInterface::State state);
	private:
		class FirstRunWizard;
		void playbackStarted(const Jerboa::TrackData& track, qint64 length);
		void playbackPaused();
		void playbackStopped(); // or end is reached

		Jerboa::PlayerInterface* m_player;

		bool m_haveUserData;
		bool m_online;

		QString m_user;
		QString m_password;
		QString m_session;

		QUrl m_nowPlayingUrl;
		QUrl m_submitUrl;

		time_t m_lastSubmissionTime;
		QNetworkReply* m_submissionReply;

		QNetworkAccessManager m_manager;

		time_t m_startTime;
		time_t m_pauseTime;
		time_t m_failTime;
	
		struct LastFmData
		{
			Jerboa::TrackData data;
 			quint64 length;
		} m_currentTrack;

		void login();
		void submit();
		void queueTrack();
};

#endif
