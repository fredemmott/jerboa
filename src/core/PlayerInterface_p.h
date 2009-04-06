#ifndef _JERBOA_PLAYER_INTERFACE_PRIVATE_H
#define _JERBOA_PLAYER_INTERFACE_PRIVATE_H

#include "PlaylistInterface.h"
#include "PlayerInterface.h"

namespace Jerboa
{
	class PlayerInterface::Private : public QObject
	{
		Q_OBJECT;
		public:
			Private(PlaylistInterface* playlist, PlayerInterface* parent);
			PlaylistInterface* m_playlist;
		public slots:
			void hitPlayOnPlaylistChange();
		private:
			PlayerInterface* m_player;
	};
}

#endif
