#include "TagLibTagReader.h"

#include "TagLibTagReader_Implementation.h"

#include <QDebug>
#include <QtPlugin>

QObject* TagLibTagReader::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::TagReader:
			return this;
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

void TagLibTagReader::readUrl(const QUrl& url)
{
	if(url.scheme() != "file")
	{
		emit schemeNotSupported(url);
		return;
	}
	d->setCurrentSource(url);
}

Jerboa::TagReader* TagLibTagReader::instance(QObject* parent) const
{
	return new TagLibTagReader(parent);
}

TagLibTagReader::TagLibTagReader(QObject* parent)
	:
		Jerboa::TagReader(parent)
{
	d = new Implementation(this);
	connect(
		d,
		SIGNAL(metaDataChanged()),
		this,
		SLOT(metaDataChanged())
	);
}

void TagLibTagReader::metaDataChanged()
{
	if(d->metaData().isEmpty())
	{
		emit notFound(d->currentSource());
		return;
	}

	const QString artist = d->getTag<QString>("ARTIST", tr("Unknown"));
	const QString artistSort = d->getTag<QString>("ARTIST-SORT", artist);
	const QString albumName = d->getTag<QString>("ALBUM", tr("No Album Title"));
	const QString title = d->getTag<QString>("TITLE", tr("No Track Title"));
	const qint8 trackNumber = d->getTag<quint8>("TRACK-NUMBER", 0);
	const qreal trackReplayGain = d->getTag<qreal>("REPLAYGAIN-TRACK-GAIN", -10000);
	const qreal albumReplayGain = d->getTag<qreal>("REPLAYGAIN-ALBUM-GAIN", trackReplayGain);
	const QString mbid = d->getTag<QString>("MUSICBRAINZ-TRACK-ID", QString());
	qDebug() << "Read track" << Q_FUNC_INFO << d->currentSource() << d->currentSource().toLocalFile();
	emit finished(
		Jerboa::TrackData(
			d->currentSource(),
			albumName,
			QString(),
			QString(),
			artist,
			artistSort,
			title,
			trackNumber,
			albumReplayGain,
			trackReplayGain,
			mbid
		)
	);
}

QString TagLibTagReader::pluginName() const
{
	return tr("TagLib Tag Reader");
}

QString TagLibTagReader::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString TagLibTagReader::uniqueId() const
{
	return "org.jerboaplayer.TagLibTagReader";
}

QSet<Jerboa::Plugin::ComponentType> TagLibTagReader::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::TagReader;
}

Q_EXPORT_PLUGIN2(Jerboa_TagLibTagReader, TagLibTagReader);
