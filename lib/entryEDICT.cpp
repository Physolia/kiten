/***************************************************************************
 *  Copyright  (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
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

#include "entryEDICT.h"

/* DISPLAY FUNCTIONS */

/** returns a brief HTML version of an Entry */
QString EntryEDICT::toBriefHTML() const
{
	return "<div class=\"EDICTBrief\">" +HTMLWord() + " " + HTMLReadings() +
	  " "+ HTMLMeanings() + "</div>"; 
}

/** returns an HTML version of an Entry that is rather complete.*/
QString EntryEDICT::toVerboseHTML() const
{
	return "<div class=\"EDICTVerbose\">" + HTMLWord() + " " + HTMLReadings() +
	  " " + HTMLMeanings() + "</div>"; 
}

/** Makes a link out of each kanji in @p inString */
inline QString EntryEDICT::kanjiLinkify(const QString &inString) const
{
	QString outString;
	int i;

	for(i = 0; i < inString.length(); i++)
	{
		if(isKanji(inString.at(i)))
		{
			outString += makeLink(inString.at(i));
		}
		else
		{
			outString += inString.at(i);
		}
	}
	
	return outString;
}

inline QString EntryEDICT::HTMLWord() const {
	return "<span class=\"Word\">"+
		( Word.isEmpty()?kanjiLinkify(Meanings.first()):kanjiLinkify(Word) ) +
		"</span>";
}

/* DATA LOADING FUNCTIONS */

/* TODO: extendedInfo as described on Breen's site
	http://www.csse.monash.edu.au/~jwb/edict_doc.html */
/** Take a QString and load it into the Entry as appropriate */
bool EntryEDICT::loadEntry(const QString &entryLine)
{
	/* Check the requirements of validity first */
	/* EDICT requires at least two '/' marks and a ' ' */
	if(entryLine.count("/") < 2 || entryLine.at(0)==' ') //KDE4 CHANGE
	{
		kDebug() << "EDICT Parser recieved bad data! : "<<entryLine<<endl;
		return false;
	}
	

	/* Set tempQString to be the reading and word portion of the entryLine */
	QString tempQString = entryLine.left(entryLine.indexOf('/'));
	/* The actual Word is the beginning of the line */
	Word = tempQString.left(tempQString.indexOf(' '));

	/* The Reading is either Word or encased in '[' */
	Readings.clear();
	int startOfReading = tempQString.indexOf('[');
	if(startOfReading != -1)  // This field is optional for kiten
		Readings.append(
				tempQString.left(tempQString.lastIndexOf(']')).mid(startOfReading+1));

	/* set Meanings to be all of the meanings in the definition */
	Meanings = entryLine.mid(tempQString.length()).split('/');

//	kdDebug()<< "Parsed: '"<<Word<<"' ("<<Readings.front()<<") \""<<
//		Meanings.join("|")<<"\" from :"<<entryLine<<endl;
	return true;
}

/** Regenerate a QString like the one we got in loadEntry() */
inline QString EntryEDICT::dumpEntry() const
{
	return Word + 
		((Readings.count() == 0) ? " " : " [" + Readings.first() + "] ") 
		+ "/" + Meanings.join("/") + "/";
}

