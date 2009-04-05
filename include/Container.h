#ifndef _JERBOA_CONTAINER_H
#define _JERBOA_CONTAINER_H

#include "Plugin.h"

#include <QObject>

namespace Jerboa
{
	class Container : public QObject
	{
		Q_OBJECT
		public:
			virtual ~Container();

			virtual void addComponent(Plugin::ComponentType componentType, QWidget* component, QWidget* widget) const = 0;
			virtual QWidget* widget() const = 0;
		protected:
			Container(QObject* parent);
	};
};

#endif
