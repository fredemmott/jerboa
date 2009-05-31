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

#include "InfoWidget.h"
#include "InfoWidget_Implementation.h"

#include "PlayerInterface.h"

#include <QtPlugin>

InfoWidget::InfoWidget() :
	QObject(),
	d(NULL)
{
} 

QString InfoWidget::pluginName() const
{
	return tr("Track Information");
}

QString InfoWidget::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString InfoWidget::uniqueId() const
{
	return "org.jerboaplayer.InfoWidget";
}

QSet<Jerboa::Plugin::ComponentType> InfoWidget::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::WidgetUsedWithPlaylist;
}

QObject* InfoWidget::component(ComponentType type, QObject* parent)
{
	if(type == Jerboa::Plugin::WidgetUsedWithPlaylist)
	{
		return new Implementation(m_player, qobject_cast<QWidget*>(parent));
	}
	return Jerboa::Plugin::component(type, parent);
}

void InfoWidget::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Player:
			m_player = qobject_cast<Jerboa::PlayerInterface*>(component);
			Q_ASSERT(m_player);
			break;
		default:
			break;
	}
}

Q_EXPORT_PLUGIN2(Jerboa_InfoWidget, InfoWidget);
