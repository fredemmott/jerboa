#include "StandardToolBar_Implementation.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenu>
#include <QSignalMapper>

StandardToolBar::Implementation::Implementation(
	Jerboa::PlayerInterface* player,
	Jerboa::PlaylistInterface* playlist,
	QWidget* parent
)
: QToolBar(parent)
, m_player(player)
, m_playlist(playlist)
{
	connect(
		m_player,
		SIGNAL(stateChanged(Jerboa::PlayerInterface::State)),
		this,
		SLOT(updateActionStates())
	);
	connect(
		m_playlist,
		SIGNAL(dataChanged()),
		this,
		SLOT(updateActionStates())
	);
	connect(
		m_playlist,
		SIGNAL(loopModeChanged(Jerboa::PlaylistInterface::LoopMode)),
		this,
		SLOT(updateLoopMode(Jerboa::PlaylistInterface::LoopMode))
	);
	connect(
		m_playlist,
		SIGNAL(shuffleModeChanged(Jerboa::PlaylistInterface::ShuffleMode)),
		this,
		SLOT(updateShuffleMode(Jerboa::PlaylistInterface::ShuffleMode))
	);

	// Playback controls
	m_previousAction = addAction(QIcon(":/StandardToolBar/previous.svgz"), tr("Previous"), m_player, SLOT(previous()));

	m_playAction = addAction(QIcon(":/StandardToolBar/play.svgz"), tr("Play"), this, SLOT(play()));
	m_pauseAction = addAction(QIcon(":/StandardToolBar/pause.svgz"), tr("Pause"), m_player, SLOT(pause()));
	m_stopAction = addAction(QIcon(":/StandardToolBar/stop.svgz"), tr("Stop"), m_player, SLOT(stop()));

	m_nextAction = addAction(QIcon(":/StandardToolBar/next.svgz"), tr("Next"), m_player, SLOT(next()));

	// Shuffle controls
	m_shuffleMapper = new QSignalMapper(this);
	m_shuffleActions = new QActionGroup(this);
	QMenu* shuffleMenu = new QMenu(this);

	connect(
		m_shuffleMapper,
		SIGNAL(mapped(int)),
		this,
		SLOT(setShuffleMode(int))
	);

	const QIcon shuffleIcon(amarokIcon("shuffle"));

	m_shuffleNoneAction = m_shuffleActions->addAction(amarokIcon("no-shuffle"), tr("No Shuffle"));
	m_shuffleTracksAction = m_shuffleActions->addAction(shuffleIcon, tr("Shuffle Tracks"));
	m_shuffleAlbumsAction = m_shuffleActions->addAction(shuffleIcon, tr("Shuffle Albums"));

	m_shuffleMapper->setMapping(m_shuffleNoneAction, Jerboa::PlaylistInterface::ShuffleNone);
	m_shuffleMapper->setMapping(m_shuffleTracksAction, Jerboa::PlaylistInterface::ShuffleTracks);
	m_shuffleMapper->setMapping(m_shuffleAlbumsAction, Jerboa::PlaylistInterface::ShuffleAlbums);

	Q_FOREACH(QAction* action, m_shuffleActions->actions())
	{
		action->setCheckable(true);
		shuffleMenu->addAction(action);
		connect(
			action,
			SIGNAL(triggered()),
			m_shuffleMapper,
			SLOT(map())
		);
	}

	m_shuffleNoneAction->setChecked(true);

	m_shuffleMenuAction = addAction(m_shuffleNoneAction->icon(), m_shuffleNoneAction->text(), this, SLOT(popupSenderMenu()));
	m_shuffleMenuAction->setMenu(shuffleMenu);

	// Loop controls
	m_loopMapper = new QSignalMapper(this);
	m_loopActions = new QActionGroup(this);
	QMenu* loopMenu = new QMenu(this);

	connect(
		m_loopMapper,
		SIGNAL(mapped(int)),
		this,
		SLOT(setLoopMode(int))
	);

	m_loopNoneAction = m_loopActions->addAction(amarokIcon("no-repeat"), tr("No Loop"));
	m_loopTrackAction = m_loopActions->addAction(amarokIcon("repeat-track"), tr("Loop Track"));
	m_loopAlbumAction = m_loopActions->addAction(amarokIcon("repeat-album"), tr("Loop Album"));
	m_loopPlaylistAction = m_loopActions->addAction(amarokIcon("repeat-playlist"), tr("Loop Playlist"));
	
	m_loopMapper->setMapping(m_loopNoneAction, Jerboa::PlaylistInterface::LoopNone);
	m_loopMapper->setMapping(m_loopTrackAction, Jerboa::PlaylistInterface::LoopTrack);
	m_loopMapper->setMapping(m_loopAlbumAction, Jerboa::PlaylistInterface::LoopAlbum);
	m_loopMapper->setMapping(m_loopPlaylistAction, Jerboa::PlaylistInterface::LoopPlaylist);

	Q_FOREACH(QAction* action, m_loopActions->actions())
	{
		action->setCheckable(true);
		loopMenu->addAction(action);
		connect(
			action,
			SIGNAL(triggered()),
			m_loopMapper,
			SLOT(map())
		);
	}

	m_loopNoneAction->setChecked(true);

	m_loopMenuAction = addAction(m_loopNoneAction->icon(), m_loopNoneAction->text(), this, SLOT(popupSenderMenu()));
	m_loopMenuAction->setMenu(loopMenu);

	// Clear playlist
	m_clearPlaylistAction = addAction(QIcon(":/StandardToolBar/clear-playlist.svgz"), tr("Clear Playlist"), m_playlist, SLOT(clear()));

	// Enable/disable controls as appropriate
	updateActionStates();
}

QIcon StandardToolBar::Implementation::amarokIcon(const QString& name)
{
	QIcon icon;
	const QStringList sizes(
		QStringList()
			<< "16x16"
			<< "22x22"
	);

	Q_FOREACH(const QString& size, sizes)
	{
		icon.addFile(QString(":/StandardToolBar/%1/%2.png").arg(size).arg(name));
	}
	return icon;
}

void StandardToolBar::Implementation::setShuffleMode(int mode)
{
	const Jerboa::PlaylistInterface::ShuffleMode shuffleMode(static_cast<Jerboa::PlaylistInterface::ShuffleMode>(mode));
	m_playlist->setShuffleMode(shuffleMode);
	updateShuffleMode(shuffleMode);
}

void StandardToolBar::Implementation::updateShuffleMode(Jerboa::PlaylistInterface::ShuffleMode mode)
{
	QAction* action = qobject_cast<QAction*>(m_shuffleMapper->mapping(mode));
	Q_ASSERT(action);
	action->setChecked(true);
	m_shuffleMenuAction->setIcon(action->icon());
	m_shuffleMenuAction->setText(action->text());
}

void StandardToolBar::Implementation::setLoopMode(int mode)
{
	const Jerboa::PlaylistInterface::LoopMode loopMode(static_cast<Jerboa::PlaylistInterface::LoopMode>(mode));
	m_playlist->setLoopMode(loopMode);
	updateLoopMode(loopMode);
}

void StandardToolBar::Implementation::updateLoopMode(Jerboa::PlaylistInterface::LoopMode mode)
{
	QAction* action = qobject_cast<QAction*>(m_loopMapper->mapping(mode));
	Q_ASSERT(action);
	action->setChecked(true);
	m_loopMenuAction->setIcon(action->icon());
	m_loopMenuAction->setText(action->text());
}

void StandardToolBar::Implementation::popupSenderMenu()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if(!action)
	{
		qFatal("Asked to popup a menu by an object which isn't a QAction.");
	}
	Q_ASSERT(action->menu());
	if(action->menu())
	{
		const QWidget* widget = widgetForAction(action);
		Q_ASSERT(widget);
		if(widget)
		{
			action->menu()->popup(mapToGlobal(widget->geometry().bottomLeft()));
		}
	}
}

void StandardToolBar::Implementation::play()
{
	if(
		(!m_player->currentTrack().isValid())
		&&
		(m_playlist->nextTrack() != -1)
	)
	{
		m_player->skipTo(m_playlist->nextTrack());
	}
	m_player->play();
}

void StandardToolBar::Implementation::updateActionStates()
{
	const Jerboa::PlayerInterface::State state = m_player->state();
	const bool havePrevious = m_playlist->currentTrack() > 0;
	const bool haveCurrent = m_playlist->currentTrack() != -1;
	const bool haveNext = m_playlist->nextTrack() != -1;

	m_previousAction->setEnabled(havePrevious);

	m_playAction->setEnabled(
		(state == Jerboa::PlayerInterface::Paused)
		||
		(
			(state == Jerboa::PlayerInterface::Stopped)
			&&
			(haveCurrent || haveNext)
		)
	);
	m_pauseAction->setEnabled(state == Jerboa::PlayerInterface::Playing);
	m_stopAction->setEnabled(state != Jerboa::PlayerInterface::Stopped);
	m_nextAction->setEnabled(haveNext);

	m_clearPlaylistAction->setEnabled(!m_playlist->tracks().isEmpty());
}

