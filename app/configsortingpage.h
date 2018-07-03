/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
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

#ifndef CONFIGSORTINGPAGE_H
#define CONFIGSORTINGPAGE_H

#include "ui_configsorting.h" //From the UI file

#include <QWidget>

class KitenConfigSkeleton;
class QString;

class ConfigSortingPage : public QWidget, public Ui::configSorting
{
  Q_OBJECT

  public:
    explicit ConfigSortingPage(  QWidget *parent = 0
                               , KitenConfigSkeleton *config = NULL
                               , Qt::WindowFlags f = {} );

  public slots:
    bool hasChanged();
    bool isDefault();
    void updateSettings();
    void updateWidgets();
    void updateWidgetsDefault();

  signals:
    void widgetChanged();

  private:
    KitenConfigSkeleton *_config;
    QStringList          _dictNames;
    QStringList          _fields;
};

#endif
