#include "ToolBar.h"

namespace Jerboa
{
	ToolBar::ToolBar(const QString& title, QWidget* parent)
	: QToolBar(title, parent)
	{
	}

	ToolBar::ToolBar(QWidget* parent)
	: QToolBar(parent)
	{
	}

	Qt::ToolBarArea ToolBar::initialArea() const
	{
		return Qt::TopToolBarArea;
	}
};
