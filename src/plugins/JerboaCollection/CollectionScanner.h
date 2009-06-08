/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _COLLECTION_SCANNER_H
#define _COLLECTION_SCANNER_H

#include "TagReader.h"
#include "TrackData.h"

#include <QMutex>
#include <QObject>
#include <QStringList>

class FileLister;
/// Class for scanning a collection.
class CollectionScanner : public QObject
{
	Q_OBJECT
	public:
		/// Construct a CollectionScanner
		CollectionScanner(Jerboa::TagReader* tagReader, QObject* parent);
	public slots:
		/// Start the collection scan
		void run();
	signals:
		/// Emitted when a track is scanned.
		void progressChanged(quint32 done, const quint32 total);
		/// Emitted when scanning is finished.
		void finished(const QList<Jerboa::TrackData>& added, const QList<Jerboa::TrackData>& modified, const QStringList& removed);
	private slots:
		/** Called when a file list is retrieved.
		 * This will then start reading the files.
		 */
		void haveFileList(const QStringList& files);
		/** Called when data for a single file has been read.
		 * This will process the data, and then read the next file.
		 */
		void processTrack(const Jerboa::TrackData& track);
		/// Called on error
		void skipToNextFile(const QUrl& url);
	private:
		void finish();
		void markFileAsRemoved(const QString& file);
		unsigned int artistId(const QString& artist, const QString& artistSort) const;
		Jerboa::TagReader* m_tagReader;
		FileLister* m_fileLister;

		quint32 m_progress;
		quint32 m_total;

		bool m_inProgress;
		bool m_reRun;
		QStringList m_filesToRead;
		QList<Jerboa::TrackData> m_addedTracks;
		QList<Jerboa::TrackData> m_modifiedTracks;
		QStringList m_modifiedFiles;
		QStringList m_removedFiles;

		QMutex m_restartMutex;
};

#endif
