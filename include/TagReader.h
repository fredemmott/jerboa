#ifndef _JERBOA_TAG_READER_H
#define _JERBOA_TAG_READER_H

#include "TrackData.h"

#include <QObject>
#include <QUrl>

namespace Jerboa
{
	class TagReader : public QObject
	{
		Q_OBJECT;
		public:
			virtual ~TagReader();
			virtual void readUrl(const QUrl& url) = 0;
		signals:
			void notFound(const QUrl& url);
			void schemeNotSupported(const QUrl& url);
			void finished(const Jerboa::TrackData& track);
		protected:
			TagReader(QObject* parent);
	};
};

#endif
