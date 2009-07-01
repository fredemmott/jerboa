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
#pragma once

#include <QHash>
#include <QObject>
#include <QPoint>

class QSplitter;

/** Class that makes splitters draggable a few pixels either edge of it.
 *
 * Only handles horizontal splitters.
 *
 * @author Fred Emmott <fred.emmott@mendeley.com>
 */
class SplitterHandleFuzzer : public QObject
{
	Q_OBJECT;
	public:
		SplitterHandleFuzzer(QSplitter* parent);
		bool eventFilter(QObject* watched, QEvent* event);
	private:
		void populateChildren(QWidget* parent, QWidget* topLevel);
		int handle(const QPoint& globalPosition, QWidget* widget) const;
		bool hitsHandle(const QPoint& globalPosition, QWidget* widget) const;
		const int m_fuzz;
		int m_index;
		bool m_overridingCursor;
		bool m_draggingSplitter;
		int m_diff;
		
		QSplitter* m_splitter;
		QHash<QWidget*, QWidget*> m_childrenToTopLevel;
};
