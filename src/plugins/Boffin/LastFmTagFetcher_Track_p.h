#pragma once

#include "LastFmTagFetcher_Track.h"

class LastFmTagFetcher::Track::Private : public QSharedData
{
	public:
		Private()
		: QSharedData()
		, identifier(0)
		{
		}

		Private(const Private& other)
		: QSharedData(other)
		, identifier(other.identifier)
		, artist(other.artist)
		, album(other.album)
		, title(other.title)
		{
		}

		unsigned int identifier;
		QString artist;
		QString album;
		QString title;
};
