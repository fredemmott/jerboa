#pragma once

#include "Scrobbler.h"
#include "WizardPage.h"

class QLineEdit;

class Scrobbler::FirstRunWizard : public Jerboa::WizardPage
{
	Q_OBJECT;
	public:
		FirstRunWizard(QWidget* parent);
		void save();
	private:
		QLineEdit* m_user;
		QLineEdit* m_password;
};
