/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef RAD_H
#define RAD_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qwidget.h>

class QCheckBox;
class QLabel;
class QListBoxItem;
class QSpinBox;
class KPushButton;
class KListBox;
class QButtonGroup;

class Radical
{
	public:
	Radical(QString = QString::null, unsigned int = 0);

	QString radical() { return _Radical; };
	unsigned int strokes() { return Strokes; };
	QString kanji() { return Kanji; };

	void addKanji(QString &);

	private:
	QString _Radical;
	unsigned int Strokes;
	QString Kanji;
};

class Rad : public QObject
{
	Q_OBJECT
	
	public:
	Rad();
	~Rad();

	QStringList radByStrokes(unsigned int);
	QStringList kanjiByRad(const QString &);
	QStringList kanjiByRad(const QStringList &);
	Radical radByKanji(QString);
	unsigned int strokesByRad(QString);

	private:
	void loadLine(QString &);
	void load();

	QValueList<Radical> list;
	bool loaded;
};

class RadWidget : public QWidget
{
	Q_OBJECT

	public:
	RadWidget(Rad *, QWidget *parent = 0, const char *name = 0);
	~RadWidget();

	signals:
	// if totalStrokes == 0, then don't search by total strokes
	void set(const QStringList &radical, unsigned int totalStrokes, unsigned int totalStrokesErr);

	public slots:
	void addRadical(const QString &);

	private slots:
	void updateList(int);
	void apply();
	void totalClicked(void);
	void selectionChanged();
	void hotlistClicked(int);
	void addToSelected(const QString &);
	void executed(QListBoxItem *);
	void removeSelected();
	void clearSelected();
	
	private:
	QSpinBox *strokesSpin;
	QSpinBox *totalSpin;
	QSpinBox *totalErrSpin;
	QLabel *totalErrLabel;
	KPushButton *ok;
	KPushButton *cancel;
	KPushButton *remove;
	KPushButton *clear;
	QButtonGroup *hotlistGroup;
	QCheckBox *totalStrokes;
	KListBox *List;
	KListBox *selectedList;
	QStringList selected;

	Rad *rad;

	unsigned int hotlistNum;
	QStringList hotlist;

	void numChanged();
};

#endif
