#pragma once

#include "LastFmTagFetcher.h"

#include "CollectionInterface.h"

#include <QList>
#include <QObject>
#include <QString>

class LastFmTagFetcher;

class CollectionTagFetcher : public QObject
{
	Q_OBJECT;
	public:
		CollectionTagFetcher(Jerboa::CollectionInterface* collection, QObject* parent);
	signals:
		void savedTags();
	private slots:
		void saveTags(const QMap<unsigned int, QList<LastFmTagFetcher::Tag> >& tags);
	private:
		QList<QString> m_urls;
		LastFmTagFetcher* m_tagFetcher;
};
