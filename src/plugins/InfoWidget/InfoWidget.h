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
#ifndef _INFOWIDGET_H
#define _INFOWIDGET_H

#include "PlayerInterface.h"
#include "Plugin.h"

#include <QString>

class InfoWidget : public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		InfoWidget();

		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;

		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent) const;
		void addComponent(ComponentType type, QObject* component);
	private:
		class Implementation;
		Implementation* d;

		Jerboa::PlayerInterface* m_player;
};

#endif
