/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "InfoWidget_Implementation.h"

#include "config.h"
#include "Utilities.h"

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QMouseEvent>
#include <QRegExp>
#include <QStyle>

InfoWidget::Implementation::Implementation(Jerboa::PlayerInterface* player, QWidget* parent) :
	QWidget(parent),
	m_coverArtPath(Jerboa::Utilities::dataLocation() + "/coverImages"),
	m_lyricsPath(Jerboa::Utilities::dataLocation() + "/lyrics"),
	m_awsKey("1JRV0F20598Z9ZAPF7R2"), // Please don't copy this to other programs; these keys are free from aws.amazon.com
	m_coverArtDownloader(new QNetworkAccessManager(this)),
	m_lyricsDownloader(new QNetworkAccessManager(this))
{
	// Connect the player
	connect(
		player,
		SIGNAL(currentTrackChanged(Jerboa::TrackData)),
		this,
		SLOT(showTrackInformation(Jerboa::TrackData))
	);

	// Make sure the directories exist
	QDir dir;
	dir.mkpath(m_coverArtPath);
	dir.mkpath(m_lyricsPath);

	// Connect the downloader
	connect(m_lyricsDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotLyrics(QNetworkReply*)));

	// Setup UI
	ui.setupUi(this);
	const int coverArtLength = 160 + (2 * ui.coverArtLabel->frameWidth());
	ui.coverArtLabel->setMaximumSize(coverArtLength, coverArtLength);
	ui.coverArtLabel->setMinimumSize(coverArtLength, coverArtLength);

	connect(
		ui.coverArtLabel, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(coverArtContextMenu(const QPoint&))
	);

	ui.creditsLabel->setText(tr("Cover art from <a href='http://%1'>%1</a>, lyrics from <a href='http://%2.org'>%2</a>.").arg("Amazon.com").arg("LyricWiki"));

	clear();

	m_coverArtMenu = new QMenu(this);
	QAction* a;
	a = new QAction(
		QApplication::style()->standardIcon(QStyle::SP_BrowserReload),
		tr("Refetch"),
		this
	);
	connect(a, SIGNAL(triggered()), this, SLOT(refetchCoverArt()));
	m_coverArtMenu->addAction(a);
	a = new QAction(
		QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon),
		tr("Choose file.."),
		this
	);
	connect(a, SIGNAL(triggered()), this, SLOT(chooseCoverArt()));
	m_coverArtMenu->addAction(a);
}

void InfoWidget::Implementation::clear()
{
	ui.coverArtLabel->setText(tr("No Album"));
	ui.trackLabel->setText(tr("No Track"));
	ui.artistLabel->setText(tr("No Artist"));
	ui.albumLabel->setText(tr("No Album"));

	ui.lyricsEdit->setDisabled(true);
	ui.lyricsEdit->setText(tr("No Lyrics"));

	m_currentTrack = Jerboa::TrackData();
}

QUrl InfoWidget::Implementation::getAWSUrl(const QString& album) const
{
	QUrl awsUrl("http://webservices.amazon.com/onca/xml/");
	awsUrl.addQueryItem("Service", "AWSECommerceService");
	awsUrl.addQueryItem("Operation", "ItemSearch");
	awsUrl.addQueryItem("ResponseGroup", "Images");
	awsUrl.addQueryItem("SearchIndex", "Music");
	awsUrl.addQueryItem("AWSAccessKeyId", m_awsKey);
	awsUrl.addQueryItem("Title", album);
	return awsUrl;
}

void InfoWidget::Implementation::setTrack(const Jerboa::TrackData& track)
{
	m_currentTrack = track;

	ui.trackLabel->setText(track.title());
	ui.artistLabel->setText(track.artist());
	ui.albumLabel->setText(track.album());

	m_simpleAlbumName = Jerboa::Utilities::simpleAlbum(track.album());

	// If we've got cover art, use it; otherwise, try and download it
	m_coverArtFile.setFileName(
		QString(
			"%1/%2.jpg"
		).arg(
			m_coverArtPath
		).arg(
			QString(
				QCryptographicHash::hash(
					QString(
						"%1#####%2"
					).arg(
						track.albumArtist()
					).arg(
						m_simpleAlbumName
					).toUtf8(),
					QCryptographicHash::Md5
				).toHex()
			)
		)
	);
	if(!m_coverArtFile.exists())
	{
		refetchCoverArt();
	}
	else
	{
		ui.coverArtLabel->setPixmap(QPixmap(m_coverArtFile.fileName(), "JPEG"));
	}

	// See if we have lyrics
	ui.lyricsEdit->setDisabled(true);
	m_lyricsFile.setFileName(
		QString(
			"%1/%2.txt"
		).arg(
			m_lyricsPath
		).arg(
			QString(
				QCryptographicHash::hash(
					QString(
						"%1#####%2#####%3"
					).arg(
						track.artist()
					).arg(
						m_simpleAlbumName
					).arg(
						track.title()
					).toUtf8(),
					QCryptographicHash::Md5
				).toHex()
			)
		)
	);

	if((!m_lyricsFile.exists()) || (m_lyricsFile.size() == 0 && QDateTime::currentDateTime().toTime_t() - QFileInfo(m_lyricsFile).lastModified().toTime_t() > 24 * 60 * 60) )
	{
		ui.lyricsEdit->setText(tr("Trying to find lyrics."));
		QUrl lyricsUrl("http://lyricwiki.org/api.php");
		lyricsUrl.addQueryItem("fmt", "text");
		lyricsUrl.addQueryItem("artist", track.artist());
		lyricsUrl.addQueryItem("song", track.title());
		m_lyricsDownloader->get(QNetworkRequest(lyricsUrl));
	}
	else if(m_lyricsFile.size() == 0 )
	{
		ui.lyricsEdit->setText(tr("Could not find lyrics."));
	}
	else
	{
		m_lyricsFile.open(QIODevice::ReadOnly);
		ui.lyricsEdit->setHtml(QString::fromUtf8(m_lyricsFile.readAll()));
		ui.lyricsEdit->setDisabled(false);
		m_lyricsFile.close();
	}
}

void InfoWidget::Implementation::coverArtContextMenu(const QPoint& pos)
{
	if(m_currentTrack.isValid())
	{
		m_coverArtMenu->popup(ui.coverArtLabel->mapToGlobal(pos));
	}
}

void InfoWidget::Implementation::gotCoverArtAwsXML(QNetworkReply* reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		ui.coverArtLabel->setText(tr("Couldn't Find Album Art"));
		return;
	}
	const QByteArray data = reply->readAll();

	const QString url = QUrl(QString(data).replace(QRegExp(".+<MediumImage><URL>([^<]+)<.+"), "\\1")).toString();

	if(!url.contains(QRegExp("^http://")))
	{
		if(m_coverArtFirstTry)
		{
			const QUrl awsUrl = getAWSUrl(ui.albumLabel->text());
			m_coverArtDownloader->get(QNetworkRequest(awsUrl));
			m_coverArtFirstTry = false;
			return;
		}
		// Okay, I'm stubborn
		QUrl mbUrl("http://musicbrainz.org/ws/1/release/");
		mbUrl.addQueryItem("type", "xml");
		mbUrl.addQueryItem("title", m_simpleAlbumName);
		disconnect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), 0, 0);
		connect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotCoverArtMbXML(QNetworkReply*)));
		m_coverArtDownloader->get(QNetworkRequest(mbUrl));
		return;
	}

	ui.coverArtLabel->setText(tr("Downloading Album Art"));

	disconnect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), 0, 0);
	connect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotCoverArt(QNetworkReply*)));
	m_coverArtDownloader->get(QNetworkRequest(QUrl::fromEncoded(url.toUtf8())));
}

void InfoWidget::Implementation::gotCoverArtMbXML(QNetworkReply* reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		ui.coverArtLabel->setText(tr("Couldn't Find Album Art"));
		return;
	}
	const QByteArray data = reply->readAll();

	const QString asin = QUrl(QString(data).replace(QRegExp(".+<asin>([^<]+)<.+"), "\\1")).toString();
	if(!asin.contains(QRegExp("^[A-Z0-9]{10}$")))
	{
		ui.coverArtLabel->setText(tr("Couldn't Find Album Art"));
		return;
	}

	QUrl awsUrl("http://webservices.amazon.com/onca/xml/");
	awsUrl.addQueryItem("Service", "AWSECommerceService");
	awsUrl.addQueryItem("Operation", "ItemLookup");
	awsUrl.addQueryItem("ResponseGroup", "Images");
	awsUrl.addQueryItem("AWSAccessKeyId", m_awsKey);
	awsUrl.addQueryItem("ItemId", asin);

	disconnect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), 0, 0);
	connect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotCoverArtAwsXML(QNetworkReply*)));
	m_coverArtDownloader->get(QNetworkRequest(awsUrl));
}

void InfoWidget::Implementation::gotCoverArt(QNetworkReply* reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qDebug() << "Error:" << reply->error();
		qDebug() << "Content:" << reply->readAll();
		qDebug() << "URL:" << reply->url() << "- Encoded:" << reply->url().toEncoded();
		ui.coverArtLabel->setText(tr("Couldn't Download Album Art"));
		return;
	}
	m_coverArtFile.open(QIODevice::WriteOnly);
	m_coverArtFile.write(reply->readAll());
	m_coverArtFile.close();
	ui.coverArtLabel->setPixmap(QPixmap(m_coverArtFile.fileName(), "JPEG"));
}

void InfoWidget::Implementation::gotLyrics(QNetworkReply* reply)
{
	const bool error = reply->error() != QNetworkReply::NoError;
	const QByteArray data = reply->readAll();

	if(error || data == "Not found")
	{
		ui.lyricsEdit->setText(tr("Could not find lyrics."));
		if( data == "Not found")
		{
			// Update mtime
			m_lyricsFile.open(QIODevice::WriteOnly);
			m_lyricsFile.resize(1);
			m_lyricsFile.resize(0);
			m_lyricsFile.close();
		}
		return;
	}

	QStringList lines = QString::fromUtf8(data).split("\n");
	QMutableStringListIterator it(lines);
	bool inLyrics = true;
	while(it.hasNext())
	{
		QString line = it.next().trimmed();
		if(inLyrics && line == "</lyric>")
		{
			inLyrics = false;
			it.remove();
		}
		else if(! inLyrics)
		{
			if(line == "<lyric>")
				inLyrics = true;
			if(line.left(2) == "==" && it.peekNext().trimmed() == "<lyric>")
			{
				it.setValue("<br /><b>" + it.value().replace("=", "") + "</b>");
				continue;
			}
			it.remove();
		}
	}
	QString lyrics = lines.join("<br />\n");
	lyrics = lyrics.replace(QRegExp("'''([^']+)'''"), "<b>\\1</b>");
	lyrics = lyrics.replace(QRegExp("''([^']+)''"), "<i>\\1</i>");

	ui.lyricsEdit->setDisabled(false);
	ui.lyricsEdit->setHtml(lyrics);

	m_lyricsFile.open(QIODevice::WriteOnly);
	m_lyricsFile.write(lyrics.toUtf8());
	m_lyricsFile.close();
}

void InfoWidget::Implementation::refetchCoverArt()
{
	ui.coverArtLabel->setText(tr("Looking up Album"));
	QUrl awsUrl = getAWSUrl(m_simpleAlbumName);
	awsUrl.addQueryItem("Artist", m_currentTrack.albumArtist());
	m_coverArtFirstTry = true;

	disconnect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), 0, 0);
	connect(m_coverArtDownloader, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotCoverArtAwsXML(QNetworkReply*)));
	m_coverArtDownloader->get(QNetworkRequest(awsUrl));
}

void InfoWidget::Implementation::mousePressEvent(QMouseEvent* event)
{
#ifdef EMBEDDED_LEFT_BUTTON_CONTEXT_MENU
	if(childAt(event->pos()) == ui.coverArtLabel)
	{
		m_coverArtMenu->popup(event->globalPos());
	}
#else
	Q_UNUSED(event);
#endif
}

void InfoWidget::Implementation::showTrackInformation(const Jerboa::TrackData& track)
{
	if(track.isValid())
	{
		setTrack(track);
	}
	else
	{
		clear();
	}
}

void InfoWidget::Implementation::chooseCoverArt()
{
	const QString openPath = QFileDialog::getOpenFileName(
		this,
		tr("Select Cover Art"),
		QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
		tr("Images (*.gif *.jpg *.jpeg *.png)")
	);
	QPixmap loader;
	if(loader.load(openPath))
	{
		loader = loader.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		loader.save(m_coverArtFile.fileName(), "JPEG");
		ui.coverArtLabel->setPixmap(loader);
	}
}
