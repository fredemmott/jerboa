#include "PlaylistInterface.h"
#include "PlaylistInterface_p.h"

namespace Jerboa
{
	PlaylistInterface::PlaylistInterface(Jerboa::TagReader* tagReader, QObject* parent)
		:
			QObject(parent),
			d(new Private(tagReader, this))
	{
	}

	PlaylistInterface::~PlaylistInterface()
	{
		delete d;
	}

	void PlaylistInterface::clear()
	{
		if(!tracks().isEmpty())
		{
			removeTracks(0, tracks().count());
		}
	}

	void PlaylistInterface::insertTracks(int index, const QList<QUrl>& urls)
	{
		d->insertTracks(index, urls);
	}
}
