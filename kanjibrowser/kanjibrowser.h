/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
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

#ifndef KANJIBROWSER_H
#define KANJIBROWSER_H

#include <KXmlGuiWindow>

class DictFileKanjidic;
class KanjiBrowserConfigSkeleton;
class KanjiBrowserView;

class KanjiBrowser : public KXmlGuiWindow
{
  friend class KanjiBrowserView;

  Q_OBJECT

  public:
         KanjiBrowser();
        ~KanjiBrowser();

  private slots:
    void changeStatusBar( const QString &text );
    void showPreferences();

  private:
    void loadKanji();

    KanjiBrowserConfigSkeleton *_config;
    KanjiBrowserView           *_view;
    DictFileKanjidic           *_dictFileKanjidic;
    QStringList                 _kanjiList;
    int                         _maxStrokeCount;
};

#endif
