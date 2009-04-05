#include "PlaylistInterface.h"

namespace Jerboa
{
	PlaylistInterface::PlaylistInterface(QObject* parent)
		:
			QObject(parent)
	{
	}

	PlaylistInterface::~PlaylistInterface()
	{
	}

	int PlaylistInterface::appendTrack(const QUrl& url)
	{
		Q_UNUSED(url);
		return -1;
	}

	void PlaylistInterface::insertTrack(int index, const QUrl& url)
	{
		Q_UNUSED(url);
	}
}
