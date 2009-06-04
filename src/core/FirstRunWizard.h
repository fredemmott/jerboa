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
		private:
			static const bool pluginComesBefore(Plugin* a, Plugin* b);
//			QWizardPage* createIntroductionPage();
			QList<Jerboa::WizardPage*> m_pages;
	};
}
