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

#ifndef KANJIBROWSERVIEW_H
#define KANJIBROWSERVIEW_H

#include "ui_kanjibrowserview.h"

class KanjiBrowserView : public QWidget, private Ui::KanjiBrowserView
{
  Q_OBJECT

  public:
         KanjiBrowserView( QWidget *parent );
        ~KanjiBrowserView();

    void setupView(   const QHash< QString, QPair<int, int> > &kanji
                    , QList<int> &kanjiGrades
                    , QList<int> &strokeCount );

  private slots:
    void changeGrade( const int grade );
    void changeStrokeCount( const int strokes );

  private:
    void reloadKanjiList();

    QHash< QString, QPair<int, int> > _kanji;
    QList<int>                        _gradeList;
    QList<int>                        _strokesList;
    QList<int>                        _currentGradeList;
    QList<int>                        _currentStrokesList;
};

#endif
