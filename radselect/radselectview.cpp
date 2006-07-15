/***************************************************************************
 *   Copyright (C) 2005 by Joseph Kerian   *
 *   jkerian@gmail.com   *
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
/*  TODO:
*	Double check that I need to subclass the QPushButton
*	Add an alternate stroke setup (setable in preferences)
*	Add preferences hooks
		radical files
		FONTS!!!

    Future Plans:
*	Design a custom QGridLayout to rearrange buttons dynamically to resize
*	Make radical search list a two/three column field with delete button
*	Design multiple radical file handling
*	Build a proper exception handling framework
*	Icon set for properly displayed radicals
*	Radical Decomposition, and associated interface
*	Rebuild most of this file as a ui file, handle any special code in
		radselect.cpp, and move the smaller classes to their own file.

*/

#include "radselectview.h"
#include "radselectbuttongrid.h"

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>


radselectView::radselectView(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	//Configure the scrolling area
	
	radselectButtonGrid *buttons = new radselectButtonGrid(this);
	radical_box->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	buttons->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	radical_box->setWidget(buttons);
	radical_box->setWidgetResizable(true);

   //== Now we connect all our signals ==
	//First a generic "if this changes, emit noise"
   connect( grade_selector, SIGNAL( textChanged(const QString&)), this,
		SLOT(changedSearch()));
	//Connect our radical grid to our adding method
	connect( buttons, SIGNAL( addRadicalToList(const QString&) ), this,
		SLOT(	addRadicalToList(const QString&) ) );
	//Connect removing the radicals from the list
   connect( selected_radicals, SIGNAL( itemClicked(QListWidgetItem*) ), 
		this, SLOT( queueDeleteRadical(QListWidgetItem*) ) );
	//Connect our search button
   connect( search_button, SIGNAL( clicked() ), this, SLOT(startSearch()));

}

radselectView::~radselectView()
{
}


void radselectView::startSearch()
{	//Something has triggered a search... usually a button-press
	QString radicals_result,grade_result,stroke_result,result;
	result = getSearchInfo(radicals_result,grade_result,stroke_result);

	emit searchTrigger(radicals_result.split(""),stroke_result,grade_result);
}

QString radselectView::getSearchInfo
	(QString& radicals_result,QString& grade_result, QString& stroke_result) {
	//This is where we assemble the search string
	QString result;
	
	for(int i=0; i < selected_radicals->count(); i++)
		radicals_result.append(selected_radicals->item(i)->text());

	if(!radicals_result.isEmpty())
		result = result.append(QString("_R:%1 ")).arg(radicals_result);

    	if(grade_selector->currentText()!="Any") {
    		QString grade = grade_selector->currentText();
		if(grade==tr("Jouyou") || grade==tr("Jinmeiyou"))
			grade_result = grade;
		else
			grade_result = grade.right(1);

		result = result.append(QString("G:%1 ").arg(grade_result));
	}

/*    	QString strokes = strokes_counter->text();
	if(!strokes.isEmpty()) {
		stroke_result = simplifyStrokeString(strokes);
		if(stroke_result.length() > 0) {
			result = result.append(QString("S:%1 ").arg(stroke_result));
			strokes_counter->setText(stroke_result);
		}
	}
	*/
	return result;
}

void radselectView::addRadicalToList(const QString& button)
{
	QString msg = QString("Radical: %1").arg(button);
	
	QList<QListWidgetItem *> isItAlreadyThere = 
		selected_radicals->findItems(button, Qt::MatchExactly);
	
	if(isItAlreadyThere.count() == 0){
		new QListWidgetItem(button,selected_radicals);
		emit signalChangeStatusbar(msg.append(" added to the radical list"));
		emit searchModified();
	} else {
		emit signalChangeStatusbar(msg.append(" already on the radical list"));
	}
}


void radselectView::queueDeleteRadical(QListWidgetItem *iVictim)
{	
	if(iVictim == 0) return;

	victim=iVictim;
	QTimer::singleShot(0, this, SLOT(deleteRadical()) );
	emit signalChangeStatusbar
		(QString("Deleting Radical %1 from the list").arg(iVictim->text()));
}
void radselectView::deleteRadical() {
	delete victim;
	emit searchModified();
}

void radselectView::load(QString iRadicals, QString iGrade,QString iStrokes)
{
	//Handle radicals first
	QStringList sep_radical = iRadicals.split(QString(""));
	selected_radicals->clear();
	for ( QStringList::Iterator it = sep_radical.begin(); 
				it != sep_radical.end(); ++it ) 
			addRadicalToList(*it);

	//Grade is relatively easy
	QString grade = grade_selector->currentText();
	if(iGrade==tr("Jouyou"))
		iGrade = "7";
	else if (iGrade==tr("Jinmeiyou"))
		iGrade = "8";
	
	if(iGrade.toInt() >= 1 && iGrade.toInt() <= 8)
		grade_selector->setCurrentIndex(iGrade.toInt());
	else
		grade_selector->setCurrentIndex(0);

	//Strokes needs redoing (due to new widget)
/*	if(!iStrokes.isEmpty()) {
		QString strokes = simplifyStrokeString(iStrokes);
		strokes_counter->setText(strokes);
	} else
		strokes_counter->setText("");
*/
	emit searchModified();
}

void radselectView::changedSearch() {
	emit searchModified();
}

#include "radselectview.moc"
