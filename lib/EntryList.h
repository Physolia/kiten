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

#ifndef KITEN_ENTRYLIST_H
#define KITEN_ENTRYLIST_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

#include "libkitenexport.h"
#include "DictQuery.h"
#include "Entry.h"

/** EntryList is a simple container for Entry objects, and is-a QList<Entry*>
 * A few simple overrides allow you to deal with sorting and translating */
class KITEN_EXPORT EntryList : public QList<Entry*> {

public:
	/** A simple overridden iterator for working with the Entries */
	typedef QListIterator<Entry*> EntryIterator;

	/** Basic constructor, create an empty EntryList */
	EntryList();
	/** Copy constructor */
	EntryList(const EntryList &);
	/** Basic Destructor, does not delete Entry* objects. Please remember to call
	 * deleteAll() before deleting an EntryList */
	virtual ~EntryList();
	/** Delete all Entry objects in our list. In the future, we'll switch to a reference
	 * counting system, and this will be deprecated */
	void deleteAll();

	/** Convert every element of the EntryList to a QString and return it
	 * */
	QString toString(Entry::printType=Entry::printAuto) const;
	/** Convert every element of the EntryList to a QString in HTML form and return it */
	QString toHTML(Entry::printType=Entry::printAuto) const;

	/** Convert a given range of the EntryList to a QString and return it
	 * @param start the location in the list where we should start
	 * @param length the length of the list we should generate */
	QString toString(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	/** Convert a given range of the EntryList to a QString in HTML form and return it
	 * @param start the location in the list where we should start
	 * @param length the length of the list we should generate  */
	QString toHTML(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	/** Convert the entire list to KVTML for export to a flashcard app
	 * @param start the location in the list where we should start
	 * @param length the length of the list we should generate */
	QString toKVTML(unsigned int start, unsigned int length) const;

	/** Sort the list according to the given fields in sortOrder, if dictionaryOrder
	 * is blank, don't order the list by dictionary, otherwise items are sorted by dictionary
	 * then by sortOrder aspects
	 * @param sortOrder the keys to sort by, see Entry::sort
	 * @param dictionaryOrder the order for the Entry objects to be sorted in, dictionary-wise */
	void sort(QStringList &sortOrder,QStringList &dictionaryOrder);

	/** Append another EntryList onto this one */
	const EntryList& operator+=(const EntryList &);
	/** Append another EntryList onto this one */
	void appendList(const EntryList *);
	/** Get the query that generated this list, note that if you have appended EntryLists from
	 * two different queries, the resulting DictQuery from this is undefined */
	DictQuery getQuery() const;
	/** Set the query for this list.  */
	void setQuery(const DictQuery&);

protected:
	/** The query that this list was generated from.
	 * Note that if this list was the result of a merge (using operator+=) of two or more EntryList objects,
	 * the value of this is undefined */
	DictQuery query;

private:
	/** An easy function that returns the html result for an empty list */
	inline QString noResultsHTML();
};

#endif
