#pragma once

#include "LastFmTagFetcher.h"

#include <QSharedData>
#include <QString>

class LastFmTagFetcher::Track
{
	public:
		Track(unsigned int identifier, const QString& artist, const QString& album, const QString& title);
		Track(const Track& other);
		Track& operator=(const Track& other);
		~Track();

		unsigned int identifier() const;
		QString artist() const;
		QString album() const;
		QString title() const;
	private:
		class Private;
		QSharedDataPointer<Private> d;
};
