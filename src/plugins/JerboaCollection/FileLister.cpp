/* LICENSE NOTICE
	This file is part of Jerboa.

	Jerboa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Jerboa.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FileLister.h"

#include <QDebug>
#include <QtConcurrentRun>
#include <QFileInfo>

FileLister::FileLister(QObject* parent) : QObject(parent) {}

void FileLister::start(const QDir& dir)
{
	m_files.clear();
	m_cancelled = false;
	QtConcurrent::run(this, &FileLister::findFiles, dir, true);
	//findFiles(dir, true);
}

void FileLister::cancel()
{
	m_cancelled = true;
}

void FileLister::findFiles(const QDir& dir, bool emitDone)
{
	QFileInfoList contents = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	Q_FOREACH(const QFileInfo& entry, contents)
	{
		if(m_cancelled)
		{
			break;
		}

		if(entry.isDir())
		{
			findFiles(QDir(entry.absoluteFilePath()), false);
		}
		else if(entry.isReadable())
		{
			m_files.append(entry.absoluteFilePath());
		}
	}
	if(emitDone)
	{
		emit finished(m_files);
	}
}
