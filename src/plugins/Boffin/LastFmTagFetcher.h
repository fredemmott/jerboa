#pragma once

#include <QList>
#include <QMap>
#include <QNetworkReply>
#include <QObject>

class WeightedTag;

class QNetworkAccessManager;

class LastFmTagFetcher : public QObject
{
	Q_OBJECT;
	public:
		class Track;
		LastFmTagFetcher(QObject* parent);

		void findTags(const QList<Track>& tracks);
	signals:
		void gotTags(const QMap<unsigned int, QList<WeightedTag> >& tags);
		void error(QNetworkReply::NetworkError);
	private slots:
		void parseReply(QNetworkReply* reply);
	private:
		QNetworkAccessManager* m_manager;
};

#include "LastFmTagFetcher_Track.h"
