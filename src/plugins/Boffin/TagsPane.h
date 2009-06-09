#pragma once

#include <QWidget>

class TagCloudModel;
class TagCloudView;

namespace Jerboa { class CollectionInterface; }

class TagsPane : public QWidget
{
	Q_OBJECT;
	public:
		TagsPane(Jerboa::CollectionInterface* collection, QWidget* parent);
	private slots:
		void readTags();
	private:
		TagCloudModel* m_model;
		TagCloudView* m_view;
};
