/* LICENSE NOTICE
	MacStyle: Various tweaks to QMacStyle to get something looking much more native.
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
#include "MacStyle.h"

#include <QtGlobal>

#include <QMacStyle>

class MacStyle::Implementation : public QMacStyle
{
	Q_OBJECT;
	public:
		Implementation();
		virtual QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const;
		virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
		virtual void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
		virtual int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;
		virtual QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget = 0) const;
	private:
		static QFont systemFont();
		static bool isSpecialTabWidget(const QStyleOption* option, const QWidget* widget);

		void drawScopeBar(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
		void drawTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
		void drawTabBarTabLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
		void drawVerticalSplitter(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
		void drawHorizontalSplitter(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
		void drawHeaderSection(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

		static void drawTableHeader(const QRect& outerBounds, bool drawTopBorder, bool drawLeftBorder, bool drawRightBorder, const QString& pixmapKey, QPainter* painter);
};
