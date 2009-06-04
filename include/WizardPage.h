#pragma once

#include <QWizardPage>

namespace Jerboa
{
	class WizardPage : public QWizardPage
	{
		Q_OBJECT;
		public:
			WizardPage(QWidget* parent);
			virtual void save() = 0;
	};
};
