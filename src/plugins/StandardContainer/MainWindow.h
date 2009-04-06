#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QList>
#include <QMainWindow>

class QSplitter;
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
		void setupTabs();
		void setupToolBar();

		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;

		QList<QWidget*> m_leftWidgets;
		QList<QWidget*> m_rightWidgets;

		QToolBar* m_toolBar;
		QSplitter* m_splitter;

		QAction* m_playAction;
		QAction* m_pauseAction;
		QAction* m_stopAction;
		
		QAction* m_previousAction;
		QAction* m_nextAction;
};

#endif
