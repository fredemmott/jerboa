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
#ifndef _JERBOA_PLUGIN_H
#define _JERBOA_PLUGIN_H

#include <QDebug>
#include <QSet>
#include <QString>
#include <QUrl>

namespace Jerboa
{
	class PlayerInterface;
	/** Abstract class defining the plugin interface.
	 * All Jerboa plugins must inherit from this class.
	 */
	class Plugin
	{
		Q_GADGET
		public:
			enum ComponentType {
				Container, // Container
				WidgetUsedWithPlaylist, // QWidget
				WidgetUsedWithCollection, // QWidget
				Player, // PlayerInterface
				PlaylistSource, // PlaylistInterface
				PlaylistModel, // QAbstractItemModel, Qt::UserDataRole is a TrackData
				PlaylistView, // QWidget
				CollectionSource, // CollectionInterface
				CollectionModel, // QAbstractItemModel - Qt::UserDataRole is a QList<TrackData>
				CollectionView, // QWidget
				TagReader, // TagReader
				ToolBar, // QToolBar
				FirstRunWizardPage, // QWizardPage
				BackEnd, // TBA
				FrontEnd // TBA
			};
			Q_ENUMS(ComponentType);
			/** Gets the plugins name.
			 * This is potentially user-visible, so should be translated
			 */
			virtual QString pluginName() const = 0;
			/// Get the plugin author.
			virtual QString pluginAuthor() const = 0;
			/** Get the plugin website.
			 *
			 * Default implementation returns a null QUrl
			 */
			virtual QUrl pluginWebsite() const;
			/** Get a unique ID for this plugin.
			 * I'd prefer this to be of reverse-domain format, eg:
			 * 	uk.co.fredemmott.jerboa.MprisPlugin
			 * If you don't have control over a suitable domain, I'll
			 * 	also accept UUIDs.
			 */
			virtual QString uniqueId() const = 0;

			/** Called when a component is chosen.
			 * Default implementation does nothing.
			 */
			virtual void addComponent(ComponentType, QObject* component);

			/** Return a list of components that this plugin provides.
			 *
			 * Default implementation returns the empty set.
			 */
			virtual QSet<ComponentType> components() const;
			/** Provide a component.
			 *
			 * Default implementation calls qFatal.
			 */
			virtual QObject* component(ComponentType, QObject* parent);
	};
}

Q_DECLARE_INTERFACE(
	Jerboa::Plugin,
	"uk.co.fredemmott.jerboa.plugin/2.0"
);

QDebug operator<<(QDebug debug, Jerboa::Plugin::ComponentType type);
#endif
