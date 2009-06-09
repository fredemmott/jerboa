#include "WeightedTag.h"
#include "WeightedTag_p.h"

WeightedTag::WeightedTag(const QString& tag, const qreal& weight)
: d(new Private())
{
	d->name = tag;
	d->weight = weight;
}

WeightedTag::WeightedTag(const WeightedTag& other)
: d(other.d)
{
}

WeightedTag::~WeightedTag()
{
}

WeightedTag& WeightedTag::operator=(const WeightedTag& other)
{
	d = other.d;
	return *this;
}

qreal WeightedTag::weight() const
{
	return d->weight;
}

QString WeightedTag::name() const
{
	return d->name;
}
