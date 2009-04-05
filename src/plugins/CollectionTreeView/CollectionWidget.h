#ifndef _COLLECTION_WIDGET_H
#define _COLLECTION_WIDGET_H

#include "PlaylistInterface.h"

#include <QWidget>

class QAbstractItemModel;
class QModelIndex;
class QTreeView;

class CollectionWidget : public QWidget
{
	Q_OBJECT;
	public:
		CollectionWidget(Jerboa::PlaylistInterface* playlist, QAbstractItemModel* collection, QWidget* parent);
	private slots:
		void addItemToPlaylist(const QModelIndex& index);
	private:
		Jerboa::PlaylistInterface* m_playlist;
		QTreeView* m_treeView;
};

#endif
