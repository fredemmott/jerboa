#include "SeekToolBar_Implementation.h"

#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QSlider>

SeekToolBar::Implementation::Implementation(
	Jerboa::PlayerInterface* player,
	QWidget* parent
)
: Jerboa::ToolBar(parent)
, m_player(player)
, m_slider(new QSlider(Qt::Horizontal, this))
, m_dontPropogateChange(false)
{
	setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
	setWindowTitle(tr("Track Position"));

	QLabel* label = new QLabel(this);
	label->setPixmap(QPixmap(":/SeekToolBar/icon.png"));
	addWidget(label);

	m_slider->setSingleStep(1000);
	m_slider->setPageStep(10000);
	m_slider->setToolTip(tr("Position"));
	addWidget(m_slider);

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

	connect(
		player,
		SIGNAL(stateChanged(Jerboa::PlayerInterface::State)),
		SLOT(adaptToState(Jerboa::PlayerInterface::State))
	);
}

void SeekToolBar::Implementation::reload()
{
	m_slider->setValue(m_player->position());
	m_slider->setMaximum(m_player->trackLength());
	m_slider->setDisabled(m_slider->maximum() == 0);
}

Qt::ToolBarArea SeekToolBar::Implementation::initialArea() const
{
	return Qt::BottomToolBarArea;
}

void SeekToolBar::Implementation::adaptToState(Jerboa::PlayerInterface::State state)
{
	if(state == Jerboa::PlayerInterface::Playing)
	{
		reload();
	}
}

void SeekToolBar::Implementation::changePosition(int newPosition)
{
	if(!m_dontPropogateChange)
	{
		m_dontPropogateChange = true;
		m_player->setPosition(newPosition);
		m_dontPropogateChange = false;
	}
}

void SeekToolBar::Implementation::moveSlider(quint64 trackPosition)
{
	if(!m_dontPropogateChange)
	{
		m_dontPropogateChange = true;
		m_slider->setValue(trackPosition);
		m_dontPropogateChange = false;
	}
}
