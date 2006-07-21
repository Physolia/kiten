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

#ifndef ENTRYKANJIDIC__
#define ENTRYKANJIDIC__

#include "entry.h"

class QString;

class /* NO_EXPORT */ EntryKanjidic : public Entry {
	public:
		EntryKanjidic(): Entry() {}
		EntryKanjidic(const EntryKanjidic &x) : Entry(x) {}
		EntryKanjidic(const QString &x) : Entry(x) {}
		EntryKanjidic(const QString &dict, const QString &entry) : Entry(dict) {loadEntry(entry);}
		Entry *clone() const {return new EntryKanjidic(*this); }
		
		virtual QString toBriefHTML() const;
		virtual QString toVerboseHTML() const;

		virtual bool matchesQuery(const dictQuery&) const;
		
		virtual bool loadEntry(const QString &);
		virtual QString dumpEntry() const;
		
	protected:
		virtual inline QString HTMLReadings() const;
		virtual inline QString HTMLWord() const;
		virtual QString makeReadingLink(const QString&) const;
};

#endif
