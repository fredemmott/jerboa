#pragma once

#include <QList>
#include <QMap>
#include <QNetworkReply>
#include <QObject>

class QNetworkAccessManager;

class LastFmTagFetcher : public QObject
{
	Q_OBJECT;
	public:
		class Track;
		class Tag;
		LastFmTagFetcher(QObject* parent);

		void findTags(const QList<Track>& tracks);
	signals:
		void gotTags(const QMap<unsigned int, QList<LastFmTagFetcher::Tag> >& tags);
		void error(QNetworkReply::NetworkError);
	private slots:
		void parseReply(QNetworkReply* reply);
	private:
		QNetworkAccessManager* m_manager;
};

#include "LastFmTagFetcher_Track.h"
#include "LastFmTagFetcher_Tag.h"
