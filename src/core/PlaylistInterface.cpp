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

	void PlaylistInterface::clear()
	{
		if(!tracks().isEmpty())
		{
			removeTracks(0, tracks().count());
		}
	}
}
