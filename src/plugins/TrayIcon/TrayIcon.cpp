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
#include "TrayIcon.h"

#include "config.h"

#include <QtDebug>
#include <QtPlugin>
#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QWheelEvent>
#include <QIcon>
#include <QMenu>
#include <QWidget>

TrayIcon::TrayIcon()
	:
		QObject(),
		m_menu(0),
		m_player(0),
		m_playlist(0),
		m_trayIcon(new QSystemTrayIcon(this))
{
	Q_INIT_RESOURCE(TrayIcon);
	connect(
		m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(activated(QSystemTrayIcon::ActivationReason))
	);
	connect(QApplication::instance(), SIGNAL(aboutToQuit()), m_trayIcon, SLOT(deleteLater()));
	m_trayIcon->installEventFilter(this);
}

void TrayIcon::addComponent(ComponentType type, QObject* component)
{
	if(type == Player || type == PlaylistSource)
	{
		if(type == Player)
		{
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
		}
		else
		{
			Q_ASSERT(type == PlaylistSource);
			m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
		}
		if(m_player && m_playlist)
		{
			load();
		}
	}
}

QString TrayIcon::pluginName() const
{
	return tr("System Tray Icon");
}

QString TrayIcon::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString TrayIcon::uniqueId() const
{
	return "org.jerboaplayer.TrayIcon";
}

void TrayIcon::load()
{
	setParent(m_player);
	m_trayIcon->show();

	m_menu = new QMenu();
	m_playAction = m_menu->addAction(QIcon(":/TrayIcon/play.svgz"), tr("Play"), this, SLOT(play()));
	m_pauseAction = m_menu->addAction(QIcon(":/TrayIcon/pause.svgz"), tr("Pause"), m_player, SLOT(pause()));
	m_stopAction = m_menu->addAction(QIcon(":/TrayIcon/stop.svgz"), tr("Stop"), m_player, SLOT(stop()));
	m_menu->addSeparator();
	m_previousAction = m_menu->addAction(QIcon(":/TrayIcon/previous.svgz"), tr("Previous"), m_player, SLOT(previous()));
	m_nextAction = m_menu->addAction(QIcon(":/TrayIcon/next.svgz"), tr("Next"), m_player, SLOT(next()));
	m_menu->addSeparator();
	QAction* quitAction = m_menu->addAction(QIcon(), tr("Quit"), QCoreApplication::instance(), SLOT(quit()));

	m_trayIcon->setContextMenu(m_menu);

	connect(m_player, SIGNAL(stateChanged(Jerboa::PlayerInterface::State)), this, SLOT(handleStateChange(Jerboa::PlayerInterface::State)));
	connect(m_player, SIGNAL(stateChanged(Jerboa::PlayerInterface::State)), this, SLOT(updateActions()));
	connect(m_playlist, SIGNAL(dataChanged()), this, SLOT(updateActions()));

	playbackStopped();
	updateActions();
}

void TrayIcon::play()
{
	if(m_player->currentTrack().isValid())
	{
		m_player->play();
	}
	else if(m_playlist->nextTrack() != -1)
	{
		m_player->next();
	}
}

void TrayIcon::playbackStarted(const Jerboa::TrackData& track)
{
	m_trayIcon->showMessage(
			track.title(),
			track.album() + "\n" + track.artist()
	);
	m_trayIcon->setIcon(m_playAction->icon());
	m_trackLine = QString("<b>%1</b><br />%2<br />%3").arg(track.title()).arg(track.album()).arg(track.artist());
	m_trayIcon->setToolTip(m_trackLine);
}

void TrayIcon::handleStateChange(Jerboa::PlayerInterface::State state)
{
	switch(state)
	{
		case Jerboa::PlayerInterface::Playing:
			playbackStarted(m_player->currentTrack());
			break;
		case Jerboa::PlayerInterface::Paused:
			playbackPaused();
			break;
		default:
			playbackStopped();
	}
}

void TrayIcon::playbackPaused()
{
	m_trayIcon->setIcon(m_pauseAction->icon());
	m_trayIcon->setToolTip(tr("Paused:<br />%1").arg(m_trackLine));
}

void TrayIcon::playbackStopped()
{
	m_trayIcon->setIcon(m_stopAction->icon());
	m_trayIcon->setToolTip(tr("Stopped"));
}

void TrayIcon::updateActions()
{
	m_playAction->setEnabled(
		m_player->state() == Jerboa::PlayerInterface::Paused
		||
		(
			m_player->state() == Jerboa::PlayerInterface::Stopped
			&&
			(
				m_playlist->currentTrack() != -1
				||
				m_playlist->nextTrack() != -1
			)
		)
	);
	m_pauseAction->setEnabled(m_player->state() == Jerboa::PlayerInterface::Playing);
	m_stopAction->setEnabled(
		m_player->state() == Jerboa::PlayerInterface::Playing
		||
		m_player->state() == Jerboa::PlayerInterface::Paused
	);
	m_nextAction->setEnabled(m_playlist->nextTrack() != -1);
	m_previousAction->setEnabled(m_playlist->currentTrack() > 0);
}

void TrayIcon::activated(QSystemTrayIcon::ActivationReason a)
{
	if ( a == QSystemTrayIcon::Trigger )
	{
#ifdef EMBEDDED_LEFT_BUTTON_CONTEXT_MENU
		mTrayIcon->contextMenu()->popup(mTrayIcon->geometry().bottomLeft());
		return;
#endif
		QMainWindow* mainWindow = 0;
		Q_FOREACH(QWidget* widget, QApplication::topLevelWidgets())
		{
			mainWindow = qobject_cast<QMainWindow*>(widget);
			if(mainWindow)
			{
				break;
			}
		}
		Q_ASSERT(mainWindow);
		if(!mainWindow)
		{
			return;
		}

		const QRect geometry(QPoint(0, 0), mainWindow->size());
		if(
			mainWindow->isActiveWindow()
		)
		{
			mainWindow->hide();
		}
		else
		{
			mainWindow->show();
			mainWindow->activateWindow();
			mainWindow->raise();
		}
	}
}

bool TrayIcon::eventFilter(QObject* obj, QEvent* event )
{
	if ( obj == m_trayIcon && event->type() == QEvent::Wheel )
	{
		QWheelEvent* wheel = dynamic_cast<QWheelEvent*>(event);
		Q_ASSERT(wheel);

		const qreal step = 0.05;

		if ( wheel->delta() > 0 )
			m_player->setVolume( qMin( qreal(1.0), m_player->volume() + step ) );
		else
			m_player->setVolume( qMax( qreal(0.0), m_player->volume() - step ) );

		return true;
	}
	else
	{
		return QObject::eventFilter(obj, event);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_TrayIcon, TrayIcon);
