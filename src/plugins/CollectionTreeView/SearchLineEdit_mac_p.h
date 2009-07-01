/* LICENSE NOTICE
	SearchLineEdit_mac::Private: Native Carbon search box in QWidget.
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

#include "SearchLineEdit_mac.h"
#include "ScopedCfPointer.h"

#include <Carbon/Carbon.h>

class SearchLineEdit_mac::Private: public QWidget 
{
	Q_OBJECT;
	public:
		Private(QWidget* parent);

		virtual QSize sizeHint() const;
		virtual void setFocus(Qt::FocusReason reason);

		QString text() const;
	public slots:
		void clear();
		void setText(const QString& text);
	signals:
		void textChanged(const QString& text);
		void returnPressed();
	private:
		void setClearButtonVisible(bool visible);

		/// Update m_searchText with new text.
		void updateText();

		ScopedCfPointer<HIViewRef> m_nativeWidget;
		ScopedCfPointer<CFStringRef> m_nativeSearchText;
		QString m_searchText;

		// QWidget::macEvent() doesn't get all the events we need
		// at least the cancel-clicked one is missing
		static OSStatus eventHandler(EventHandlerCallRef handler, EventRef event, void* userData);
};
