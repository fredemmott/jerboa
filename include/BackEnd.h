#ifndef _JERBOA_BACK_END_H
#define _JERBOA_BACK_END_H

#include <QObject>

namespace Jerboa
{
	class BackEnd: public QObject
	{
		Q_OBJECT;
		public:
			virtual ~BackEnd();
			virtual QString playerId() const = 0;
			virtual QString playlistSourceId() const = 0;
			virtual QString playlistViewId() const = 0;
		protected:
			BackEnd(QObject* parent);
	};
};

#endif
