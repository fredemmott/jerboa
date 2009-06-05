#pragma once

#include <QToolBar>

namespace Jerboa
{
	class ToolBar : public QToolBar
	{
		Q_OBJECT;
		public:
			ToolBar(const QString& title, QWidget* parent = 0);
			ToolBar(QWidget* parent = 0);
			virtual Qt::ToolBarArea initialArea() const;
	};
};
