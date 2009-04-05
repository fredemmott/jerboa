#include "PlaylistModel_Implementation.h"

#include <QDebug>
#include <QRegExp>

PlaylistModel::Implementation::Implementation(Jerboa::PlaylistInterface* playlist, QObject* parent)
	:
		QAbstractItemModel(parent),
		m_playlist(playlist),
		m_tracks(playlist->tracks())
{
}

int PlaylistModel::Implementation::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 3;
}

QVariant PlaylistModel::Implementation::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(orientation);
	if(role != Qt::DisplayRole)
	{
		return QVariant();
	}
	switch(section)
	{
		case 0:
			return tr("Title");
		case 1:
			return tr("Artist");
		case 2:
			return tr("Album");
		default:
			return QVariant();
	}
}

QVariant PlaylistModel::Implementation::data(const QModelIndex& index, int role) const
{
	if(role != Qt::DisplayRole || !index.isValid())
	{
		return QVariant();
	}
	if(index.row() < 0 || index.row() >= m_tracks.count())
	{
		return QVariant();
	}
	Jerboa::TrackData track = m_tracks.at(index.row());
	switch(index.column())
	{
		case 0:
			return track.title();
		case 1:
			return track.artist();
		case 2:
			return track.album();
		default:
			return QVariant();
	}
}

QModelIndex  PlaylistModel::Implementation::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

int PlaylistModel::Implementation::rowCount(const QModelIndex& parent) const
{
	if(parent.isValid())
	{
		return 0;
	}
	else
	{
		return m_tracks.count();
	}
}

QModelIndex PlaylistModel::Implementation::index(int row, int column, const QModelIndex& parent) const
{
	if(column < 0 || column > 2 || row < 0 || row >= m_tracks.count() || parent.isValid())
	{
		return QModelIndex();
	}
	return createIndex(row, column);
}
