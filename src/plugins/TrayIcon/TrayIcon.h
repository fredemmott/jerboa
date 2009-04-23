/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _TRAY_ICON_H
#define _TRAY_ICON_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

#include <QSystemTrayIcon>

class QMenu;
class QAction;

class TrayIcon : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		TrayIcon();
		void load();

		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	protected:
		bool eventFilter(QObject* obj, QEvent* event);
	private slots:
		void activated(QSystemTrayIcon::ActivationReason);
		void handleStateChange(Jerboa::PlayerInterface::State);
		void updateActions();
		void play();
	private:
		void playbackStarted(const Jerboa::TrackData& track);
		void playbackPaused();
		void playbackStopped();

		QWidget* mainWindow();

		QMenu* m_menu;
		QAction* m_playAction;
		QAction* m_pauseAction;
		QAction* m_stopAction;
		QAction* m_nextAction;
		QAction* m_previousAction;

		QString m_trackLine;
		Jerboa::PlayerInterface* m_player;
		Jerboa::PlaylistInterface* m_playlist;
		QSystemTrayIcon* m_trayIcon;

		QWidget* m_mainWindow;
};

#endif
