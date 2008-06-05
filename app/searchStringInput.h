/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef SEARCHSTRINGINPUT_H
#define SEARCHSTRINGINPUT_H

#include "DictQuery.h"

class KAction;
class KToggleAction;
class KSelectAction;
class WordType;
class KHistoryComboBox;
class kiten;

class searchStringInput : public QObject {
	Q_OBJECT

	public:
		explicit searchStringInput(kiten *parent);

		void setDefaultsFromConfig();
		void updateFontFromConfig();

		DictQuery getSearchQuery() const;
		void setSearchQuery(const DictQuery &query);
	public slots:
		void test();

	signals:
		void search();

	private slots:
		void focusInput();

	private:
		KToggleAction *actionDeinflect;
		KToggleAction *actionFilterRare;
		KSelectAction *actionSearchSection;	//Search exact/anywhere/beginning
		KSelectAction *actionSelectWordType;
		KHistoryComboBox *actionTextInput;
		KAction *actionFocusInput;
		kiten *parent;
};

#endif
