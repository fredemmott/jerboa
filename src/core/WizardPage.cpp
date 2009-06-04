#include "WizardPage.h"

namespace Jerboa
{
	WizardPage::WizardPage(QWidget* parent)
	: QWizardPage(parent)
	{
		setFinalPage(true);
	}
}
