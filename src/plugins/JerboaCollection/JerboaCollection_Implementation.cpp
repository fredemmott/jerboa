#include "JerboaCollection_Implementation.h"

#include "TrackData.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

JerboaCollection::Implementation::Implementation(QObject* parent)
	:
		CollectionInterface(parent)
{
	QSqlQuery query;
	query.setForwardOnly(true);
	query.exec(
		"SELECT FileName, Album, AlbumArtist, AlbumArtistRomanised, Artist, ArtistRomanised, Title, TrackNumber, AlbumRG, TrackRG, MBID FROM TrackData"
	);
	if(!query.isValid())
	{
		qFatal("Couldn't read database: %s", qPrintable(query.lastError().text()));
	}
	if(query.size() != -1)
	{
		m_tracks.reserve(query.size() + 10);
	}
	for(query.first(); query.isValid(); query.next())
	{
		m_tracks.append(
			Jerboa::TrackData(
				QUrl::fromLocalFile(query.value(0).toString()),
				query.value(1).toString(), // Album
				query.value(2).toString(), // AlbumArtist
				query.value(3).toString(), // AlbumArtistRomanised
				query.value(4).toString(), // Artist
				query.value(5).toString(), // ArtistRomanised,
				query.value(6).toString(), // Title
				query.value(7).value<quint8>(), // TrackNumber
				query.value(8).value<qreal>(), // AlbumRG
				query.value(9).value<qreal>(), // TrackRG
				query.value(10).toString() // MBID
			)
		);
	}
	qDebug() << "SELECTED" << m_tracks.count() << "tracks";
}

QVector<Jerboa::TrackData> JerboaCollection::Implementation::tracks() const
{
	return m_tracks;
}
