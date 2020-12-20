/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADICAL_H
#define RADICAL_H

#include <QSet>
#include <QString>

class Radical : public QString
{
  public:
                         Radical();
    explicit             Radical(  const QString &irad
                                 , unsigned int strokes = 0 );

    const QSet<QString>& getKanji() const;
    void                 addKanji( const QSet<QString> &newKanji );
    unsigned int         strokes() const;

    bool                 operator<( const Radical &other ) const;

  protected:
    unsigned int  strokeCount;
    QSet<QString> kanji;
    QSet<QString> components;
};

#endif
