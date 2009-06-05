#include "VolumeToolBar_Implementation.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QSlider>
#include <QStyle>

VolumeToolBar::Implementation::Implementation(
	Jerboa::PlayerInterface* player,
	QWidget* parent
)
: QToolBar(parent)
, m_player(player)
, m_slider(new QSlider(Qt::Horizontal, this))
{
	m_slider->setToolTip(tr("Volume"));
	m_slider->setRange(0, 100);
	volumeChanged(player->volume());

	QLabel* label = new QLabel(this);
	QIcon icon = style()->standardIcon(QStyle::SP_MediaVolume);
	label->setPixmap(icon.pixmap(icon.availableSizes().first()));
	addWidget(label);
	addWidget(m_slider);

	connect(
		m_slider,
		SIGNAL(valueChanged(int)),
		SLOT(changeVolume(int))
	);

	connect(
		m_player,
		SIGNAL(volumeChanged(qreal)),
		SLOT(volumeChanged(qreal))
	);
}

void VolumeToolBar::Implementation::changeVolume(int value)
{
	m_player->setVolume(value / 100.0);
}

void VolumeToolBar::Implementation::volumeChanged(qreal value)
{
	m_slider->setValue(qRound(value * 100));
}
