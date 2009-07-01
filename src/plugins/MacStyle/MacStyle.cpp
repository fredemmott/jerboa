#include "MacStyle.h"
#include "MacStyle_Implementation.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>

MacStyle::MacStyle()
	:
		QObject(0)
{
}

QString MacStyle::pluginName() const
{
	return tr("Mac OSX Styling");
}

QString MacStyle::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString MacStyle::uniqueId() const
{
	return "org.jerboaplayer.MacStyle";
}

void MacStyle::addComponent(ComponentType type, QObject* component)
{
	switch(type)
	{
		case Jerboa::Plugin::Player:
			QApplication::setStyle(new Implementation());
			return;
		default:
			Plugin::addComponent(type, component);
	}
}

Q_EXPORT_PLUGIN2(Jerboa_MacStyle, MacStyle);
