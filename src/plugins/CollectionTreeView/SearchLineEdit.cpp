/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SearchLineEdit.h"
// Copyright 2008, 2009 Frederick Emmott <mail@fredemmott.co.uk>
// Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>

#include <QDebug>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOptionFrameV2>

SearchLineEdit::SearchLineEdit(QWidget* parent) : QLineEdit(parent)
{
	m_leftPadding = ((height() / 5) * 2) + 3;
	int left;
	int top;
	int right;
	int bottom;
	getTextMargins(&left, &top, &right, &bottom);
	left += (height() / 2);
	right += (height() * 2/ 3 ) - 2;
	setTextMargins(left, top, right, bottom);
}

void SearchLineEdit::mouseMoveEvent(QMouseEvent* event)
{
	QLineEdit::mouseMoveEvent(event);
	if(event->x() < m_leftPadding || (m_clearButton.contains(event->posF()) && !text().isEmpty()))
	{
		setCursor(Qt::ArrowCursor);
	}
	else
	{
		setCursor(Qt::IBeamCursor);
	}
}

void SearchLineEdit::paintEvent(QPaintEvent *event)
{

	QLineEdit::paintEvent(event);

	// --- start magnifying glass code from Arora ---
	QPainterPath myPath;

	int radius = (height() / 6) * 2;
	QRect circle(height() / 3 - 1, height() / 4 + 1, radius, radius);
	myPath.addEllipse(circle);

	myPath.arcMoveTo(circle, 300);
	QPointF c = myPath.currentPosition();
	int diff = height() / 8;
	qreal rightEdge = qMin(width() - 2, (int)c.x() + diff);
	myPath.lineTo(rightEdge, c.y() + diff);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::darkGray, 2));
	painter.drawPath(myPath);
	// --- end magnifying glass code from Arora ---

	// Start clear button code
	if(!text().isEmpty())
	{
		int vPadding = height() / 3 - 1;
		int clearDiameter = height() - (2*vPadding);
		m_clearButton = QRectF(
			width() - (clearDiameter + vPadding),
			vPadding,
			clearDiameter,
			clearDiameter
		);
		painter.setBrush(QBrush(Qt::gray));
		painter.setPen(QPen(Qt::gray, 2));
		painter.drawEllipse(m_clearButton);
		QPen xPen(Qt::white, 2);
		xPen.setCapStyle(Qt::RoundCap);
		painter.setPen(xPen);
		int offset(3);
		painter.drawLine(
			m_clearButton.topLeft() + QPoint(offset, offset),
			m_clearButton.bottomRight() - QPoint(offset, offset)
		);
		painter.drawLine(
			m_clearButton.topRight() + QPoint(-offset, offset),
			m_clearButton.bottomLeft() + QPoint(offset, -offset)
		);
	}
	else if(!hasFocus())
	{
		painter.setPen(QPen(Qt::darkGray, 1));
		QFontMetrics metrics(font());
		int left, top, right, bottom;
		getTextMargins(&left, &top, &right, &bottom);
		QStyleOptionFrameV2 panel;
		initStyleOption(&panel);
		QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

		painter.drawText(
			rightEdge + 5, r.top() + top + metrics.height() - metrics.descent(), tr("Search")
		);
	}
	painter.end();
}

void SearchLineEdit::mouseReleaseEvent(QMouseEvent* event)
{
	QLineEdit::mouseReleaseEvent(event);
	if(m_clearButton.contains(event->posF()))
	{
		setText(QString());
		emit textEdited(text()); // I count clicking the clear button as user-edited.
	}
}
