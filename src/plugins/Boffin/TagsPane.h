#pragma once

#include <QWidget>

class TagCloudModel;
class TagCloudView;

namespace Jerboa {
	class CollectionInterface;
	class PlaylistInterface;
}

class QPushButton;

class TagsPane : public QWidget
{
	Q_OBJECT;
	public:
		TagsPane(Jerboa::CollectionInterface* collection, Jerboa::PlaylistInterface* m_playlist, QWidget* parent);
	private slots:
		void readTags();
	private:
		Jerboa::CollectionInterface* m_collection;
		Jerboa::PlaylistInterface* m_playlist;
		TagCloudModel* m_model;
		TagCloudView* m_view;
		QPushButton* m_addButton;
};
