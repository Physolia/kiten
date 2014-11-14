/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#include <KAboutData>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include <QApplication>
#include <QCommandLineParser>
#include <QDBusConnection>

#include "kiten.h"

void noMessageOutput( QtMsgType, const char* )
{
}

int main( int argc, char *argv[] )
{
//   qInstallMsgHandler(noMessageOutput);	//Disable Qt Errors from showing

  // Copyright and author information
  Kdelibs4ConfigMigrator migrate(QLatin1String("kiten"));
  migrate.setConfigFiles(QStringList() << QLatin1String("kitenrc"));
  migrate.setUiFiles(QStringList() << QLatin1String("kitenui.rc"));
  migrate.migrate();

  KAboutData aboutData( QStringLiteral("kiten"), i18n("Kiten"), QStringLiteral("1.3"), i18n("Japanese Reference Tool"),
                  KAboutLicense::GPL_V2, i18n("(c) 2001-2004, Jason Katz-Brown\n"
                                                    "(c) 2006-2007, Eric Kjeldergaard\n"
                                                    "(c) 2006-2008, Joseph Kerian\n"
                                                    "(c) 2011, Daniel E. Moctezuma"),
                  QString(), "http://edu.kde.org/kiten" );

  aboutData.addAuthor( i18n("Jason Katz-Brown"), i18n("Original author"), "jason@katzbrown.com" );
  aboutData.addCredit( i18n("Jim Breen"),        i18n("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\nAlso is main author of edict and kanjidic, which Kiten essentially require."), "jwb@csse.monash.edu.au" );
  aboutData.addAuthor( i18n("Neil Stevens"),     i18n("Code simplification, UI suggestions."), "neil@qualityassistant.com" );
  aboutData.addCredit( i18n("David Vignoni"),    i18n("svg icon"), "david80v@tin.it");
  aboutData.addCredit( i18n("Paul Temple"),      i18n("Porting to KConfig XT, bug fixing"), "paul.temple@gmx.net" );
  aboutData.addAuthor( i18n("Joseph Kerian"),    i18n("KDE4 rewrite"), "jkerian@gmail.com" );
  aboutData.addAuthor( i18n("Eric Kjeldergaard"), i18n("KDE4 rewrite"), "kjelderg@gmail.com" );
  aboutData.addAuthor( i18n("Daniel E. Moctezuma"), i18n("Deinflection system improvements, Dictionary updates for EDICT and KANJIDIC, GUI Improvements, Kanji Browser, Bug fixes, Code polishing and simplification"), "democtezuma@gmail.com" );

  aboutData.setOrganizationDomain( "kde.org" ); //Set this for the DBUS ID
  QApplication app(argc, argv);
  QCommandLineParser parser;
  KAboutData::setApplicationData(aboutData);

  parser.addVersionOption();
  parser.addHelpOption();
  aboutData.setupCommandLine(&parser);
  parser.process(app);
  aboutData.processCommandLine(&parser);

  Kiten *kiten = new Kiten();

  //Register the DBUS name or die
  if ( ! QDBusConnection::sessionBus().registerService( "org.kde.kiten" ) )
  {
    exit( 1 );
  }

  kiten->show();
  QDBusConnection::sessionBus().registerObject( "/", kiten, QDBusConnection::ExportAllSlots );
  return app.exec();
}
