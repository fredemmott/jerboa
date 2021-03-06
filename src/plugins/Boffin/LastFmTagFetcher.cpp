#include "LastFmTagFetcher.h"

#include "WeightedTag.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QUrl>

LastFmTagFetcher::LastFmTagFetcher(QObject* parent)
: QObject(parent)
, m_manager(new QNetworkAccessManager(this))
{
	connect(
		m_manager,
		SIGNAL(finished(QNetworkReply*)),
		SLOT(parseReply(QNetworkReply*))
	);
}

void LastFmTagFetcher::findTags(const QList<Track>& tracks)
{
	// Not lines and +join, because last must end with \n
	QString out;

	Q_FOREACH(const Track& track, tracks)
	{
		out.append(
			QString(
				"%1\t%2\t%3\t%4\n"
			).arg(
				QString::number(track.identifier())
			).arg(
				track.artist()
			).arg(
				track.album()
			).arg(
				track.title()
			)
		);
	}

	QNetworkRequest request(QUrl("http://musiclookup.last.fm/trackresolve"));
	QNetworkReply* reply = m_manager->post(request, out.toUtf8());

	connect(
		reply,
		SIGNAL(error(QNetworkReply::NetworkError)),
		this,
		SIGNAL(error(QNetworkReply::NetworkError))
	);
}

void LastFmTagFetcher::parseReply(QNetworkReply* reply)
{
	QMap<unsigned int, QList<WeightedTag> > out;
	Q_FOREACH(const QString& line, QString::fromUtf8(reply->readAll()).split('\n'))
	{
		if(!line.isEmpty())
		{
			QStringList parts = line.split("\t");
			if(!parts.isEmpty())
			{
				bool ok;
				const unsigned int id = parts.takeFirst().toInt(&ok);
				Q_ASSERT(ok);
				if(ok)
				{
					const bool evenParts = parts.count() % 2 == 0;
					Q_ASSERT(evenParts);
					if(evenParts)
					{
						QList<WeightedTag> tags;
						while(!parts.isEmpty())
						{
							const QString name = parts.takeFirst();
							const qreal weight = parts.takeFirst().toDouble();
							tags.append(WeightedTag(name, weight));
						}
						if(!tags.isEmpty())
						{
							out.insert(id, tags);
						}
					}
				}
			}
		}
	}
	emit gotTags(out);
}
