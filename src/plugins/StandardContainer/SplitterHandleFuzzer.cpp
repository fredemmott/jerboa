/* LICENSE NOTICE
	SearchHandleFuzzer: Make splitters have an abnormal drag area.
	Copyright (C) 2009 Mendeley Ltd.  <fred.emmott@mendeley.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SplitterHandleFuzzer.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QSplitter>
#include <QSplitterHandle>

SplitterHandleFuzzer::SplitterHandleFuzzer(QSplitter* parent)
: QObject(parent)
, m_fuzz(3)
, m_index(-1)
, m_overridingCursor(false)
, m_draggingSplitter(false)
, m_splitter(parent)
{
	m_splitter->setMouseTracking(true);
	for(int i = 0; i < m_splitter->count(); ++i)
	{
		QWidget* widget = m_splitter->widget(i);
		populateChildren(widget, widget);
	}
}

void SplitterHandleFuzzer::populateChildren(QWidget* parent, QWidget* topLevel)
{
	parent->installEventFilter(this);
	parent->setMouseTracking(true);
	m_childrenToTopLevel.insert(parent, topLevel);

	Q_FOREACH(QObject* object, parent->children())
	{
		QWidget* widget = qobject_cast<QWidget*>(object);
		if(widget)
		{
			populateChildren(widget, topLevel);
		}
	}
}

bool SplitterHandleFuzzer::hitsHandle(const QPoint& globalPosition, QWidget* widget) const
{
	return handle(globalPosition, widget) >= 0;
}

int SplitterHandleFuzzer::handle(const QPoint& globalPosition, QWidget* widget) const
{
	QWidget* topLevel = m_childrenToTopLevel.value(widget);
	Q_ASSERT(topLevel);
	const QPoint position = topLevel->mapFromGlobal(globalPosition);

	const int index = m_splitter->indexOf(topLevel);

	const bool hitsSplitterOnLeft = (position.x() + 1 <= m_fuzz && index != 0);
	const bool hitsSplitterOnRight = (position.x() >= topLevel->width() - (m_fuzz + 1) && index != m_splitter->count() - 1);

	if(hitsSplitterOnLeft)
	{
		return index - 1;
	}
	if(hitsSplitterOnRight)
	{
		return index;
	}
	return -1;
}

bool SplitterHandleFuzzer::eventFilter(QObject* watched, QEvent* event)
{
	if(event->type() == QEvent::MouseMove && !m_draggingSplitter)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		QWidget* widget = qobject_cast<QWidget*>(watched);
		Q_ASSERT(widget);

		if(hitsHandle(mouseEvent->globalPos(), widget))
		{
			if(!m_overridingCursor)
			{
				m_overridingCursor = true;
				QApplication::setOverrideCursor(Qt::SplitHCursor);
			}
		}
		else if(m_overridingCursor)
		{
			m_overridingCursor = false;
			QApplication::restoreOverrideCursor();
		}
	}
	else if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		QWidget* widget = qobject_cast<QWidget*>(watched);
		Q_ASSERT(widget);

		if(hitsHandle(mouseEvent->globalPos(), widget) || m_draggingSplitter)
		{
			const int position = m_splitter->mapFromGlobal(mouseEvent->globalPos()).x();

			switch(event->type())
			{
				case QEvent::MouseButtonPress:
					m_draggingSplitter = true;
					m_index = handle(mouseEvent->globalPos(), widget);
					m_diff = m_splitter->sizes().at(m_index) - position;
					break;
				case QEvent::MouseButtonRelease:
					m_draggingSplitter = false;
					break;
				default:
					break;
			}


			QSplitterHandle* handle = m_splitter->handle(m_index + 1);
			QApplication::postEvent(
				handle,
				new QMouseEvent(
					event->type(),
					QPoint(),
					mouseEvent->globalPos() + QPoint(m_diff, 0),
					mouseEvent->button(),
					mouseEvent->buttons(),
					mouseEvent->modifiers()
				)
			);

			return true;
		}
		else if(m_overridingCursor && event->type() == QEvent::MouseButtonRelease)
		{
			m_overridingCursor = false;
			QApplication::restoreOverrideCursor();
		}
	}
	return false;
}
