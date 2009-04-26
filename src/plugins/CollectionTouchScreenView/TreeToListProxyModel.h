#ifndef _TREE_TO_LIST_PROXY_MODEL_H
#define _TREE_TO_LIST_PROXY_MODEL_H

#include <QAbstractProxyModel>
#include <QPersistentModelIndex>

class TreeToListProxyModel : public QAbstractProxyModel
{
	Q_OBJECT;
	public:
		TreeToListProxyModel(QObject* parent);
		QModelIndex rootIndex() const;
		void setRootIndex(const QModelIndex&);

		// QAbstractItemModel
		int rowCount(const QModelIndex&) const;
		QModelIndex parent(const QModelIndex&) const;

		// QAbstractProxyModel
		QModelIndex mapFromSource(const QModelIndex&) const;
		QModelIndex mapToSource(const QModelIndex&) const;
	private:
		QPersistentModelIndex m_rootIndex;
};

#endif
