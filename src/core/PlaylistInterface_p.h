#ifndef _JERBOA_PLAYLIST_PRIVATE_H
#define _JERBOA_PLAYLIST_PRIVATE_H

#include "PlaylistInterface.h"

#include <QObject>
#include <QQueue>

namespace Jerboa
{
	class PlaylistInterface::Private : public QObject
	{
		Q_OBJECT
		public:
			Private(TagReader* tagReader, PlaylistInterface* playlist);
			void insertTracks(int position, const QList<QUrl>&);
		private slots:
			void addTrackFromUrlDrop(const Jerboa::TrackData& track);
			void tagReaderError();
		private:
			struct UrlDrop
			{
				UrlDrop(int position, const QList<QUrl>& urls);
				int position;
				QQueue<QUrl> urls;
			};
	
			void incrementUrlPositions();
			void loadNextUrl();
	
			QQueue<UrlDrop> m_droppedUrls;
			Jerboa::PlaylistInterface* m_playlist;
			Jerboa::TagReader* m_tagReader;
	};
};

#endif
