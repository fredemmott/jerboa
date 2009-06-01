#include "VolumeToolBar_Implementation.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenu>
#include <QSignalMapper>
#include <QSlider>

VolumeToolBar::Implementation::Implementation(
	Jerboa::PlayerInterface* player,
	QWidget* parent
)
: QToolBar(parent)
, m_player(player)
, m_slider(new QSlider(Qt::Horizontal, this))
{
	m_slider->setToolTip(tr("Volume"));
	addWidget(m_slider);
}
