#pragma once

#include "LastFmTagFetcher.h"

#include <QSharedData>
#include <QString>

class LastFmTagFetcher::Tag
{
	public:
		Tag(const QString& tag, const qreal& weight);
		Tag(const Tag& other);
		Tag& operator=(const Tag& other);

		qreal weight() const;
		QString name() const;
	private:
		class Private;
		QSharedDataPointer<Private> d;
};
