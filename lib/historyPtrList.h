/***************************************************************************
 *   Copyright (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
 *             (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __HISTORY_PTR_LIST_H__
#define __HISTORY_PTR_LIST_H__

#include <q3ptrlist.h>
#include <qstringlist.h>
#include "entry.h"


class historyPtrList : protected Q3PtrList<EntryList> {
	public:
		/** Construct a historyPtrList, this should be done early on */
		historyPtrList();
		/** Add an item to the end of the history list and set it as
		  the current displayed item */
		void addItem(EntryList*);
		/** Return a list of the entries. Note that this is usually
		  just a QStringList of all of the EntryList's dictQuery->toString() 
		  calls. */
		QStringList toStringList();
		/** Return a list of the entries prior to the current one (not including
		  the current entry */
		QStringList toStringListPrev();
		/** Return a summary list that only includes those after the current */
		QStringList toStringListNext();
		/** Add one to the current location, convenient for 'forward' buttons */
		void next(int distance = 1);
		/** Sub one from the current location, the counterpart to next() */
		void prev(int distance = 1);
		/** Return the current location, using a 0 based index */
		int at() {return Q3PtrList<EntryList>::at();}
		/** Return the item at the location given by the param, and set it
		  to be the current history list item */
		EntryList *at(uint);
		/** Return the current item */
		EntryList *current() {return Q3PtrList<EntryList>::current();}
		/** Return the total number of items in the list */
		int count() {return Q3PtrList<EntryList>::count();}
};

#endif

