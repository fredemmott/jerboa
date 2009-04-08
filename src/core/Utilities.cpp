#include "Utilities.h"

#include "config.h"

#include <QDesktopServices>
#include <QRegExp>

namespace Jerboa
{
	QString Utilities::simpleAlbum(const QString& albumName)
	{
		QString out(albumName);
		out.replace(QRegExp("[[(<{].+"), "");
		return out.simplified();
	}

	QString Utilities::dataLocation()
	{
#ifdef EMBEDDED_USE_FIXED_PATHS
		return EMBEDDED_FIXED_DATA_PATH;
#else
		return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
	}
}
