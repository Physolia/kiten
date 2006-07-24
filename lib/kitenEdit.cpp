/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

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

#include "kitenEdit.h"
#include <kcombobox.h>
#include <kcompletion.h>

#include <kdebug.h>
KitenEdit::KitenEdit(KActionCollection *parent, QWidget *bar) : 
	KHistoryCombo(parent)
{
	comboBox = new KComboBox(true, bar);
	completion = comboBox->completionObject();
	kDebug() << comboBox << endl;
}

KitenEdit::~KitenEdit()
{
}

QString KitenEdit::text()
{
	return comboBox->currentText();
}

void KitenEdit::setText(const QString &text)
{
	//comboBox->setEditText(text);
}

KComboBox *KitenEdit::ComboBox()
{
	return comboBox;
}

KCompletion *KitenEdit::Completion()
{
	return completion;
}

#include "kitenEdit.moc"
