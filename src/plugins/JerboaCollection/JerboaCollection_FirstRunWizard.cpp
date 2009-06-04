#include "JerboaCollection_FirstRunWizard.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QVBoxLayout>

JerboaCollection::FirstRunWizard::FirstRunWizard(QWidget* parent)
: Jerboa::WizardPage(parent)
, m_defaultLocation(QDir::toNativeSeparators(QDesktopServices::storageLocation(QDesktopServices::MusicLocation)))
{
	setTitle(tr("Your Collection"));

	QLabel* label = new QLabel(tr("Where do you keep your music?"), this);

	m_path = new QLineEdit(m_defaultLocation, this);

	QPushButton* button = new QPushButton(
		style()->standardIcon(QStyle::SP_DirIcon),
		QString(),
		this
	);
	button->setToolTip(tr("Find Directory"));

	QHBoxLayout* entryLayout = new QHBoxLayout();
	entryLayout->addWidget(m_path);
	entryLayout->addWidget(button);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(label);
	mainLayout->addLayout(entryLayout);
	setLayout(mainLayout);

	connect(
		button,
		SIGNAL(clicked()),
		SLOT(browseForDirectory())
	);
}

void JerboaCollection::FirstRunWizard::browseForDirectory()
{
	const QString path = QFileDialog::getExistingDirectory(
		this,
		tr("Music Location"),
		QDesktopServices::storageLocation(QDesktopServices::MusicLocation)
	);
	if(!path.isEmpty())
	{
		m_path->setText(QDir::toNativeSeparators(path));
	}
}

void JerboaCollection::FirstRunWizard::save()
{
	QSettings settings;
	const QDir dir(QDir::fromNativeSeparators(m_path->text()));
	if(dir.exists())
	{
		settings.setValue("collection/directory", dir.absolutePath());
	}
}
