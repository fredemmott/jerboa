#include "CollectionFilter.h"

#include "TrackData.h"

#include <QDebug>

CollectionFilter::CollectionFilter(QObject* parent)
	:
		QSortFilterProxyModel(parent)
{
}

void CollectionFilter::setFilterString(const QString& filter)
{

	if(filter.isEmpty())
	{
		m_filterTokens.clear();
	}
	else
	{
		m_filterTokens = filter.split(' ');
	}
	invalidateFilter();
}

bool CollectionFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	/*
	 * Performance hints temporarily disabled for working on the search algorithm
	if(m_filterTokens.isEmpty())
	{
		return true;
	}
	*/
	const QList<Jerboa::TrackData> tracks = sourceModel()->index(source_row, 0, source_parent).data(Qt::UserRole).value<QList<Jerboa::TrackData> >();
	Q_FOREACH(const Jerboa::TrackData& track, tracks)
	{
		const QString mimeData(track.mimeData());
		bool matched = true;
		Q_FOREACH(const QString& token, m_filterTokens)
		{
			if(!mimeData.contains(token, Qt::CaseInsensitive))
			{
				matched = false;
				break;
			}
		}
		if(matched)
		{
			return true;
		}
	}
	return false;
	
}
