#include "LastFmTagFetcher_Track.h"
#include "LastFmTagFetcher_Track_p.h"

LastFmTagFetcher::Track::Track(unsigned int identifier, const QString& artist, const QString& album, const QString& title)
: d(new Private())
{
	d->identifier = identifier;
	d->artist = artist;
	d->album = album;
	d->title = title;
}

LastFmTagFetcher::Track::~Track()
{
}

LastFmTagFetcher::Track::Track(const Track& other)
: d(other.d)
{
}

LastFmTagFetcher::Track& LastFmTagFetcher::Track::operator=(const LastFmTagFetcher::Track& other)
{
	d = other.d;
	return *this;
}

unsigned int LastFmTagFetcher::Track::identifier() const
{
	return d->identifier;
}

QString LastFmTagFetcher::Track::artist() const
{
	return d->artist;
}

QString LastFmTagFetcher::Track::album() const
{
	return d->album;
}

QString LastFmTagFetcher::Track::title() const
{
	return d->title;
}
