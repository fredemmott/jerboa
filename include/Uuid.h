/*
* libMendeley: Assorted classes used by Mendeley Desktop
* Copyright (C) 2009 Mendeley Limited <copyright@mendeley.com>

* Mendeley contact: Mendeley Copyright (copyrightmendeley.com)
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; version 2.1.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _MENDELEY_UUID_H
#define _MENDELEY_UUID_H

#include <QAtomicInt>

namespace Mendeley
{
	/** Alternative to QUuid which makes UUIDs that are less likely to have collisions.
	 *
	 * QUuid is purely based on date and time.
	 *
	 * This class takes into account:
	 *  - The current date and time
	 *  - The application PID
	 *  - The current thread
	 *  - The MAC addresses of all interfaces on the local host
	 *
	 * Not as fast, but safer if you have multiple clients which need to have non-colliding UUIDs.
	 *
	 * @author Fred Emmott <fred.emmott@mendeley.com>
	 */
	class Uuid
	{
		public:
			/** Alternative to QUuid::createUuid() which works when used across
			 * multiple threads simultaneously.
			 */
			static QString createUuid();
		private:
			/// Random seed for the first call to this function per thread.
			static void randomSeed(char* seed, int bytes);
			static void nextValue(char* value, int bytes);
	};
}

#endif
