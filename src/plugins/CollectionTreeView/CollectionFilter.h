#ifndef _COLLECTION_FILTER_H
#define _COLLECTION_FILTER_H

#include <QSortFilterProxyModel>
#include <QStringList>

class CollectionFilter : public QSortFilterProxyModel
{
	Q_OBJECT;
	public:
		CollectionFilter(QObject* parent);
	public slots:
		void setFilterString(const QString& filter);
	protected:
		bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
	private:
		QStringList m_filterTokens;
		/*
		 * Progressive - see how fast without performance hacks first
		 * Can also whitelist parents
		QString m_previousFilterString;
		QList<QModelIndex> m_matchedLastTime;
		*/
};

#endif
