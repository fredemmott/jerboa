#pragma once

#include <QAbstractProxyModel>

class ChildrenProxyModel : public QAbstractProxyModel
{
	Q_OBJECT;
	public:
		ChildrenProxyModel(QObject* parent);

		QModelIndex root() const;
		void setRoot(const QModelIndex& sourceParent);

		virtual void setSourceModel(QAbstractItemModel* sourceModel);

		// QAbstractItemModel
		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QModelIndex parent(const QModelIndex& index) const;
		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

		// QAbstractProxyModel

		QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
		QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
	private slots:
		void sourceRowsAboutToBeInserted(const QModelIndex& parent, int start, int end);
		void sourceRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
		void sourceRowsInserted(const QModelIndex& parent, int start, int end);
		void sourceRowsRemoved(const QModelIndex& parent, int start, int end);
	private:
		QPersistentModelIndex m_root;
};
