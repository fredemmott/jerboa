#pragma once

#include "WizardPage.h"

class QComboBox;
class QLineEdit;

namespace Jerboa
{
	class DatabaseWizardPage : public WizardPage
	{
		Q_OBJECT;
		public:
			DatabaseWizardPage(QWidget* parent = 0);
			void save();
		private slots:
			void fillDefaults();
		private:
			QComboBox* m_driver;
			QLineEdit* m_database;
			QLineEdit* m_host;
			QLineEdit* m_user;
			QLineEdit* m_password;
	};
};
