/* LICENSE NOTICE
	ScopedCfPointer: Like std::auto_ptr, but for CFRefs (calls CFRelease)
	Copyright (C) 2009 Mendeley Ltd.  <fred.emmott@mendeley.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option), any later version.

	Jerboa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _SCOPED_CF_POINTER_H
#define _SCOPED_CF_POINTER_H

#ifdef qDebug
#define Q_DEBUG qDebug
#undef qDebug
#endif
#include <Carbon/Carbon.h>
#ifdef Q_DEBUG
#define qDebug Q_DEBUG
#endif

template <class T> class ScopedCfPointer
{
	public:
		ScopedCfPointer(T reference)
			: d(reference)
		{
		}

		void operator=(T data)
		{
			d = data;
		}

		ScopedCfPointer()
			: d(0)
		{
		}

		operator T() const
		{
			return d;
		}

		~ScopedCfPointer()
		{
			if(d)
			{
				CFRelease(d);
			}
		}

		T& data()
		{
			return d;
		}
	private:
		T d;
};

#endif
