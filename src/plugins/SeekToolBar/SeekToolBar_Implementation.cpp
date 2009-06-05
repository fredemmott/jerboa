#include "SeekToolBar_Implementation.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenu>
#include <QSignalMapper>
#include <QSlider>
#include <QTimer>

SeekToolBar::Implementation::Implementation(
	Jerboa::PlayerInterface* player,
	QWidget* parent
)
: QToolBar(parent)
, m_player(player)
, m_slider(new QSlider(Qt::Horizontal, this))
, m_timer(new QTimer(this))
{
	m_slider->setSingleStep(1000);
	m_slider->setPageStep(10000);
	m_slider->setToolTip(tr("Position"));
	addWidget(m_slider);

	m_timer->setInterval(1000);

	connect(
		m_timer,
		SIGNAL(timeout()),
		SLOT(updatePosition())
	);

	connect(
		player,
		SIGNAL(positionChanged(quint64)),
		SLOT(moveSlider(quint64))
	);

	connect(
		player,
		SIGNAL(currentTrackChanged(Jerboa::TrackData)),
		SLOT(reload())
	);

	connect(
		m_slider,
		SIGNAL(valueChanged(int)),
		SLOT(changePosition(int))
	);
}

void SeekToolBar::Implementation::reload()
{
	m_timer->stop();
	m_slider->setValue(m_player->position());
	m_slider->setMaximum(m_player->trackLength());
	m_slider->setDisabled(m_slider->maximum() == 0);
}

void SeekToolBar::Implementation::adaptToState(Jerboa::PlayerInterface::State state)
{
	if(state == Jerboa::PlayerInterface::Loading)
	{
		reload();
		m_timer->start();
	}
	else
	{
		m_timer->stop();
	}
}

void SeekToolBar::Implementation::changePosition(int newPosition)
{
	m_player->setPosition(newPosition);
}

void SeekToolBar::Implementation::moveSlider(quint64 trackPosition)
{
	m_slider->setValue(trackPosition);
}

void SeekToolBar::Implementation::updatePosition()
{
	moveSlider(m_player->position());
}
