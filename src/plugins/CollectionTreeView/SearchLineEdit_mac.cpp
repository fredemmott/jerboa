/* LICENSE NOTICE
	SearchLineEdit_mac: Container for native OSX search box.
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
#include "SearchLineEdit_mac.h"
#include "SearchLineEdit_mac_p.h"

#include <QDebug>
#include <QHBoxLayout>

SearchLineEdit_mac::SearchLineEdit_mac(QWidget* parent)
: QWidget(parent)
, d(new SearchLineEdit_mac::Private(this))
{
	d->move(2, 2); // highlight frame space

	connect(
		d,
		SIGNAL(textChanged(QString)),
		this,
		SIGNAL(textChanged(QString))
	);
	connect(
		d,
		SIGNAL(returnPressed()),
		this,
		SIGNAL(returnPressed())
	);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setMinimumSize(sizeHint());
	setFixedHeight(sizeHint().height());
}

QSize SearchLineEdit_mac::sizeHint() const
{
	return d->sizeHint() + QSize(6, 4);
}

void SearchLineEdit_mac::clear()
{
	d->clear();
}

QString SearchLineEdit_mac::text() const
{
	return d->text();
}

void SearchLineEdit_mac::setText(const QString& text)
{
	d->setText(text);
}

void SearchLineEdit_mac::setFocus(Qt::FocusReason reason)
{
	d->setFocus(reason);
}
