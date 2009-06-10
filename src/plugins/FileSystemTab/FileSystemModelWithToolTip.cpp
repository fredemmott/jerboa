#include "FileSystemModelWithToolTip.h"

FileSystemModelWithToolTip::FileSystemModelWithToolTip(QObject* parent)
: QFileSystemModel(parent)
{
}

QVariant FileSystemModelWithToolTip::data(const QModelIndex& index, int role) const
{
	if(index.isValid() && role == Qt::ToolTipRole)
	{
		return QFileSystemModel::data(index, QFileSystemModel::FilePathRole);
	}
	else
	{
		return QFileSystemModel::data(index, role);
	}
}
