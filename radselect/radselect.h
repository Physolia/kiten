/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#ifndef RADSELECT_H
#define RADSELECT_H

#include "dictquery.h"

#include <KXmlGuiWindow>

class QDBusInterface;
class RadSelectView;

/**
 * This class serves as the main window for radselect.
 * It handles the menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Joseph Kerian <jkerian@gmail.com>
 * @version 0.1
 */
class RadSelect : public KXmlGuiWindow
{
  Q_OBJECT

  public:
    RadSelect();
    virtual ~RadSelect();

   /**
    * This loads a string for a given query into the UI
    */
    void loadSearchString( const QString &searchString );

  protected:
    // Overridden virtuals for Qt drag 'n drop (XDND)
    virtual void dragEnterEvent( QDragEnterEvent *event );
    virtual void dropEvent( QDropEvent *event );

  protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties( KConfigGroup &config );

    /**
     * This function is called when this app is restored. The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties( const KConfigGroup &config );


  private slots:
    void optionsPreferences();

    void changeStatusbar( const QString &text );

    void sendSearch( const QStringList &kanji );

  private:
    QDBusInterface *m_dbusInterface;
    RadSelectView  *m_view;
    DictQuery       m_currentQuery;
};

#endif
