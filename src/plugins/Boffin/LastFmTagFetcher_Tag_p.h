#pragma once

#include "LastFmTagFetcher_Tag.h"

class LastFmTagFetcher::Tag::Private : public QSharedData
{
	public:
		Private()
		: QSharedData()
		, weight(0)
		{
		}
	
		Private(const Private& other)
		: QSharedData(other)
		, weight(other.weight)
		, name(other.name)
		{
		}
	
		qreal weight;
		QString name;
};
