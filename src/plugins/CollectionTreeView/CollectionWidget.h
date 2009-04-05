#ifndef _COLLECTION_WIDGET_H
#define _COLLECTION_WIDGET_H

#include "PlaylistInterface.h"

#include <QWidget>

class QAbstractItemModel;
class QTreeView;

class CollectionWidget : public QWidget
{
	Q_OBJECT;
	public:
		CollectionWidget(Jerboa::PlaylistInterface* playlist, QAbstractItemModel* collection, QWidget* parent);
	private:
		Jerboa::PlaylistInterface* m_playlist;
		QTreeView* m_treeView;
};

#endif
