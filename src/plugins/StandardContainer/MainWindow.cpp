#include "MainWindow.h"

#include <QAction>
#include <QDebug>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>

MainWindow::MainWindow(
	Jerboa::PlayerInterface* player,
	Jerboa::PlaylistInterface* playlist,
	QWidget* parent
)
:
	QMainWindow(parent),
	m_player(player),
	m_playlist(playlist)
{
	connect(
		m_player,
		SIGNAL(stateChanged(Jerboa::PlayerInterface::State)),
		this,
		SLOT(handlePlayerState(Jerboa::PlayerInterface::State))
	);

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
	
	setupToolBar();
}

void MainWindow::setupToolBar()
{
	m_playAction = m_toolBar->addAction(tr("Play"), this, SLOT(play()));
	m_pauseAction = m_toolBar->addAction(tr("Pause"), this, SLOT(pause()));
	m_stopAction = m_toolBar->addAction(tr("Stop"), this, SLOT(stop()));

	m_pauseAction->setEnabled(false);
	m_stopAction->setEnabled(false);
}

void MainWindow::handlePlayerState(Jerboa::PlayerInterface::State state)
{
	m_playAction->setEnabled(state == Jerboa::PlayerInterface::Paused || state == Jerboa::PlayerInterface::Stopped);
	m_pauseAction->setEnabled(state == Jerboa::PlayerInterface::Playing);
	m_stopAction->setEnabled(state != Jerboa::PlayerInterface::Stopped);
}

void MainWindow::play()
{
	m_player->play();
}

void MainWindow::pause()
{
	m_player->pause();
}

void MainWindow::stop()
{
	m_player->stop();
}

void MainWindow::addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component)
{
	switch(componentType)
	{
		case Jerboa::Plugin::CollectionView:
			m_leftTabs->insertTab(0, component, tr("Collection"));
			break;
		case Jerboa::Plugin::PlaylistView:
			m_rightTabs->insertTab(0, component, tr("Playlist"));
			break;
		default:
			qFatal("MainWindow doesn't handle component type %d", componentType);
	}
}
