#include "MainWindow.h"

#include "ToolBar.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>

const int MainWindow::m_stateVersion(2);

MainWindow::MainWindow(
	QWidget* parent
)
: QMainWindow(parent)
{

	setUnifiedTitleAndToolBarOnMac(true);
	QSettings settings;
	settings.beginGroup("StandardContainerPlugin");
	restoreGeometry(settings.value("geometry").toByteArray());

	m_splitter = 0;

	connect(
		qApp,
		SIGNAL(aboutToQuit()),
		SLOT(saveSettings())
	);
}

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.beginGroup("StandardContainerPlugin");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState(m_stateVersion));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
	saveSettings();
}

void MainWindow::showEvent(QShowEvent* event)
{
	QSettings settings;
	settings.beginGroup("StandardContainerPlugin");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray(), m_stateVersion);
	QMainWindow::showEvent(event);
	restoreGeometry(settings.value("geometry").toByteArray());

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
#ifdef Q_WS_MAC
		leftTabs->setDocumentMode(true);
		leftTabs->setProperty("SCOPE_BAR", true);
#else
		leftTabs->setTabPosition(QTabWidget::West);
#endif

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
#ifdef Q_WS_MAC
		rightTabs->setDocumentMode(true);
		rightTabs->setProperty("SCOPE_BAR", true);
#else
		rightTabs->setTabPosition(QTabWidget::West);
#endif

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
