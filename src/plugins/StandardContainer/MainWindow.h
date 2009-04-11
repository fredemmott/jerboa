#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QList>
#include <QMainWindow>

class QActionGroup;
class QSignalMapper;
class QSplitter;
class QToolBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
	public:
		MainWindow(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QWidget* parent);
		void addComponent(Jerboa::Plugin::ComponentType componentType, QWidget* component);
	private slots:
		void updateActionStates();
		void popupSenderMenu();
		void play();
		void setShuffleMode(int mode);
		void setLoopMode(int mode);

		// update UI if changed by another plugin, eg MPRIS
		void updateShuffleMode(Jerboa::PlaylistInterface::ShuffleMode mode);
		void updateLoopMode(Jerboa::PlaylistInterface::LoopMode mode);
	private:
		static QIcon amarokIcon(const QString& name);
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

		QActionGroup* m_shuffleActions;
		QSignalMapper* m_shuffleMapper;
		QAction* m_shuffleMenuAction;
		QAction* m_shuffleNoneAction;
		QAction* m_shuffleTracksAction;
		QAction* m_shuffleAlbumsAction;

		QActionGroup* m_loopActions;
		QSignalMapper* m_loopMapper;
		QAction* m_loopMenuAction;
		QAction* m_loopNoneAction;
		QAction* m_loopTrackAction;
		QAction* m_loopAlbumAction;
		QAction* m_loopPlaylistAction;

		QAction* m_clearPlaylistAction;
};

#endif
