/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "dictfiledeinflect.h"

#include "../entrylist.h"
#include "entrydeinflect.h"
#include "../DictEdict/dictfileedict.h"
#include "../DictEdict/entryedict.h"

#include <KDebug>
#include <KLocale>
#include <KMessageBox>
#include <KStandardDirs>

#include <QFile>
#include <QHash>
#include <QList>
#include <QRegExp>
#include <QString>
#include <QTextCodec>
#include <QTextStream>

//This is a very primative form of information hiding
//But C++ can get stupid with static QT objects...
//So this turns out to be much, much easier
//TODO: Fix this for thread safety/functionality (I'm presuming it's broken atm)

//Declare our constants
QList<DictFileDeinflect::Conjugation> *DictFileDeinflect::conjugationList = NULL;

DictFileDeinflect::DictFileDeinflect()
: DictFile( "Deinflect" )
{
}

EntryList* DictFileDeinflect::doSearch( const DictQuery &query )
{
  return NULL;
  if ( conjugationList == NULL )
    return NULL;

  QString text = query.getWord();
  if( text.isEmpty() )
  {
    text = query.getPronunciation();
  }
  if( text.isEmpty() )
  {
    return NULL;
  }

  EntryList *ret = new EntryList;
  int index = 0;
  foreach( const Conjugation &it, *conjugationList )
  {
    if( text.endsWith( it.ending ) )
    {
      QString replacement = text;
      replacement.truncate( text.length() - it.ending.length() );
      replacement += it.replace;
      EntryDeinflect *foo = new EntryDeinflect( replacement, it.label, index++, it.ending );
      ret->append( foo );

      //if( ret->count() >= 3 )
      //{
        //return ret;
      //}
    }
  }

  // Check if the entry was a verb, if so, find the tense the user gave us
  // and return the verb in dictionary form.
  //QVector<QString> edictResults = DictFileEdict::getSearchResults();
  //EntryList *results = new EntryList;
  //kDebug() << endl << endl;
  //foreach( const QString result, edictResults )
  //{
    //if( result.contains( "v5" ) )
    //{
        //EntryDeinflect *entry = new EntryDeinflect( result );
        //kDebug() << result << endl;
        //results->append( entry );
     //}
   //}

   //kDebug() << endl;

  return ret;
  //return results;
}

QStringList DictFileDeinflect::listDictDisplayOptions( QStringList orig ) const
{
  return QStringList( "Deinflection" );
}

bool DictFileDeinflect::loadDictionary( const QString &file, const QString &name )
{
  if ( conjugationList != NULL )
    return true;

  conjugationList = new QList<Conjugation>;

  QString vconj;
  if( file.isEmpty() )
  {
    KStandardDirs *dirs = KGlobal::dirs();
    vconj = dirs->findResource( "data", "kiten/vconj" );

    //Find the file
    if ( vconj.isEmpty() )
    {
      KMessageBox::error( 0, i18n( "Verb deinflection information not found, so verb deinflection cannot be used." ) );
      return false;
    }
  }
  else
  {
    vconj = file;
  }

  QHash<unsigned long,QString> names;
  //Open the file
  QFile f( vconj );
  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    KMessageBox::error( 0, i18n( "Verb deinflection information could not be loaded, so verb deinflection cannot be used." ) );
    return false;
  }

  QTextStream t( &f );
  t.setCodec( QTextCodec::codecForName( "eucJP" ) );

  //The file starts out with a number -> name list of the conjugation types
  //In the format "#[#].\tNAME\n"
  //The next section beginning is flagged with a $ at the beginning of the line
  for( QString text = t.readLine(); ! t.atEnd() && text.at( 0 ) != '$';
            text = t.readLine() )
  {
    if( text.at( 0 ) != '#' )
    {
      unsigned long number = text.left( 2 ).trimmed().toULong();
      QString name = text.right( text.length() - 2 ).trimmed();
      names[ number ] = name;
    }
  }

  //Now for the actual conjugation data
  //Format is "ENDING_TO_REPLACE\tREPLACEMENT\tNUMBER_FROM_LIST_ABOVE\n"
  for( QString text = t.readLine(); ! text.isEmpty(); text = t.readLine() )
  {
    if( text.at(0) != '#' ) //We still have these comments everywhere
    {
      QStringList things( text.split( QChar( '\t' ) ) );

      Conjugation conj;
      conj.ending = things.first();
      conj.replace = ( things.at( 1 ) );
      conj.label = names.find( things.last().toULong() ).value();

      conjugationList->append( conj );
    }
  }

  f.close();

  m_dictionaryName = name;

  return true;
}

bool DictFileDeinflect::validDictionaryFile( const QString &filename )
{
  return false;
}

bool DictFileDeinflect::validQuery( const DictQuery &query )
{
  return true;
}
