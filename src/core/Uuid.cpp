/*
* libMendeley: Assorted classes used by Mendeley Desktop
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
* Copyright (C) 2009 Mendeley Limited
*
* This version is maintained by Mendeley Limited, and most queries should
* be directed to them.
*
* Nokia contact: Qt Software Information (qt-info@nokia.com)
* Mendeley contact: Mendeley Copyright (copyright@mendeley.com)
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation, version 2.1.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1161  USA
*/
#include "Uuid.h"
#include "memcpy_safe.h"

// This needs to be included before QNetworkInterface to work around a bug
// in the Qt 4.5 headers.
#include <QPair>

#include <QByteArray>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QNetworkInterface>
#include <QString>
#include <QThread>
#include <QThreadStorage>
#include <QUuid>

#include <cstdlib>

namespace Mendeley
{
	void Uuid::nextValue(char* value, int bytes)
	{
		// progress by md5
		const QByteArray hash(QCryptographicHash::hash(QByteArray(value, bytes), QCryptographicHash::Md5));
		::memcpy_safe(value, bytes, hash.constData(), qMin(bytes, hash.count()));
	}

	// wrap seed inside a struct because QThreadStorage<> uses delete rather than
	// delete[] to free the stored value
	struct UuidSeed
	{
		char seed[16];
	};

	QString Uuid::createUuid()
	{
		// Begin section lifted from QUuid::createUuid(), modified to set a
		// different random seed for each thread - otherwise qrand() always returns the same
		// sequence of random numbers for each thread and the resulting uuids will be the same
		// when called from different threads
		//
		// See Qt Software Task Tracker Bug #171206
		// (http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=171206)
		static QThreadStorage<UuidSeed*> threadStorage;
		if(!threadStorage.hasLocalData())
		{
			UuidSeed* seedValue = new UuidSeed;
			randomSeed(seedValue->seed, 16);
			threadStorage.setLocalData(seedValue);
		}

		char* value = threadStorage.localData()->seed;
		nextValue(value, 16);
	
		struct
		{
			uint data1;
			ushort data2;
			ushort data3;
			uchar data4[8];
		} parts;
	
		uint *data = &(parts.data1);
		::memcpy_safe(data, sizeof(parts), value, 16);
	
		parts.data4[0] = (parts.data4[0] & 0x3F) | 0x80;		// UV_DCE
		parts.data3 = (parts.data3 & 0x0FFF) | 0x4000;		// UV_Random
		// End Section lifted from QUuid::createUuid()
	
		QUuid uuid(parts.data1,parts.data2,parts.data3,
				   parts.data4[0],parts.data4[1],parts.data4[2],
				   parts.data4[3],parts.data4[4],parts.data4[5],
				   parts.data4[6],parts.data4[7]);
	
		return uuid.toString();
	}
	
	void Uuid::randomSeed(char* seed, int bytes)
	{
		Q_ASSERT(bytes == 16);
		// Seed based on:
		// - 'mendeley' (salt)
		// - date and time
		// - PID
		// - thread
		// - all MAC addresses
		QByteArray data;
		{
			QDataStream stream(&data, QIODevice::WriteOnly);
			stream << "mendeley";
			stream << QDateTime::currentDateTime();
			stream << QCoreApplication::applicationPid();
			stream << reinterpret_cast<qint64>(QThread::currentThread());
			Q_FOREACH(const QNetworkInterface& interface, QNetworkInterface::allInterfaces())
			{
				stream << interface.hardwareAddress() << interface.name();
			}
		}
		const QByteArray hash(QCryptographicHash::hash(data, QCryptographicHash::Md5));
		::memcpy_safe(seed, bytes, hash.constData(), qMin(bytes, hash.count()));
	}
}
