#include "MainWindow.h"

#include <QAction>
#include <QDebug>
#include <QIcon>
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
	m_previousAction = m_toolBar->addAction(QIcon(":/StandardContainer/previous.svgz"), tr("Previous"), m_player, SLOT(previous()));

	m_playAction = m_toolBar->addAction(QIcon(":/StandardContainer/play.svgz"), tr("Play"), m_player, SLOT(play()));
	m_pauseAction = m_toolBar->addAction(QIcon(":/StandardContainer/pause.svgz"), tr("Pause"), m_player, SLOT(pause()));
	m_stopAction = m_toolBar->addAction(QIcon(":/StandardContainer/stop.svgz"), tr("Stop"), m_player, SLOT(stop()));

	m_nextAction = m_toolBar->addAction(QIcon(":/StandardContainer/next.svgz"), tr("Next"), m_player, SLOT(next()));

	m_pauseAction->setEnabled(false);
	m_stopAction->setEnabled(false);
}

void MainWindow::handlePlayerState(Jerboa::PlayerInterface::State state)
{
	m_playAction->setEnabled(state == Jerboa::PlayerInterface::Paused || state == Jerboa::PlayerInterface::Stopped);
	m_pauseAction->setEnabled(state == Jerboa::PlayerInterface::Playing);
	m_stopAction->setEnabled(state != Jerboa::PlayerInterface::Stopped);
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
			qDebug() << "Adding tab" << widget->windowTitle();
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
