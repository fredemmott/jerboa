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
#include "MacStyle_Implementation.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QLinearGradient>
#include <QPainter>
#include <QPixmapCache>
#include <QStyleOption>
#include <QTreeView>
#include <QWidget>

#define Q_DEBUG qDebug
#undef qDebug
#include <Carbon/Carbon.h>
#define qDebug Q_DEBUG
	
QRect MacStyle::Implementation::subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const
{
	if(element == QStyle::SE_ShapedFrameContents && qobject_cast<const QTreeView*>(widget))
	{
		return widget->rect();
	}
	return QMacStyle::subElementRect(element, option, widget);
}

MacStyle::Implementation::Implementation() : QMacStyle()
{
}

int MacStyle::Implementation::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
	const int upstream(QMacStyle::pixelMetric(metric, option, widget));
	switch(metric)
	{
		case PM_SplitterWidth:
			return 1;
		case PM_TabBarBaseOverlap:
			if(isSpecialTabWidget(option, widget))
			{
				return widget->height();
			}
		case PM_LayoutLeftMargin:
		case PM_LayoutTopMargin:
		case PM_LayoutRightMargin:
		case PM_LayoutBottomMargin:
			if(widget && widget->isWindow())
			{
				return QMacStyle::pixelMetric(metric); // ignroe the widget
			}
			else
			{
				return upstream;
			}
		default:
			return upstream;
	}
}

void MacStyle::Implementation::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	switch(element)
	{
		case PE_FrameTabWidget:
			if(!isSpecialTabWidget(option, widget))
			{
				break;
			}
		case PE_FrameStatusBarItem:
			// No frames around these, please
			return;
		case PE_FrameTabBarBase:
			if(isSpecialTabWidget(option, widget))
			{
				drawScopeBar(option, painter, widget);
				return;
			}
		default:
			break;
	}
	QMacStyle::drawPrimitive(element, option, painter, widget);
}

bool MacStyle::Implementation::isSpecialTabWidget(const QStyleOption* option, const QWidget* widget)
{
	if(!widget)
	{
		return false;
	}
	const QTabWidget* tabWidget = qobject_cast<const QTabWidget*>(widget);
	const QTabBar* tabBar = qobject_cast<const QTabBar*>(widget);
	return (tabWidget || tabBar);
}

void MacStyle::Implementation::drawScopeBar(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	Q_UNUSED(widget);
	QColor bottomColor(177, 177, 177);
	QColor gradientStart(233, 233, 233);
	QColor gradientStop(208, 208, 208);

	QLinearGradient gradient;
	gradient.setColorAt(0, gradientStart);
	gradient.setColorAt(1, gradientStop);
	gradient.setStart(0, 0);
	gradient.setFinalStop(0, option->rect.height() - 1);

	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(gradient);
	painter->drawRect(option->rect);
	painter->setPen(QPen(bottomColor));
	const QPointF leftPoint(0, static_cast<qreal>(option->rect.height()) - 0.5);
	const QPointF rightPoint(option->rect.width(), static_cast<qreal>(option->rect.height()) - 0.5);
	painter->drawLine(leftPoint, rightPoint);
	painter->restore();
}

void MacStyle::Implementation::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	if(element == CE_FocusFrame)
	{
		return;
	}
	if(element == CE_Splitter)
	{
		// Horizontal QSplitters have vertical splitter handles
		if(option->state.testFlag(QStyle::State_Horizontal))
		{
			drawVerticalSplitter(option, painter, widget);
			return;
		}
		else
		{
			drawHorizontalSplitter(option, painter, widget);
			return;
		}
	}
	if(element == CE_HeaderSection)
	{
		drawHeaderSection(option, painter, widget);
		return;
	}
	if(element == CE_TabBarTabLabel && isSpecialTabWidget(option, widget))
	{
		drawTabBarTabLabel(option, painter, widget);
		return;
	}
	if(element == CE_TabBarTabShape && isSpecialTabWidget(option, widget))
	{
		drawTabBarTabShape(option, painter, widget);
		return;
	}
	QMacStyle::drawControl(element, option, painter, widget);
}

void MacStyle::Implementation::drawHeaderSection(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	if(const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader*>(option))
	{
		int value = 0;
		int state = kThemeStateActive;
		int adornment = kThemeAdornmentNone;
		QRect ir = header->rect;
		State flags = header->state;
		if(flags & State_On)
		{
			value = kThemeButtonOn;
		}
		else
		{
			value = kThemeButtonOff;
		}
		if(header->position == QStyleOptionHeader::End)
		{
			ir.adjust(-1, 0, 0, 0);
		}
                if (header->position != QStyleOptionHeader::Beginning
                    && header->position != QStyleOptionHeader::OnlyOneSection)
		{
			adornment =
				header->direction == Qt::LeftToRight
				? kThemeAdornmentHeaderButtonLeftNeighborSelected
				: kThemeAdornmentHeaderButtonRightNeighborSelected;
                }
		if (flags & State_Active)
		{
			if (!(flags & State_Enabled))
			{
				state = kThemeStateUnavailable;
			}
			else if (flags & State_Sunken)
			{
				state = kThemeStatePressed;
			}
		}
		else
		{
			if (flags & State_Enabled)
			{
				state = kThemeStateInactive;
			}
			else
			{
				state = kThemeStateUnavailableInactive;
			}
		}
		if (header->sortIndicator != QStyleOptionHeader::None)
		{
			value = kThemeButtonOn;
			if (header->sortIndicator == QStyleOptionHeader::SortDown)
			{
				adornment = kThemeAdornmentHeaderButtonSortUp;
			}
		}
		if (flags & State_HasFocus)
		{
			adornment = kThemeAdornmentFocus;
		}
		ir = visualRect(header->direction, header->rect, ir);
		// XXX HACK XXX - can't seem to find a way to get at a CGContextRef without including internal
		// Qt headers, so, use the qt internal, not-guaranteed-to-exist-in-future-versions pixmap.
		QString key = QString(QLatin1String("$qt_tableh%1-%2-%3")).arg(state).arg(adornment).arg(value);
		QPixmap buffer;
		if (!QPixmapCache::find(key, buffer))
		{
			QMacStyle::drawControl(CE_HeaderSection, option, painter, widget);
		}
		drawTableHeader(
			ir, // internal rect
			false, // top border
			false, // left border
			header->position != QStyleOptionHeader::End && header->position != QStyleOptionHeader::OnlyOneSection, // right border
			key, // pixmap key
			painter
		);
	}
}

void MacStyle::Implementation::drawTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	if(!(option->state.testFlag(State_Selected) || (option->state.testFlag(State_MouseOver) && option->state.testFlag(State_Enabled))))
	{
		return;
	}
	painter->save();
	painter->translate(3, 0);

	const QBrush selectedBrush(QColor(144, 144, 144));
	const QBrush activeBrush(QColor(113, 113, 113));

	if(option->state.testFlag(State_Sunken) /*&& option->state.testFlag(State_MouseOver)*/)
	{
		painter->setBrush(activeBrush);
	}
	else
	{
		painter->setBrush(selectedBrush);
	}

	const bool drawBorder = option->state.testFlag(State_Sunken) || option->state.testFlag(State_Selected);

	QRectF highlightRect(option->rect);
	highlightRect.setHeight(highlightRect.height() - 4);
	highlightRect.setTop(highlightRect.top() + 3);
	highlightRect.setWidth(highlightRect.width() - 4);
	highlightRect.setLeft(highlightRect.left() + 2);

	painter->setRenderHint(QPainter::Antialiasing);

	if(drawBorder)
	{
		painter->save();
		QRectF borderRect(highlightRect);

		// 2px top padding, 2px bottom
		highlightRect.setTop(highlightRect.top() + 2);

		// 1px each side
		highlightRect.setWidth(highlightRect.width() - 1);
		highlightRect.setLeft(highlightRect.left() + 1);

		// Now, draw a gradient around it
		QColor topColor;
		const QColor bottomColor = QColor(230, 230, 230);
		if(option->state.testFlag(State_Sunken))
		{
			topColor = QColor(47, 47, 47);
		}
		else
		{
			topColor = QColor(101, 101, 101);
		}
		const QColor middleColor = painter->brush().color();
		const QColor topMidColor = middleColor.darker(120);
		const QColor bottomMidColor = middleColor.darker(90);
		
		painter->setPen(Qt::NoPen);

		QRectF gradientRect(borderRect);
		gradientRect.setHeight(gradientRect.height() + 1);

		QLinearGradient gradient;
		gradient.setColorAt(0, topColor);
		gradient.setColorAt(0.5, middleColor);
		gradient.setColorAt(0.75, bottomMidColor);
		gradient.setColorAt(1, bottomColor);
		gradient.setStart(0, 0);
		gradient.setFinalStop(0, gradientRect.height());

		painter->setBrush(gradient);
		painter->drawRoundedRect(gradientRect, 8, 12);
		
		painter->setPen(topColor);
		painter->drawLine(borderRect.topLeft() + QPointF(8, 0.5), borderRect.topRight() + QPointF(-8, 0.5));
		painter->setPen(topMidColor);
		painter->drawLine(borderRect.topLeft() + QPointF(8, 1.5), borderRect.topRight() + QPointF(-8, 1.5));
		painter->setPen(bottomMidColor);
		painter->drawLine(borderRect.bottomLeft() + QPointF(8, -0.5), borderRect.bottomRight() + QPointF(-8, -0.5));
		painter->setPen(bottomColor);
		painter->drawLine(borderRect.bottomLeft() + QPointF(8, 0.5), borderRect.bottomRight() + QPointF(-8, 0.5));

		painter->restore();
	}

	painter->setPen(Qt::NoPen);

	// Draw the rounded rectangle
	painter->drawRoundedRect(highlightRect, 8, 12);

	// If it's active or sunken, draw a border
	if(
		option->state.testFlag(State_Selected)
		||
		(
			option->state.testFlag(State_Sunken) && option->state.testFlag(State_MouseOver)
		)
	)
	{
	}

	painter->restore();
}

void MacStyle::Implementation::drawVerticalSplitter(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	Q_UNUSED(widget);
	// Taken from MacMainWindow Qt example
	painter->save();
	painter->setPen(QColor(145, 145, 145));
	painter->drawLine(0, 0, 0, option->rect.height());
	painter->restore();
}

void MacStyle::Implementation::drawHorizontalSplitter(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	QRect r(option->rect);

	painter->save();
	QColor topColor(145, 145, 145);
	QColor bottomColor(142, 142, 142);
	QColor gradientStart(252, 252, 252);
	QColor gradientStop(211, 211, 211);

	painter->setPen(topColor);
	painter->drawLine(0, 0, r.width(), 0);
	painter->setPen(bottomColor);
	painter->drawLine(0, r.height() - 1, r.width(), r.height() - 1);

	QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, r.height() -3));
	linearGrad.setColorAt(0, gradientStart);
	linearGrad.setColorAt(1, gradientStop);
	painter->fillRect(QRect(QPoint(0,1), r.size() - QSize(0, 2)), QBrush(linearGrad));
	painter->restore();

	QMacStyle::drawControl(CE_Splitter, option, painter, widget);

}

QFont MacStyle::Implementation::systemFont()
{
	QFont font("Lucida Grande", 12, QFont::Bold);
	font.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
	return font;
}

void MacStyle::Implementation::drawTabBarTabLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
	QStyleOptionTabV3 tabOption(*qstyleoption_cast<const QStyleOptionTabV3*>(option));
	Q_UNUSED(widget);

	painter->save();
	painter->translate(3, 0);
	QPen topPen;
	QPen shadowPen;
	if(option->state.testFlag(State_Selected) || (option->state.testFlag(State_MouseOver) && option->state.testFlag(State_Enabled)))
	{
		topPen = QPen(option->palette.base(), 1);
		shadowPen = QPen(QColor(72, 72, 72));
	}
	else
	{
		if(option->state.testFlag(State_Enabled))
		{
			topPen = QPen(QColor(60, 60, 60));
		}
		else
		{
			topPen = QPen(option->palette.text(), 1);
		}
		shadowPen = QPen(option->palette.midlight(), 1);
	}
	painter->setFont(systemFont());

	painter->setRenderHint(QPainter::Antialiasing);

	painter->setPen(shadowPen);
	painter->drawText(option->rect.translated(0, 1), Qt::AlignCenter | Qt::TextShowMnemonic, tabOption.text);
	painter->setPen(topPen);
	painter->drawText(option->rect.translated(0, 0), Qt::AlignCenter | Qt::TextShowMnemonic, tabOption.text);
	painter->restore();
}

void MacStyle::Implementation::drawTableHeader(const QRect& outerBounds, bool drawTopBorder, bool drawLeftBorder, bool drawRightBorder, const QString& key, QPainter* p)
{
	// Grabbed from qmacstyle_mac.mm
	static SInt32 headerHeight = 0;
	static OSStatus err = GetThemeMetric(kThemeMetricListHeaderHeight, &headerHeight);
	Q_UNUSED(err);

	QPixmap buffer;
	// XXX HACK XXX - can't seem to find a way to get at a CGContextRef without including internal
	// Qt headers, so, use the qt internal, not-guaranteed-to-work pixmap
	if (!QPixmapCache::find(key, buffer))
	{
		qFatal("Couldn't find qt table pixmap '%s'.", qPrintable(key));
	}
	const int buttonw = qRound(outerBounds.width());
	const int buttonh = qRound(outerBounds.height());
	const int framew = 1;
	const int frameh_n = 4;
	const int frameh_s = 3;
	const int transh = buffer.height() - frameh_n - frameh_s;
	int center = buttonh - frameh_s - int(transh / 2.0f) + 1; // Align bottom;

	int skipTopBorder = 0;
	if (!drawTopBorder)
	{
		skipTopBorder = 1;
	}

	p->save();
	p->translate(outerBounds.topLeft());

	// If we have no right border, extend the middle block for a pixel.
	int extraCentreWidth = 0;
	if(!drawRightBorder)
	{
		extraCentreWidth = 1;
	}

	p->drawPixmap(QRect(QRect(0, -skipTopBorder, buttonw + extraCentreWidth - framew , frameh_n)), buffer, QRect(framew, 0, 1, frameh_n));
	p->drawPixmap(QRect(0, buttonh - frameh_s, buttonw + extraCentreWidth - framew, frameh_s), buffer, QRect(framew, buffer.height() - frameh_s, 1, frameh_s));
	// Draw upper and lower center blocks
	p->drawPixmap(QRect(0, frameh_n - skipTopBorder, buttonw + extraCentreWidth - framew, center - frameh_n + skipTopBorder), buffer, QRect(framew, frameh_n, 1, 1));
	p->drawPixmap(QRect(0, center, buttonw + extraCentreWidth - framew, buttonh - center - frameh_s), buffer, QRect(framew, buffer.height() - frameh_s, 1, 1));
	if(drawRightBorder)
	{
		// Draw right center block borders
		p->drawPixmap(QRect(buttonw - framew, frameh_n - skipTopBorder, framew, center - frameh_n), buffer, QRect(buffer.width() - framew, frameh_n, framew, 1));
		p->drawPixmap(QRect(buttonw - framew, center, framew, buttonh - center - 1), buffer, QRect(buffer.width() - framew, buffer.height() - frameh_s, framew, 1));
		// Draw right corners
		p->drawPixmap(QRect(buttonw - framew, -skipTopBorder, framew, frameh_n), buffer, QRect(buffer.width() - framew, 0, framew, frameh_n));
		p->drawPixmap(QRect(buttonw - framew, buttonh - frameh_s, framew, frameh_s), buffer, QRect(buffer.width() - framew, buffer.height() - frameh_s, framew, frameh_s));
		// Draw right center transition block border
		p->drawPixmap(QRect(buttonw - framew, center - qRound(transh / 2.0f), framew, buffer.height() - frameh_n - frameh_s), buffer, QRect(buffer.width() - framew, frameh_n + 1, framew, transh));
	}
	// Draw center transition block
	p->drawPixmap(QRect(0, center - qRound(transh / 2.0f), buttonw + extraCentreWidth - framew, buffer.height() - frameh_n - frameh_s), buffer, QRect(framew, frameh_n + 1, 1, transh));
	if(drawLeftBorder)
	{
		// Draw left center block borders
		p->drawPixmap(QRect(0, frameh_n - skipTopBorder, framew, center - frameh_n + skipTopBorder), buffer, QRect(0, frameh_n, framew, 1));
		p->drawPixmap(QRect(0, center, framew, buttonh - center - 1), buffer, QRect(0, buffer.height() - frameh_s, framew, 1));
		// Draw left corners
		p->drawPixmap(QRect(0, -skipTopBorder, framew, frameh_n), buffer, QRect(0, 0, framew, frameh_n));
		p->drawPixmap(QRect(0, buttonh - frameh_s, framew, frameh_s), buffer, QRect(0, buffer.height() - frameh_s, framew, frameh_s));
		// Draw left center transition block border
		p->drawPixmap(QRect(0, center - qRound(transh / 2.0f), framew, buffer.height() - frameh_n - frameh_s), buffer, QRect(0, frameh_n + 1, framew, transh));
	}

	p->restore();
}

QSize MacStyle::Implementation::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const
{
	const QSize upstream = QMacStyle::sizeFromContents(type, option, size, widget);
	switch(type)
	{
		case CT_TabBarTab:
			{
				QStyleOptionTabV3 tabOption(*qstyleoption_cast<const QStyleOptionTabV3*>(option));

				QFontMetrics metrics(systemFont());
				return QSize(metrics.width(tabOption.text) + pixelMetric(PM_TabBarTabHSpace, option, widget), metrics.height() + 1 +2*pixelMetric(PM_TabBarTabVSpace, option, widget));
			}
		default:
			return upstream;
	}
}
