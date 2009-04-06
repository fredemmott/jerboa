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

	m_splitter = 0;

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
			component->setWindowTitle(tr("Collection"));
			m_leftWidgets.prepend(component);
			break;
		case Jerboa::Plugin::PlaylistView:
			component->setWindowTitle(tr("Playlist"));
			m_rightWidgets.prepend(component);
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
