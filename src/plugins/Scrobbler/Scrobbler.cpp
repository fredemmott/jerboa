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
#include "Scrobbler.h"
#include "Scrobbler_FirstRunWizard.h"

#include "Utilities.h"

#include <QtPlugin>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>

Scrobbler::Scrobbler() : QObject()
{
	Q_INIT_RESOURCE(Scrobbler);

	m_online = false;
	m_failTime = 0;
	m_submissionReply = 0;
	m_haveUserData = false;
};

void Scrobbler::addComponent(ComponentType type, QObject* component)
{
	if(type == Player)
	{
		if(!QSqlDatabase::database().tables().contains("LastFMCache"))
		{
			QFile file(":/Scrobbler/tables.sql");
			file.open(QIODevice::ReadOnly);
			QSqlQuery query;
			Q_FOREACH(const QString& statement, QString(file.readAll()).split("\n\n"))
			{
				query.exec(statement);
			}
		}
		m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
		Q_ASSERT(m_player);

		connect(
			component, SIGNAL(stateChanged(Jerboa::PlayerInterface::State)),
			this, SLOT(playerStateChanged(Jerboa::PlayerInterface::State))
		);
		QSettings settings;
		QString user = settings.value("lastfm/user", "").toString();
		QString password = settings.value("lastfm/password", "").toString();
	
		if ( user.isEmpty() )
		{
			m_haveUserData = false;
			return;
		}
	
		m_haveUserData = true;
		m_user = user;
		m_password = password;
		login();
	}
}

void Scrobbler::login()
{
	if(! m_haveUserData)
	{	
		return;
	}
	if(QDateTime::currentDateTime().toTime_t() - m_failTime < 30 )
	{
		return;
	}
	m_failTime = 0;
	m_online = false;

	QString timestamp = QString::number(QDateTime::currentDateTime().toTime_t());

	QUrl initURL("http://post.audioscrobbler.com/");
	initURL.addQueryItem("hs", "true"); // handshake
	initURL.addQueryItem("p", "1.2"); // protocol version
	
	// If you're looking at this code for work on another player,
	// please don't copy the client and client version - ask for
	// your own ID from Russ at last.fm - in the mean time, you can
	// use "tst" version "1.0" for testing.

	initURL.addQueryItem("c", "yan"); // client
	initURL.addQueryItem("v", "0.2"); // client version
	initURL.addQueryItem("u", m_user); // user
	initURL.addQueryItem("t", timestamp); // timestamp
	initURL.addQueryItem(
		"a",
		QCryptographicHash::hash(
			QString(m_password + timestamp).toUtf8(),
			QCryptographicHash::Md5
		).toHex()
	); // Auth token

	QNetworkReply* reply = m_manager.get(QNetworkRequest(initURL));
	connect(reply, SIGNAL(finished()), this, SLOT(loginFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

QObject* Scrobbler::component(ComponentType type, QObject* parent)
{
	Q_ASSERT(type == FirstRunWizardPage);
	return new FirstRunWizard(qobject_cast<QWidget*>(parent));
}

QSet<Jerboa::Plugin::ComponentType> Scrobbler::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>()
		<< FirstRunWizardPage
	;
}

void Scrobbler::error(QNetworkReply::NetworkError error)
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT(reply);

	qDebug() << "An error occured:" << error;
	qDebug() << "URL:" << reply->url();
	qDebug() << "Operation:" << reply->operation();
	m_failTime = QDateTime::currentDateTime().toTime_t();
	m_online = false;
}

void Scrobbler::loginFinished()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT(reply);

	QString data = reply->readAll();
	QStringList lines = data.split("\n");
	if ( lines[0] != "OK" )
	{
		qDebug() << "LastFM auth failed:" << data << reply->error();
		if ( lines[0] != "BANNED" && lines[0] != "BADAUTH" && lines[0] != "BADTIME" )
			login();
		return;
	}
	qDebug() << "LastFM auth successful";

	m_session = lines[1];
	m_nowPlayingUrl.setUrl(lines[2]);
	m_submitUrl.setUrl(lines[3]);

	m_online = true;

	submit();
	reply->close();
}

void Scrobbler::submit()
{
	if(!m_online)
	{
		login();
		return;
	}
	if(m_submissionReply)
	{
		return;
	}

	QSqlQuery query;
	query.exec("SELECT `TimeStamp`, `Length`, `Artist`, `Album`, `Name`, `TrackNumber`, `MBID` FROM `LastFMCache` ORDER BY `TimeStamp` ASC LIMIT 50");
	if(! query.first())
	{
		return;
	}
		
	unsigned int i = 0;
	QUrl submitURL(m_submitUrl);
	submitURL.addQueryItem("s", m_session);
	do
	{
		QSqlRecord r = query.record();
		time_t timestamp = r.value(0).value<time_t>();
		quint64 length = r.value(1).value<quint64>();
		QString artist = r.value(2).toString();
		QString album = r.value(3).toString();
		QString title = r.value(4).toString();
		unsigned int trackNumber = r.value(5).toUInt();
		QString mbid = r.value(6).toString();

		submitURL.addQueryItem(QString("a[%1]").arg(i), artist);
		submitURL.addQueryItem(QString("t[%1]").arg(i), title);
		submitURL.addQueryItem(QString("n[%1]").arg(i), trackNumber == 0 ? "" : QString::number(trackNumber));
		submitURL.addQueryItem(QString("m[%1]").arg(i), mbid);
		submitURL.addQueryItem(QString("b[%1]").arg(i), album);
		submitURL.addQueryItem(QString("i[%1]").arg(i), QString::number(timestamp));
		submitURL.addQueryItem(QString("o[%1]").arg(i), "P");
		submitURL.addQueryItem(QString("r[%1]").arg(i), "");
		submitURL.addQueryItem(QString("l[%1]").arg(i), QString::number(length));

		m_lastSubmissionTime = timestamp;
		i++;
	} while(query.next());

	QByteArray data = QString(submitURL.encodedQuery().replace("%5B", "[").replace("%5D", "]")).toUtf8();

	QNetworkRequest request(m_submitUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	m_submissionReply = m_manager.post(request, data);
	connect(m_submissionReply, SIGNAL(finished()), this, SLOT(submitFinished()));
	connect(m_submissionReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

void Scrobbler::submitFinished()
{
	QString data = m_submissionReply ->readAll();
	m_submissionReply->close();
	QStringList lines = data.split("\n");
	m_submissionReply = 0;

	if ( lines[0] != "OK" )
	{
		qDebug() << "Failed submission:" << data;
		if ( lines[0] == "BADSESSION" )
		{
			qDebug() << "Retrying login...";
			m_online = false;
			login();
		}
		return;
	}
	QSqlQuery query;
	query.prepare(QString("DELETE FROM `LastFMCache` WHERE `TimeStamp` <= %1").arg(m_lastSubmissionTime));
	query.exec();
	qDebug() << "Flushed last.fm cache block";
	submit();
}

void Scrobbler::playbackStarted(const Jerboa::TrackData& track, qint64 length)
{
	if(m_online) // Last.fm "Now Playing"
	{
		QUrl submitURL(m_nowPlayingUrl);
		submitURL.addQueryItem("s", m_session);
		submitURL.addQueryItem("a", track.artist());
		submitURL.addQueryItem("t", track.title());
		submitURL.addQueryItem("b", Jerboa::Utilities::simpleAlbum(track.album()));
		submitURL.addQueryItem("n", track.trackNumber() != 0 ? QString::number(track.trackNumber()) : "");
		submitURL.addQueryItem("m", track.musicBrainzId() != "NONE" ? track.musicBrainzId() : "");
		submitURL.addQueryItem("l", QString::number(length / 1000)); // msec -> sec
		QByteArray data = QString(submitURL.encodedQuery()).toUtf8();

		QNetworkRequest request(m_nowPlayingUrl);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

		QNetworkReply* reply = m_manager.post(request, data);
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
	}

	// Track playback time for scrobbling

	if(m_currentTrack.data == track) // unpause
	{
		m_startTime += QDateTime::currentDateTime().toTime_t() - m_pauseTime;
		m_pauseTime = 0;
		return;
	}
	else
	{
		queueTrack();
		submit();
	}

	m_currentTrack.data = track;
	m_currentTrack.length = length / 1000;  // msec -> sec

	m_startTime = QDateTime::currentDateTime().toTime_t();
	m_pauseTime = 0;
};

void Scrobbler::playbackPaused()
{
	m_pauseTime = QDateTime::currentDateTime().toTime_t();
}

QString Scrobbler::pluginName() const
{
	return tr("Last.fm Scrobbler");
}

QString Scrobbler::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString Scrobbler::uniqueId() const
{
	return "org.jerboaplayer.Scrobbler";
}

void Scrobbler::queueTrack()
{
	if(! m_currentTrack.data.isValid())
	{
		return;
	}
	const quint64 total = m_currentTrack.length;
	if(m_startTime != 0 && total > 30 && QDateTime::currentDateTime().toTime_t() - m_startTime > qMin<time_t>(240, total / 2) )
	{
		QSqlQuery query;	
		query.prepare("INSERT INTO `LastFMCache` (`TimeStamp`, `Length`, `Artist`, `Album`, `Name`, `TrackNumber`, `MBID`) \
				VALUES(:timestamp, :length, :artist, :album, :title, :trackNumber, :mbid)");
		query.bindValue(":timestamp", QString::number(QDateTime::currentDateTime().toTime_t()));
		query.bindValue(":length", QString::number(total));

		const Jerboa::TrackData& track = m_currentTrack.data;
		query.bindValue(":artist", track.artist());
		query.bindValue(":album", Jerboa::Utilities::simpleAlbum(track.album()));
		query.bindValue(":title", track.title());
		query.bindValue(":trackNumber", track.trackNumber());
		query.bindValue(":mbid", track.musicBrainzId().isEmpty() ? "" : track.musicBrainzId());
		query.exec();
		qDebug() << "Added to LastFM cache.";
	}
	m_currentTrack.data = Jerboa::TrackData();
}

void Scrobbler::playerStateChanged(Jerboa::PlayerInterface::State state)
{
	switch(state)
	{
		case Jerboa::PlayerInterface::Playing:
			playbackStarted(m_player->currentTrack(), m_player->trackLength());
			break;
		case Jerboa::PlayerInterface::Paused:
			playbackPaused();
			break;
		case Jerboa::PlayerInterface::Stopped:
			playbackStopped();
			break;
		default:
			break;
	}
}

void Scrobbler::playbackStopped()
{
	queueTrack();
	submit();
	m_currentTrack.data = Jerboa::TrackData();
}

Scrobbler::~Scrobbler()
{
	queueTrack();
}

Q_EXPORT_PLUGIN2(Jerboa_Scrobbler, Scrobbler);
