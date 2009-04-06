#include "PlayerInterface_p.h"

namespace Jerboa
{
	PlayerInterface::Private::Private(PlaylistInterface* playlist, PlayerInterface* parent)
		:
			QObject(parent),
			m_playlist(playlist),
			m_player(parent)
	{
	}

	void PlayerInterface::Private::hitPlayOnPlaylistChange()
	{
	}
}
