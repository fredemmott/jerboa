/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), version 3 of the license.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _INFO_WIDGET_IMPLEMENTATION_H
#define _INFO_WIDGET_IMPLEMENTATION_H

#include "InfoWidget.h"
#include "PlayerInterface.h"
#include "TrackData.h"

#include <QByteArray>
#include <QFile>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#include "ui_InfoWidget.h"

class InfoWidget::Implementation : public QWidget
{
	Q_OBJECT;
	public:
		Implementation(Jerboa::PlayerInterface* player, QWidget* parent);

	private slots:
		void showTrackInformation(const Jerboa::TrackData& track);

		void gotCoverArtAwsXML(QNetworkReply*); // Amazon Web Service
		void gotCoverArtMbXML(QNetworkReply*); // MusicBrainz
		void gotCoverArt(QNetworkReply*);
		void gotLyrics(QNetworkReply*);
		void coverArtContextMenu(const QPoint& pos);
		void refetchCoverArt();
		void chooseCoverArt();
	protected:
		void mousePressEvent(QMouseEvent* event);
	private:
		void clear();
		void setTrack(const Jerboa::TrackData& track);

		// Functions
		QUrl getAWSUrl(const QString& album) const;

		// Constants
		const QString m_coverArtPath;
		const QString m_lyricsPath;
		const QString m_awsKey;

		// Current Track
		Jerboa::TrackData m_currentTrack;

		// Other member variables
		Ui_InfoWidget ui;
		QString m_simpleAlbumName;
		QFile m_coverArtFile;
		QNetworkAccessManager* m_coverArtDownloader;
		bool m_coverArtFirstTry;
		QFile m_lyricsFile;
		QNetworkAccessManager* m_lyricsDownloader;

		QMenu* m_coverArtMenu;
};

#endif
