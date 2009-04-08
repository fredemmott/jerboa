#ifndef _JERBOA_UTILITIES_H
#define _JERBOA_UTILITIES_H

#include <QString>

namespace Jerboa
{
	class Utilities
	{
		public:
			static QString simpleAlbum(const QString& albumName);
			static QString dataLocation();
	};
}

#endif
