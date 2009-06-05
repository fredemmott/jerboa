#pragma once

#include "SeekToolBar.h"

#include "PlayerInterface.h"

#include <QToolBar>

class QSlider;
class QTimer;

class SeekToolBar::Implementation : public QToolBar
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlayerInterface* player, QWidget* parent);
	private slots:
		void reload();
		void adaptToState(Jerboa::PlayerInterface::State);
		void changePosition(int newPosition);
		void moveSlider(quint64 trackPosition);
		void updatePosition();
	private:
		Jerboa::PlayerInterface* m_player;
		QSlider* m_slider;
		QTimer* m_timer;
};
