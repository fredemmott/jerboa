#ifndef _PLAYLIST_MODEL_IMPLEMENTATION_H
#define _PLAYLIST_MODEL_IMPLEMENTATION_H

#include "PlaylistInterface.h"
#include "PlaylistModel.h"
#include "TagReader.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QQueue>
#include <QStringList>

class PlaylistModel::Implementation : public QAbstractItemModel
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlaylistInterface* playlist, Jerboa::TagReader* tagReader, QObject* parent);

		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent) const;

		Qt::ItemFlags flags(const QModelIndex& index) const;
		QStringList mimeTypes() const;
		bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
		Qt::DropActions supportedDropActions() const;
	private slots:
		void addTracks(int index, const QList<Jerboa::TrackData>& data);
		void removeTracks(int index, int count);
		void highlightCurrentTrack(int newCurrentTrack);
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
		int m_currentTrack;
		Jerboa::PlaylistInterface* m_playlist;
		Jerboa::TagReader* m_tagReader;
		QList<Jerboa::TrackData> m_tracks;
};

#endif
