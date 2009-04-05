#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include "PlayerInterface.h"
#include "Plugin.h"

#include <QMainWindow>

class QSplitter;
class QTabWidget;
class QToolBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow(Jerboa::PlayerInterface* player, QWidget* parent);
		void addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component);
	private:
		Jerboa::PlayerInterface* m_player;
		QToolBar* m_toolBar;
		QSplitter* m_splitter;
		QTabWidget* m_leftTabs;
		QTabWidget* m_rightTabs;
};

#endif
