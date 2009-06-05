#pragma once

#include "SeekToolBar.h"

#include "PlayerInterface.h"

#include <QToolBar>

class QSlider;

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
	private:
		Jerboa::PlayerInterface* m_player;
		QSlider* m_slider;
		bool m_dontPropogateChange;
};
