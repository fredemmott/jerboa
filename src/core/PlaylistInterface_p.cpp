#include "PlaylistInterface_p.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

namespace Jerboa
{
	PlaylistInterface::Private::Private(TagReader* tagReader, PlaylistInterface* playlist)
		:
			QObject(playlist),
			m_playlist(playlist),
			m_tagReader(tagReader->instance(this))
	{
		connect(
			m_tagReader,
			SIGNAL(notFound(QUrl)),
			this,
			SLOT(tagReaderError())
		);
		connect(
			m_tagReader,
			SIGNAL(schemeNotSupported(QUrl)),
			this,
			SLOT(tagReaderError())
		);
		connect(
			m_tagReader,
			SIGNAL(finished(Jerboa::TrackData)),
			this,
			SLOT(addTrackFromUrlDrop(Jerboa::TrackData))
		);
	}

	void PlaylistInterface::Private::tagReaderError()
	{
		m_droppedUrls.head().urls.dequeue();
		if(m_droppedUrls.head().urls.isEmpty())
		{
			m_droppedUrls.dequeue();
		}
		loadNextUrl();
	}
	
	void PlaylistInterface::Private::addTrackFromUrlDrop(const Jerboa::TrackData& track)
	{
		UrlDrop* drop = &m_droppedUrls[0];
		Q_ASSERT(drop->urls.head() == track.url());
		drop->urls.dequeue();
		m_playlist->insertTracks(qBound(0, drop->position, m_playlist->tracks().count()), QList<Jerboa::TrackData>() << track);
		incrementUrlPositions();
		loadNextUrl();
	}

	void PlaylistInterface::Private::insertTracks(int position, const QList<QUrl>& urls)
	{
		m_droppedUrls.enqueue(UrlDrop(position >= 0 ? position : m_playlist->tracks().count(), urls));
		if(m_droppedUrls.count() == 1)
		{
			loadNextUrl();
		}
	}
	
	void PlaylistInterface::Private::incrementUrlPositions()
	{
		QMutableListIterator<UrlDrop> iterator(m_droppedUrls);
		while(iterator.hasNext())
		{
			UrlDrop* drop = &iterator.next();
			if(drop->urls.isEmpty())
			{
				iterator.remove();
			}
			else
			{
				drop->position++;
			}
		}
	}
	
	void PlaylistInterface::Private::loadNextUrl()
	{
		if(m_droppedUrls.isEmpty())
		{
			return;
		}
		Q_ASSERT(!m_droppedUrls.first().urls.isEmpty());
		UrlDrop* drop = &m_droppedUrls.head();
		const QUrl url(drop->urls.head());
		if(url.scheme() == "file")
		{
			drop->urls.dequeue();
			QStringList fileNames;
			fileNames.append(url.toLocalFile());
			QList<QUrl> urls;
			while(!fileNames.isEmpty())
			{
				const QString fileName(fileNames.takeFirst());
				if(QFileInfo(fileName).isDir())
				{
					Q_FOREACH(const QString& entry, QDir(fileName).entryList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name))
					{
						fileNames.append(fileName + '/' + entry);
					}
				}
				else
				{
					urls.prepend(QUrl::fromLocalFile(fileName));
				}
				QApplication::processEvents();
			}
			Q_FOREACH(const QUrl& url, urls)
			{
				drop->urls.prepend(url);
			}
			m_tagReader->readUrl(drop->urls.head());
		}
		else
		{
			m_tagReader->readUrl(url);
		}
	}

	PlaylistInterface::Private::UrlDrop::UrlDrop(int _position, const QList<QUrl>& _urls)
	{
		position = _position;
		Q_FOREACH(const QUrl& url, _urls)
		{
			urls.enqueue(url);
		}
	}
}
