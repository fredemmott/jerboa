#pragma once

#include "Plugin.h"

#include <QList>
#include <QWizard>

namespace Jerboa
{
	class WizardPage;

	class FirstRunWizard : public QWizard
	{
		Q_OBJECT;
		public:
			FirstRunWizard(const QList<Plugin*>& plugins, QWidget* parent = 0);
			virtual void accept();
		private:
			static const bool pluginComesBefore(Plugin* a, Plugin* b);
//			QWizardPage* createIntroductionPage();
			QList<WizardPage*> m_pages;
	};
}
