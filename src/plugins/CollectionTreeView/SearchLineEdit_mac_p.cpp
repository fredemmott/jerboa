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
#include "SearchLineEdit_mac_p.h"

#include "QtCFString.h"

#include <QDebug>
#include <QMoveEvent>
#include <QTimerEvent>

SearchLineEdit_mac::Private::Private(QWidget* parent)
: QWidget(parent)
, m_nativeSearchText(QtCFString::toCFStringRef(QString()))
{
	// Create the widget
	HISearchFieldCreate(
		NULL, // bounds
		kHISearchFieldAttributesSearchIcon, // attributes
		NULL, // menu
		NULL, // gray text
		&m_nativeWidget.data()
	);
	// List of events to subscribe to
	EventTypeSpec events[] =
	{
		{ kEventClassSearchField, kEventSearchFieldCancelClicked },
		{ kEventClassTextField, kEventTextAccepted },
		{ kEventClassTextField, kEventTextDidChange }
	};
	// Subscribe to them
	HIViewInstallEventHandler(
		m_nativeWidget.data(),
		eventHandler,
		sizeof(events),
		events,
		reinterpret_cast<void*>(this),
		NULL
	);
	create(reinterpret_cast<WId>(m_nativeWidget.data()));
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFixedHeight(sizeHint().height());
}

QString SearchLineEdit_mac::Private::text() const
{
	return m_searchText;
}

QSize SearchLineEdit_mac::Private::sizeHint() const
{
	// taken from MacMainWindow sample
	EventRef event;
	HIRect optimalBounds;
	CreateEvent(0, kEventClassControl,
		kEventControlGetOptimalBounds,
		GetCurrentEventTime(),
		kEventAttributeUserEvent, &event);

	SendEventToEventTargetWithOptions(event,
		HIObjectGetEventTarget(HIObjectRef(winId())),
		kEventTargetDontPropagate);

	GetEventParameter(event,
		kEventParamControlOptimalBounds, typeHIRect,
		0, sizeof(HIRect), 0, &optimalBounds);

	ReleaseEvent(event);
	if(optimalBounds.size.width == 0)
	{
		optimalBounds.size.width = 90;
	}
	return QSize(
		optimalBounds.size.width + 100, // make it a bit wider.
		optimalBounds.size.height
	);

}

void SearchLineEdit_mac::Private::updateText()
{
	ScopedCfPointer<CFStringRef> nativeText = HIViewCopyText(m_nativeWidget.data());
	m_searchText = QtCFString::toQString(nativeText.data());
	setClearButtonVisible(!m_searchText.isEmpty());
	emit textChanged(m_searchText);
}

void SearchLineEdit_mac::Private::clear()
{
	setText(QString());
}

void SearchLineEdit_mac::Private::setText(const QString& text)
{
	m_nativeSearchText = QtCFString::toCFStringRef(text);
	HIViewSetText(m_nativeWidget.data(), m_nativeSearchText.data());
	m_searchText = text;
	setClearButtonVisible(!text.isEmpty());
	emit textChanged(text);
}

void SearchLineEdit_mac::Private::setFocus(Qt::FocusReason reason)
{
	HIViewSetNextFocus(m_nativeWidget,NULL);
	HIViewAdvanceFocus(m_nativeWidget,NULL);
}

OSStatus SearchLineEdit_mac::Private::eventHandler(
	EventHandlerCallRef handler,
	EventRef event,
	void* userData
)
{
	SearchLineEdit_mac::Private *widget = qobject_cast<SearchLineEdit_mac::Private*>(reinterpret_cast<QWidget*>(userData));
	Q_ASSERT(widget);

	switch(GetEventClass(event))
	{
		case kEventClassSearchField:
			switch(GetEventKind(event))
			{
				case kEventSearchFieldCancelClicked:
					widget->clear();
					break;
			}
			break;
		case kEventClassTextField:
			switch(GetEventKind(event))
			{
				case kEventTextAccepted:
					widget->updateText();
					widget->returnPressed();
					break;
				case kEventTextDidChange:
					widget->updateText();
					break;
			}
			break;
	}
	return eventNotHandledErr;
}

void SearchLineEdit_mac::Private::setClearButtonVisible(bool visible)
{
	if(visible)
	{
		HISearchFieldChangeAttributes(
			m_nativeWidget,
			kHISearchFieldAttributesCancel, // Set
			NULL // Clear
		);
	}
	else
	{
		HISearchFieldChangeAttributes(
			m_nativeWidget,
			NULL, // Set
			kHISearchFieldAttributesCancel // Clear
		);
	}
}
