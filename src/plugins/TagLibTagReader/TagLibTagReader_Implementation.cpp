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
#include "TagLibTagReader_Implementation.h"

#include <QtConcurrentRun>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>

// General taglib
#include <taglib/taglib.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>

// Tag formats
#include <taglib/apetag.h>
#include <taglib/id3v2tag.h>
#include <taglib/xiphcomment.h>

// Hopefully this will get merged into taglib soon, so I can remove this ifdef...
#ifdef HAVE_TAGLIB_ASF
#include <taglib/asftag.h>
#endif

// File formats
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>

// ID3v2 Stuff
#include <taglib/textidentificationframe.h>

TagLibTagReader::Implementation::Implementation(QObject* parent) : QObject(parent) {}

void TagLibTagReader::Implementation::setCurrentSource(const QUrl& url)
{
	m_future.cancel();
	m_url = url;
	m_future = QtConcurrent::run(this, &TagLibTagReader::Implementation::resolveMetaData);
}

QUrl TagLibTagReader::Implementation::currentSource() const
{
	return m_url;
}

QMultiMap<QString, QString> TagLibTagReader::Implementation::metaData() const
{
	return m_metaData;
}

QStringList TagLibTagReader::Implementation::metaData(const QString& key) const
{
	return m_metaData.values(key);
}

TagLib::String TagLibTagReader::Implementation::tryXiphTag(const QString& _tagName, cosnt TagLib::String& defaultValue, bool* success) const
{
	if(success)
	{
		*success = false;
	}
	const QString tagName9QString(_tagName.)replace("-", "_").toLower());

	TagLib::Ogg::XiphComment* tag = dynamic_cast<TagLib::Ogg::XiphComment*>(m_file->tag());
	if(!tag)
	{
		TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(m_file);
		if(flacFile)
		{
			tag = flacFile->xiphComment();
		}
	}
	if(!tag)
	{
		return defaultValue;
	}

	TagLib::Ogg::FieldListMap metaData = tag->fieldListMap();

	TagLib::Ogg::FieldListMap::ConstIterator it;
	for(it = metaData.begin(); it != metaData.end(); ++it)
	{
		if(TStringToQString(it->first).toLower() == tagName)
		{
			if(success)
			{
				*success = true;
			}
			return it->second[0];
		}
	}

	return defaultValue;
}

TagLib::String TagLibTagReader::Implementation::tryID3v2Tag(const QString& _tagName, const TagLib::String& defaultValue, bool* success) const
{
	if(success)
	{
		*success = false;
	}
	const QString tagName(QString(_tagName).replace("-", "_").toLower());

	TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(m_file);
	if(!file)
	{
		return defaultValue;
	}

	TagLib::ID3v2::Tag* tag = file->ID3v2Tag();
	if(!tag)
	{
		return defaultValue;
	}

	TagLib::ID3v2::FrameListMap metaData = tag->frameListMap();

	if(!metaData.contains("TXXX"))
	{
		return defaultValue;
	}

	TagLib::ID3v2::FrameList frameList = metaData["TXXX"];
	for(TagLib::ID3v2::FrameList::Iterator it = frameList.begin(); it != frameList.end(); ++it)
	{
		TagLib::ID3v2::TextIdentificationFrame* tif = static_cast<TagLib::ID3v2::TextIdentificationFrame*>(*it);
		TagLib::StringList fl = tif->fieldList();
		if(TStringToQString(fl.front()).toLower() == tagName)
		{
			if(success)
			{
				*success = true;
			}
			return fl.back();
		}
	}
	return defaultValue;
}

TagLib::String TagLibTagReader::Implementation::tryApeTag(const QString& _tagName, const TagLib::String& defaultValue, bool* success) const
{
	if(success)
	{
		*success = false;
	}
	const QString tagName(QString(_tagName).replace("-", "_").toLower());

	TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(m_file);
	if(!file)
	{
		return defaultValue;
	}

	TagLib::APE::Tag* tag = file->APETag();
	if(!tag)
	{
		return defaultValue;
	}

	TagLib::APE::ItemListMap map = tag->itemListMap();
	TagLib::APE::ItemListMap::ConstIterator it;
	for(it = map.begin(); it != map.end(); ++it)
	{
		if(TStringToQString(it->first).toLower() == tagName)
		{
			if(success)
			{
				*success = true;
			}
			return it->second.values()[0];
		}
	}
	return defaultValue;
}

#ifndef HAVE_TAGLIB_ASF
TagLib::String TagLibTagReader::Implementation::tryASFTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const
{
	Q_UNUSED(tagName);
	if(success)
	{
		*success = false;
	}
	return defaultValue;
}
#else
TagLib::String TagLibTagReader::Implementation::tryASFTag(const QString& _tagName, const TagLib::String& defaultValue, bool* success) const
{
	if(success)
	{
		*success = false;
	}
	const QString tagName(QString(_tagName).replace("-", "_").toLower());

	TagLib::ASF::Tag* tag = dynamic_cast<TagLib::ASF::Tag*>(m_file->tag());
	if(!tag)
	{
		return defaultValue;
	}

	TagLib::ASF::AttributeListMap metaData = tag->attributeListMap();

	TagLib::ASF::AttributeListMap::ConstIterator it;
	for(it = metaData.begin(); it != metaData.end(); ++it)
	{
		if(TStringToQString(it->first).toLower() == tagName)
		{
			if(success)
			{
				*success = true;
			}
			return it->second[0].toString();
		}
	}

	return defaultValue;
}
#endif

TagLib::String TagLibTagReader::Implementation::getTag(const QString& tagName, const TagLib::String& defaultValue, bool* success) const
{
	TagLib::String retval;

	if(success)
	{
		*success = false;
	}

	// Look for Xiph Comments
	retval = tryXiphTag(tagName, defaultValue, success);	

	// Look for APE
	if(!*success)
	{
		retval = tryApeTag(tagName, defaultValue, success);
	}

	// Look for ID3v2
	if(!*success)
	{
		retval = tryID3v2Tag(tagName, defaultValue, success);
	}

	// Look for ASF Tag
	if(!*success)
	{
		retval = tryASFTag(tagName, defaultValue, success);
	}

	if(*success && !retval.isNull() && !retval.isEmpty())
	{
		return retval;
	}
	else
	{
		return defaultValue;
	}
}

void TagLibTagReader::Implementation::resolveMetaData()
{
	m_metaData.clear();
	const QString fileName(m_url.toLocalFile());
	QString fileName = QDir::toNativeSeparators(mMediaSource.fileName());
#ifdef _WIN32
	TagLib::FileRef fileRef(reinterpret_cast<const wchar_t*>(fileName.utf16()));
#else
	TagLib::FileRef fileRef(QFile::encodeName(fileName));
#endif
	if(fileRef.isNull() || !fileRef.tag())
	{
		emit(metaDataChanged());
		return;
	}

	TagLib::String temporaryString;
	uint temporaryUInt;
	bool success;
	m_file = fileRef.file();

	// Artist
	temporaryString = fileRef.tag()->artist();
	if(!(temporaryString.isNull() || temporaryString.isEmpty()))
	{
		m_metaData.insert("ARTIST", TStringToQString(temporaryString));
	}

	// Album
	temporaryString = fileRef.tag()->album();
	if(!(temporaryString.isNull() || temporaryString.isEmpty()))
	{
		m_metaData.insert("ALBUM", TStringToQString(temporaryString));
	}

	// Title
	temporaryString = fileRef.tag()->title();
	if(!(temporaryString.isNull() || temporaryString.isEmpty()))
	{
		m_metaData.insert("TITLE", TStringToQString(temporaryString));
	}

	// Track Number
	temporaryUInt = f.tag()->track();
	if(temporaryUInt)
	{
		m_metaData.insert("TRACK-NUMBER", QString::number(temporaryUInt));
	}

	// ReplayGain Album Gain
	temporaryString = getTag("REPLAYGAIN-ALBUM-GAIN", TagLib::String::null, &success);
	if(success)
	{
		m_metaData.insert("REPLAYGAIN-ALBUM-GAIN", TStringToQString(temporaryString).replace(" dB", ""));
	}

	// ReplayGain Track Gain
	temporaryString = getTag("REPLAYGAIN-TRACK-GAIN", TagLib::String::null, &success);
	if(success)
	{
		m_metaData.insert("REPLAYGAIN-TRACK-GAIN", TStringToQString(temporaryString).replace(" dB", ""));
	}

	// ArtistSort - good for unicode tracks - also good for putting The Beatles in the 'B' section
	temporaryString = getTag("artistsort", TagLib::String::null, &success);
	if(success)
	{
		m_metaData.insert("ARTIST-SORT", TStringToQString(temporaryString));
	}
	else
	{
		temporaryString = getTag("WM/ArtistSortOrder", TagLib::String::null, &success);
		if(success)
		{
			m_metaData.insert("ARTIST-SORT", TStringToQString(temporaryString));
		}
	}

	temporaryString = getTag("musicbrainz-trackid", TagLib::String::null, &success);
	if(success)
	{
		m_metaData.insert("MUSICBRAINZ-TRACK-ID", TStringToQString(temporaryString));
	emit(metaDataChanged());
}
