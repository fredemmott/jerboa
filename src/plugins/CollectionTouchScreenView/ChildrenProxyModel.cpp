#include "ChildrenProxyModel.h"

ChildrenProxyModel::ChildrenProxyModel(QObject* parent)
: QAbstractProxyModel(parent)
{
}

int ChildrenProxyModel::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid())
	{
		return 0;
	}
	else
	{
		return sourceModel()->rowCount(root());
	}
}

int ChildrenProxyModel::columnCount(const QModelIndex& parent) const
{
	return sourceModel()->columnCount(mapToSource(parent));
}

QModelIndex ChildrenProxyModel::root() const
{
	return m_root;
}

void ChildrenProxyModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
	if(sourceModel())
	{
		disconnect(sourceModel(), 0, this, 0);
	}
	QAbstractProxyModel::setSourceModel(newSourceModel);

	connect(
		newSourceModel,
		SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)),
		this,
		SLOT(sourceRowsAboutToBeInserted(QModelIndex, int, int))
	);
	connect(
		newSourceModel,
		SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
		this,
		SLOT(sourceRowsAboutToBeRemoved(QModelIndex, int, int))
	);
	connect(
		newSourceModel,
		SIGNAL(rowsInserted(QModelIndex, int, int)),
		this,
		SLOT(sourceRowsInserted(QModelIndex, int, int))
	);
	connect(
		newSourceModel,
		SIGNAL(rowsRemoved(QModelIndex, int, int)),
		this,
		SLOT(sourceRowsRemoved(QModelIndex, int, int))
	);
}

void ChildrenProxyModel::sourceRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
	if(parent == root())
	{
		beginInsertRows(QModelIndex(), start, end);
	}
}

void ChildrenProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
	if(parent == root())
	{
		beginRemoveRows(QModelIndex(), start, end);
	}
}

void ChildrenProxyModel::sourceRowsInserted(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(start);
	Q_UNUSED(end);
	if(parent == root())
	{
		endInsertRows();
	}
}

void ChildrenProxyModel::sourceRowsRemoved(const QModelIndex& parent, int start, int end)
{
	Q_UNUSED(start);
	Q_UNUSED(end);
	if(parent == root())
	{
		endRemoveRows();
	}
}

void ChildrenProxyModel::setRoot(const QModelIndex& sourceParent)
{
	m_root = sourceParent;
	reset();
}

QModelIndex ChildrenProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if(!sourceIndex.isValid())
	{
		return QModelIndex();
	}
	Q_ASSERT(sourceIndex.model() == sourceModel());
	if(sourceIndex.parent() != m_root)
	{
		return QModelIndex();
	}
	return createIndex(sourceIndex.row(), sourceIndex.column());
}

QModelIndex ChildrenProxyModel::index(int row, int column, const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return createIndex(row, column);
}

QModelIndex ChildrenProxyModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

QModelIndex ChildrenProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if(!proxyIndex.isValid())
	{
		return m_root;
	}
	Q_ASSERT(proxyIndex.model() == this);
	return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), root());
}
