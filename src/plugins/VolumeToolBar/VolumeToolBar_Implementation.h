#pragma once

#include "VolumeToolBar.h"

#include "PlayerInterface.h"
#include "ToolBar.h"

class QSlider;

class VolumeToolBar::Implementation : public Jerboa::ToolBar
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlayerInterface* player, QWidget* parent);
		Qt::ToolBarArea initialArea() const;
	private slots:
		void changeVolume(int value);
		void volumeChanged(qreal value);
	private:
		Jerboa::PlayerInterface* m_player;
		QSlider* m_slider;
		bool m_dontPropogateChange;
};
