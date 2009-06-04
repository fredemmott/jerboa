#include "FirstRunWizard.h"

#include "DatabaseWizardPage.h"
#include "WizardPage.h"

#include <QCoreApplication>

namespace Jerboa
{
	FirstRunWizard::FirstRunWizard(const QList<Jerboa::Plugin*>& plugins, QWidget* parent)
	: QWizard(parent)
	{
		setWindowTitle(QCoreApplication::applicationName());

		QList<Plugin*> sortedPlugins(plugins);
		qSort(sortedPlugins.begin(), sortedPlugins.end(), pluginComesBefore);
		Q_FOREACH(Plugin* plugin, sortedPlugins)
		{
			if(plugin->components().contains(Plugin::FirstRunWizardPage))
			{
				WizardPage* page = qobject_cast<WizardPage*>(plugin->component(Plugin::FirstRunWizardPage, 0));
				Q_ASSERT(page);
				if(page)
				{
					addPage(page);
				}
			}
		}
		addPage(new DatabaseWizardPage());
	}

	const bool FirstRunWizard::pluginComesBefore(Plugin* a, Plugin* b)
	{
		QList<Plugin::ComponentType> aTypes = a->components().toList();
		QList<Plugin::ComponentType> bTypes = b->components().toList();
		if(aTypes.isEmpty())
		{
			return false;
		}
		if(bTypes.isEmpty())
		{
			return true;
		}
		qSort(aTypes);
		qSort(bTypes);
		return aTypes.first() < bTypes.first();
	}
};
