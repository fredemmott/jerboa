#include "DatabaseWizardPage.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QSqlDatabase>
#include <QVBoxLayout>

namespace Jerboa
{
	DatabaseWizardPage::DatabaseWizardPage(QWidget* parent)
	: WizardPage(parent)
	{
		setTitle(tr("Database"));
		QVBoxLayout* layout = new QVBoxLayout;
		setLayout(layout);
		QLabel* label = new QLabel(tr("Where would you like Jerboa to store its database? If you don't know what this means, the default is fine."), this);
		label->setWordWrap(true);
		layout->addWidget(label);

		QFormLayout* formLayout = new QFormLayout;
		layout->addLayout(formLayout);

		m_driver = new QComboBox(this);
		m_driver->addItems(QSqlDatabase::drivers());
		m_database = new QLineEdit(this);
		m_host = new QLineEdit("localhost", this);
		m_user = new QLineEdit(this);
		m_password = new QLineEdit(this);
		m_password->setEchoMode(QLineEdit::Password);

		formLayout->addRow(tr("Type:"), m_driver);
		formLayout->addRow(tr("Database:"), m_database);
		formLayout->addRow(tr("Host:"), m_host);
		formLayout->addRow(tr("User:"), m_user);
		formLayout->addRow(tr("Password:"), m_password);

		fillDefaults();
	}

	void DatabaseWizardPage::fillDefaults()
	{
		const bool sqlite = m_driver->currentText() == QLatin1String("QSQLITE");
		if(sqlite)
		{
			m_database->setText(
				QDir::toNativeSeparators(
					QString(
						"%1/collection.sqlite"
					).arg(
						QDesktopServices::storageLocation(QDesktopServices::DataLocation)
					)
				)
			);
		}
		else
		{
			m_database->setText(QCoreApplication::applicationName());
		}
		m_host->setDisabled(sqlite);
		m_user->setDisabled(sqlite);
		m_password->setDisabled(sqlite);
	}

	void DatabaseWizardPage::save()
	{
		QSettings settings;
		settings.beginGroup("database");
		settings.setValue("type", m_driver->currentText());
		settings.setValue("host", m_host->text());
		settings.setValue("name", m_database->text());
		settings.setValue("user", m_user->text());
		settings.setValue("password", m_password->text());
	}
}
