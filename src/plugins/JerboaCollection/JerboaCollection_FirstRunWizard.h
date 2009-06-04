#pragma once

#include "JerboaCollection.h"
#include "WizardPage.h"

class QLineEdit;

class JerboaCollection::FirstRunWizard : public Jerboa::WizardPage
{
	Q_OBJECT;
	public:
		FirstRunWizard(QWidget* parent);
		void save();
	private:
		const QString m_defaultLocation;
		QLineEdit* m_path;
};
