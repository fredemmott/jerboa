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
#ifndef _TAG_LIB_TAG_READER_IMPLEMENTATION_H
#define _TAG_LIB_TAG_READER_IMPLEMENTATION_H

#include "TagLibTagReader.h"

#include <QFuture>
#include <QMultiMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include <taglib/tfile.h>
#include <taglib/tstring.h>

class TagLibTagReader::Implementation: public QObject
{
	Q_OBJECT
	public:
		Implementation(QObject* parent);
		void setCurrentSource(const QUrl& url);
		QUrl currentSource() const;
		QMultiMap<QString, QString> metaData() const;
		QStringList metaData(const QString& key) const;
		template<class T> T getTag(const QString& name, const T& defaultValue) const
		{
				if ( metaData(name).isEmpty() )
						return defaultValue;
				QVariant v = metaData(name).at(0);
				if ( ! v.canConvert<T>() )
						return defaultValue;
				return v.value<T>();
		};
	signals:
		void metaDataChanged();
	private:
		void resolveMetaData();

		QMutex m_mutex;
		QMultiMap<QString, QString> m_metaData;
		QUrl m_url;
		QFuture<void> m_future;

		TagLib::File* m_file;

		TagLib::String getTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const;
		TagLib::String tryApeTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const;
		TagLib::String tryASFTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const;
		TagLib::String tryID3v2Tag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const;
		TagLib::String tryXiphTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const;
};

#endif
