#pragma once

#include <QList>
#include <QMultiMap>
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
		void gotTags(const QMultiMap<int, LastFmTagFetcher::Tag>& tags);
		void error(QNetworkReply::NetworkError);
	private slots:
		void parseReply(QNetworkReply* reply);
	private:
		QNetworkAccessManager* m_manager;
};

#include "LastFmTagFetcher_Track.h"
//#include "LastFmTagFetcher_Tag.h"
