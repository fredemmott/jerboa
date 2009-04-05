#ifndef _JERBOA_FRONT_END_H
#define _JERBOA_FRONT_END_H

#include <QObject>

namespace Jerboa
{
	class FrontEnd: public QObject
	{
		Q_OBJECT;
		public:
			virtual ~FrontEnd();
			/// The Plugin::uniqueId() of the container that's wanted
			virtual QString containerId() const = 0;
			virtual QString playlistViewId() const = 0;
			virtual QString collectionViewId() const = 0;
		protected:
			FrontEnd(QObject* parent);
	};
};

#endif
