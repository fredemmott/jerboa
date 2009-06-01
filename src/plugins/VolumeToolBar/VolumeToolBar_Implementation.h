#pragma once

#include "VolumeToolBar.h"

#include "PlayerInterface.h"

#include <QToolBar>

class QSlider;

class VolumeToolBar::Implementation : public QToolBar
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlayerInterface* player, QWidget* parent);
	private:
		Jerboa::PlayerInterface* m_player;
		QSlider* m_slider;
};
