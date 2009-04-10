#ifndef _PLAYLIST_MODEL_IMPLEMENTATION_H
#define _PLAYLIST_MODEL_IMPLEMENTATION_H

#include "PlaylistInterface.h"
#include "PlaylistModel.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QStringList>

class PlaylistModel::Implementation : public QAbstractItemModel
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent);

		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& index) const;
		int rowCount(const QModelIndex& parent) const;

		Qt::ItemFlags flags(const QModelIndex& index) const;
		QStringList mimeTypes() const;
		bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	private slots:
		void addTracks(int index, const QList<Jerboa::TrackData>& data);
		void removeTracks(int index, int count);
		void highlightCurrentTrack(int newCurrentTrack);
	private:
		int m_currentTrack;
		Jerboa::PlaylistInterface* m_playlist;
		QList<Jerboa::TrackData> m_tracks;
};

#endif
