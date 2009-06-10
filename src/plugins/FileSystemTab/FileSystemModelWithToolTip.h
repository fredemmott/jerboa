#pragma once

#include <QFileSystemModel>

class FileSystemModelWithToolTip : public QFileSystemModel
{
	Q_OBJECT;
	public:
		FileSystemModelWithToolTip(QObject* parent);
		virtual QVariant data(const QModelIndex& index, int role) const;
};
