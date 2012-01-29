#ifndef _TAG_LIB_TAG_READER_H
#define _TAG_LIB_TAG_READER_H

#include "JerboaPlugin.h"
#include "TagReader.h"

#include <QObject>

class TagLibTagReader: public Jerboa::TagReader, public Jerboa::Plugin
{
	Q_OBJECT;
	Q_INTERFACES(Jerboa::Plugin);
	public:
		TagLibTagReader(QObject* parent = 0);

		// TagReader
		virtual void readUrl(const QUrl& url);
		virtual Jerboa::TagReader* instance(QObject* parent) const;

		// Plugin
		QString pluginName() const;
		QString pluginAuthor() const;
		QString uniqueId() const;

		QSet<Jerboa::Plugin::ComponentType> components() const;
		QObject* component(ComponentType, QObject* parent);
	private slots:
		void metaDataChanged();
	private:
		class Implementation;
		Implementation* d;
};

#endif
