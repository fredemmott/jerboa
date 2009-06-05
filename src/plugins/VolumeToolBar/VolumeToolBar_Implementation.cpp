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
	QLabel* label = new QLabel(this);
	QIcon icon = style()->standardIcon(QStyle::SP_MediaVolume);
	label->setPixmap(icon.pixmap(icon.availableSizes().first()));
	addWidget(label);
	addWidget(m_slider);
}
