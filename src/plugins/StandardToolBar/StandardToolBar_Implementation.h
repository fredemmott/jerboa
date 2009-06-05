#ifndef _STANDARD_TOOL_BAR_IMPLEMENTATION_H
#define _STANDARD_TOOL_BAR_IMPLEMENTATION_H

#include "StandardToolBar.h"

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "ToolBar.h"

#include <QToolBar>

class QActionGroup;
class QSignalMapper;

class StandardToolBar::Implementation : public Jerboa::ToolBar
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlayerInterface* player, Jerboa::PlaylistInterface* playlist, QWidget* parent);
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
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;

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
