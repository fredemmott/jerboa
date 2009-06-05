#include "MainWindow.h"

#include "ToolBar.h"

#include <QDebug>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>

MainWindow::MainWindow(
	QWidget* parent
)
: QMainWindow(parent)
{

	setUnifiedTitleAndToolBarOnMac(true);

	m_splitter = 0;
}

void MainWindow::addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component)
{
	switch(componentType)
	{
		case Jerboa::Plugin::CollectionView:
			component->setWindowTitle(tr("Collection"));
		case Jerboa::Plugin::WidgetUsedWithPlaylist:
			m_leftWidgets.append(component);
			break;
		case Jerboa::Plugin::ToolBar:
			{
				Jerboa::ToolBar* toolBar = qobject_cast<Jerboa::ToolBar*>(component);
				if(toolBar)
				{
					addToolBar(toolBar->initialArea(), toolBar);
				}
				else
				{
					QToolBar* qToolBar = qobject_cast<QToolBar*>(component);
					Q_ASSERT(qToolBar);
					addToolBar(qToolBar);
				}
			}
			break;
		case Jerboa::Plugin::PlaylistView:
			component->setWindowTitle(tr("Playlist"));
		case Jerboa::Plugin::WidgetUsedWithCollection:
			m_rightWidgets.append(component);
			break;
		default:
			qFatal("MainWindow doesn't handle component type %d", componentType);
	}
	setupTabs();
}

void MainWindow::setupTabs()
{
	Q_FOREACH(QWidget* widget, m_leftWidgets + m_rightWidgets)
	{
		widget->setParent(this);
	}
	delete m_splitter;
	m_splitter = new QSplitter(this);
	setCentralWidget(m_splitter);

	if(m_leftWidgets.count() > 1)
	{
		QTabWidget* leftTabs = new QTabWidget(m_splitter);
		leftTabs->setTabPosition(QTabWidget::West);

		Q_FOREACH(QWidget* widget, m_leftWidgets)
		{
			leftTabs->addTab(widget, widget->windowTitle());
		}

		m_splitter->addWidget(leftTabs);
	}
	else if(m_leftWidgets.count() == 1)
	{
		m_splitter->addWidget(m_leftWidgets.first());
	}

	if(m_rightWidgets.count() > 1)
	{
		QTabWidget* rightTabs = new QTabWidget(m_splitter);
		rightTabs->setTabPosition(QTabWidget::West);

		Q_FOREACH(QWidget* widget, m_rightWidgets)
		{
			rightTabs->addTab(widget, widget->windowTitle());
		}

		m_splitter->addWidget(rightTabs);
	}
	else if(m_rightWidgets.count() == 1)
	{
		m_splitter->addWidget(m_rightWidgets.first());
	}
}
