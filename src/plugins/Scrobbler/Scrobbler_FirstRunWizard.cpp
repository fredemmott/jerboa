#include "Scrobbler_FirstRunWizard.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QVBoxLayout>

Scrobbler::FirstRunWizard::FirstRunWizard(QWidget* parent)
: Jerboa::WizardPage(parent)
{
	setTitle(QString::fromLatin1("last.fm"));
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Scrobbler/lastfm.png"));

	QLabel* label = new QLabel(
		tr("If you use <a href='http://www.last.fm'>last.fm</a>, enter your account details below so that Jerboa can scrobble the tracks you listen to."),
		this
	);
	label->setWordWrap(true);
	label->setOpenExternalLinks(true);

	m_user = new QLineEdit(this);
	m_password = new QLineEdit(this);
	m_password->setEchoMode(QLineEdit::Password);

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addWidget(label);

	QFormLayout* formLayout = new QFormLayout();
	layout->addLayout(formLayout);

	formLayout->addRow(tr("Username:"), m_user);
	formLayout->addRow(tr("Password:"), m_password);

	setLayout(layout);
}

void Scrobbler::FirstRunWizard::save()
{
	QSettings settings;
	settings.beginGroup("lastfm");
	settings.setValue("user", m_user->text());
	settings.setValue("password", m_password->text());
}
