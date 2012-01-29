#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include "JerboaPlugin.h"

#include <QList>
#include <QMainWindow>

class QSplitter;
class QToolBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow(QWidget* parent);
		void addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component);
	protected:
		void showEvent(QShowEvent* event);
		void closeEvent(QCloseEvent* event);
	private slots:
		void saveSettings();
	private:
		static const int m_stateVersion;
		void setupTabs();

		QList<QWidget*> m_leftWidgets;
		QList<QWidget*> m_rightWidgets;

		QSplitter* m_splitter;

};

#endif
