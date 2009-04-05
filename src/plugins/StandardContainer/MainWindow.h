#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QMainWindow>

class QSplitter;
class QTabWidget;
class QToolBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QWidget* parent);
		void addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component);
	private slots:
		void handlePlayerState(Jerboa::PlayerInterface::State);
		void play();
		void pause();
		void stop();
	private:
		void setupToolBar();

		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;

		QToolBar* m_toolBar;
		QSplitter* m_splitter;
		QTabWidget* m_leftTabs;
		QTabWidget* m_rightTabs;

		QAction* m_playAction;
		QAction* m_pauseAction;
		QAction* m_stopAction;
		
		QAction* m_previousAction;
		QAction* m_nextAction;
};

#endif
