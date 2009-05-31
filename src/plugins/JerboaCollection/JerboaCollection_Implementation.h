#ifndef _JERBOA_COLLECTION_IMPLEMENTATION_H
#define _JERBOA_COLLECTION_IMPLEMENTATION_H

#include "CollectionInterface.h"
#include "TagReader.h"
#include "JerboaCollection.h"

class CollectionScanner;

#include <QDir>

class QFileSystemWatcher;

class JerboaCollection::Implementation : public Jerboa::CollectionInterface
{
	Q_OBJECT
	public:
		Implementation(Jerboa::TagReader* tagReader, QObject* parent);
		QVector<Jerboa::TrackData> tracks() const;
	private slots:
		void applyChanges(const QList<Jerboa::TrackData>& added, const QList<Jerboa::TrackData>& modified, const QStringList& removed);
		void rescanTree();
	private:
		void monitorDirectory(const QDir& directory);
		const QDir m_directory;
		QVector<Jerboa::TrackData> m_tracks;

		QFileSystemWatcher* m_watcher;
		CollectionScanner* m_collectionScanner;
};

#endif
