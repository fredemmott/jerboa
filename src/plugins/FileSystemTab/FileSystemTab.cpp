#include "FileSystemTab.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileSystemModel>
#include <QSettings>
#include <QTimer>
#include <QTreeView>
#include <QtPlugin>

FileSystemTab::FileSystemTab(QObject* parent)
	:
		QObject(parent),
		m_playlist(0)
{
}

void FileSystemTab::addComponent(ComponentType type, QObject* component)
{
	if(type == PlaylistSource)
	{
		m_playlist = qobject_cast<Jerboa::PlaylistInterface*>(component);
		Q_ASSERT(m_playlist);
	}
}

QObject* FileSystemTab::component(Jerboa::Plugin::ComponentType type, QObject* parent)
{
	switch(type)
	{
		case Jerboa::Plugin::WidgetUsedWithPlaylist:
			{
				QFileSystemModel* model = new QFileSystemModel(parent);
				model->setRootPath("/");
				model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
				model->setNameFilterDisables(false);

				QStringList musicGlobs;
				musicGlobs
					<< "*.aac"
					<< "*.aiff"
					<< "*.ape"
					<< "*.au"
					<< "*.cdda"
					<< "*.flac"
					<< "*.m4a"
					<< "*.mp3"
					<< "*.oga"
					<< "*.ogg"
					<< "*.ogm"
					<< "*.wav"
					<< "*.wma"
				;
				model->setNameFilters(musicGlobs);

				QTreeView* view = new QTreeView(qobject_cast<QWidget*>(parent));
				view->setWindowTitle("Files");
				view->setModel(model);
				view->setHeaderHidden(true);
				for(int i = 1; i < model->columnCount(); ++i)
				{
					view->setColumnHidden(i, true);
				}

				QString path = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);

				if(path.isEmpty() || !QDir(path).exists())
				{
					path = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
				}

				const QModelIndex index(model->index(QSettings().value("collection/directory", path).toString()));
				for(QModelIndex iterator(index); iterator.isValid(); iterator = iterator.parent())
				{
					view->setExpanded(iterator, true);
				}

				view->setDragDropMode(QAbstractItemView::DragOnly);
				view->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
				view->setSelectionMode(QAbstractItemView::ExtendedSelection);

				connect(
					view,
					SIGNAL(doubleClicked(QModelIndex)),
					this,
					SLOT(addPathToPlaylist(QModelIndex))
				);

				QTimer* timer = new QTimer(this);
				connect(
					timer,
					SIGNAL(timeout()),
					this,
					SLOT(scrollToSelection())
				);
				timer->setSingleShot(true);
				timer->start(1000);

				m_view = view;
				return view;
			}
		default:
			return Jerboa::Plugin::component(type, parent);
	}
}

void FileSystemTab::addPathToPlaylist(const QModelIndex& index)
{
	Q_ASSERT(m_playlist);
	const QString path = index.data(QFileSystemModel::FilePathRole).toString();
	if(!path.isEmpty())
	{
		m_playlist->insertTracks(-1, QList<QUrl>() << QUrl::fromLocalFile(path));
	}
}

void FileSystemTab::scrollToSelection()
{
	const QModelIndexList selectedIndexes(m_view->selectionModel()->selectedIndexes());
	if(!selectedIndexes.isEmpty())
	{
		m_view->scrollTo(selectedIndexes.first(), QAbstractItemView::PositionAtTop);
	}
}

QString FileSystemTab::pluginName() const
{
	return tr("File-system Tab");
}

QString FileSystemTab::pluginAuthor() const
{
	return QString::fromLatin1("Fred Emmott");
}

QString FileSystemTab::uniqueId() const
{
	return "org.jerboaplayer.FileSystemTab";
}

QSet<Jerboa::Plugin::ComponentType> FileSystemTab::components() const
{
	return QSet<Jerboa::Plugin::ComponentType>() << Jerboa::Plugin::WidgetUsedWithPlaylist;
}

Q_EXPORT_PLUGIN2(Jerboa_FileSystemTab, FileSystemTab);
