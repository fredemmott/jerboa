#include "LastFmTagFetcher_Tag.h"
#include "LastFmTagFetcher_Tag_p.h"

LastFmTagFetcher::Tag::Tag(const QString& tag, const qreal& weight)
: d(new Private())
{
	d->name = tag;
	d->weight = weight;
}

LastFmTagFetcher::Tag::Tag(const Tag& other)
: d(other.d)
{
}

LastFmTagFetcher::Tag& LastFmTagFetcher::Tag::operator=(const Tag& other)
{
	d = other.d;
	return *this;
}

qreal LastFmTagFetcher::Tag::weight() const
{
	return d->weight;
}

QString LastFmTagFetcher::Tag::name() const
{
	return d->name;
}
