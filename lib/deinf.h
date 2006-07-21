/* This file is part of Kiten, a KDE Japanese Reference Tool...

 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2006  Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef DEFINF_H
#define DEFINF_H

#include <qstring.h>

class /* NO_EXPORT */ deinf
{
	public:
		deinf(); 
		
		//Actually attempt a deinflection on target. name is an empty list
		//that will contain a list of explanations for the changes given in
		//the return value... note that the first returned value SHOULD be 
		//the best option in most cases.
		QStringList deinflect(const QString &target, QStringList &name);

	private:
		bool load();

};


#endif
