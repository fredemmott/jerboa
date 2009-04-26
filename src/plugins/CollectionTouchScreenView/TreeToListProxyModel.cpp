#include "TreeToListProxyModel.h"

TreeToListProxyModel::TreeToListProxyModel(QObject* parent)
: QAbstractProxyModel(parent)
{
}

QModelIndex TreeToListProxyModel::parent(const QModelIndex& child) const
{
	if(mapToSource(child) == m_rootIndex)
	{
		return QModelIndex();
	}
	else
	{
		return mapfromSource(m_rootIndex);
	}
}

QModelIndex TreeToListProxyModel::rootIndex() const
{
	return m_rootIndex;
}

void TreeToListProxyModel::setRootIndex(const QModelIndex& index)
{
	m_rootIndex = index;
}

int TreeToListProxyModel::rowCount(const QModelIndex& index) const
{
	if(index == m_rootIndex)
	{
		return sourceModel()->rowCount(mapToSource(index));
	}
	else
	{
		return 0;
	}

QModelIndex TreeToListProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if(sourceIndex == m_rootIndex)
	{
		return QModelIndex();
	}
	else
	{
		return createIndex(sourceIndex.row(), sourceIndex.column());
	}
}

QModelIndex TreeToListProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if(!proxyIndex.isValid())
	{
		return m_rootIndex;
	}
	else
	{
		return sourceModel->index(proxyIndex.row(), proxyIndex.column(), rootIndex());
	}
}
