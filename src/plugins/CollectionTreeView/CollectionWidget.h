#ifndef _COLLECTION_WIDGET_H
#define _COLLECTION_WIDGET_H

#include "PlaylistInterface.h"

#include <QTimer>
#include <QWidget>

class CollectionFilter;

class QAbstractItemModel;
class QLineEdit;
class QModelIndex;
class QTreeView;

class CollectionWidget : public QWidget
{
	Q_OBJECT;
	public:
		CollectionWidget(Jerboa::PlaylistInterface* playlist, QAbstractItemModel* collection, QWidget* parent);
	private slots:
		void addItemToPlaylist(const QModelIndex& index);
		void updateSearch();
		void acceptSearch();
		void expandArtists();
	private:
		QTimer m_timer;
		Jerboa::PlaylistInterface* m_playlist;
		QLineEdit* m_searchBox;
		QTreeView* m_treeView;
		CollectionFilter* m_filter;
};

#endif
