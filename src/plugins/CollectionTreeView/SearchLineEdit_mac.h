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
#pragma once

#include <QWidget>

class SearchLineEdit_mac : public QWidget
{
	Q_OBJECT;
	public:
		SearchLineEdit_mac(QWidget* parent);

		virtual void setFocus(Qt::FocusReason reason);

		QString text() const;
	public slots:
		void clear();
		void setText(const QString& text);
	signals:
		void textChanged(const QString& text);
		void returnPressed();
	private:
		class Private;
		Private* d;
};
