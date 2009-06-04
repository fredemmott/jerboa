#include "FirstRunWizard.h"

#include "DatabaseWizardPage.h"
#include "WizardPage.h"

#include <QCoreApplication>
#include <QLabel>
#include <QVBoxLayout>

namespace Jerboa
{
	FirstRunWizard::FirstRunWizard(const QList<Jerboa::Plugin*>& plugins, QWidget* parent)
	: QWizard(parent)
	{
		setWindowTitle(QCoreApplication::applicationName());
		addPage(createIntroductionPage());

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
		WizardPage* databasePage = new DatabaseWizardPage();
		addPage(databasePage);
	}

	QWizardPage* FirstRunWizard::createIntroductionPage()
	{
		QWizardPage* page = new QWizardPage;
		page->setFinalPage(true);
		page->setTitle(tr("Introduction"));

		QLabel* label = new QLabel("Welcome to Jerboa; there's a few settings you might want to change now, but you can just click 'Finish' at any time, and change them later from settings.", page);
		label->setWordWrap(true);
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(label);
		page->setLayout(layout);

		return page;
	}

	void FirstRunWizard::accept()
	{
		Q_FOREACH(WizardPage* page, m_pages)
		{
			page->save();
		}
		QDialog::accept();
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
