#pragma once

#include <QSharedData>
#include <QString>

class WeightedTag
{
	public:
		WeightedTag(const QString& tag, const qreal& weight);
		WeightedTag(const WeightedTag& other);
		WeightedTag& operator=(const WeightedTag& other);
		~WeightedTag();

		qreal weight() const;
		QString name() const;
	private:
		class Private;
		QSharedDataPointer<Private> d;
};
