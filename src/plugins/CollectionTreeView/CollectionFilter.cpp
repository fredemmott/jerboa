#include "CollectionFilter.h"

#include "TrackData.h"

#include <QDebug>

CollectionFilter::CollectionFilter(QObject* parent)
	:
		QSortFilterProxyModel(parent)
{
}

QVariant CollectionFilter::data(const QModelIndex& index, int role) const
{
	if(index.isValid() && role == Qt::UserRole && index.column() == 0)
	{
		QList<Jerboa::TrackData> tracks;
		const int rows = rowCount(index);
		if(rows == 0)
		{
			// Track
			tracks.append(QSortFilterProxyModel::data(index, Qt::UserRole).value<QList<Jerboa::TrackData> >());
		}
		else
		{
			// Album or artist, recurse
			for(int i = 0; i < rows; ++i)
			{
				tracks.append(index.child(i, 0).data(Qt::UserRole).value<QList<Jerboa::TrackData> >());
			}
		}
		return QVariant::fromValue(tracks);
	}
	return QSortFilterProxyModel::data(index, role);
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
