#include "MainWindow.h"

#include <QDebug>
#include <QSplitter>
#include <QTabWidget>
#include <QToolBar>

MainWindow::MainWindow(
	Jerboa::PlayerInterface* player,
	QWidget* parent
)
:
	QMainWindow(parent),
	m_player(player)
{
	m_toolBar = new QToolBar(this);
	addToolBar(m_toolBar);
	setUnifiedTitleAndToolBarOnMac(true);

	m_splitter = new QSplitter(Qt::Horizontal, this);
	setCentralWidget(m_splitter);

	m_leftTabs = new QTabWidget(m_splitter);
	m_leftTabs->setTabPosition(QTabWidget::West);
	m_splitter->addWidget(m_leftTabs);

	m_rightTabs = new QTabWidget(m_splitter);
	m_rightTabs->setTabPosition(QTabWidget::East);
	m_splitter->addWidget(m_rightTabs);
}

void MainWindow::addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component)
{
	switch(componentType)
	{
		case Jerboa::Plugin::CollectionView:
			m_leftTabs->insertTab(0, component, tr("Collection"));
			break;
		default:
			qFatal("MainWindow doesn't handle component type %d", componentType);
	}
}
