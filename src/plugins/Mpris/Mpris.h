/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _MPRIS_H
#define _MPRIS_H

#include "PlayerInterface.h"
#include "PlaylistInterface.h"
#include "Plugin.h"

class Mpris: public QObject, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		Mpris();

		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;
		void addComponent(ComponentType type, QObject* component);
	private:
		void createInstance();
		Jerboa::PlaylistInterface* m_playlist;
		Jerboa::PlayerInterface* m_player;
};

#endif
