/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>

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

#ifndef LEARN_H
#define LEARN_H

#include <tqlistview.h>

#include <kurl.h>

#include "widgets.h"

class KAction;
class KPushButton;
class TQButtonGroup;
class TQListViewItem;
class TQSplitter;
class TQTabWidget;

// Item that sorts all columns numerically
class LearnItem : public QListViewItem
{
public:
	LearnItem(TQListView *parent, TQString label1, TQString label2 = TQString::null, TQString label3 = TQString::null, TQString label4 = TQString::null, TQString label5 = TQString::null, TQString label6 = TQString::null, TQString label7 = TQString::null, TQString label8 = TQString::null);
	int compare(TQListViewItem *item, int col, bool ascending) const;
};

class Learn : public KMainWindow
{
	Q_OBJECT
	
	public:
	Learn(Dict::Index *, TQWidget *parent = 0, const char *name = 0);
	~Learn();

	bool closeWindow();

	struct scoreCompare;

	signals:
	void destroyed(Learn *);
	void linkClicked(const TQString &);
	void configureLearn();

	public slots:
	void showKanji(TQListViewItem *);
	void updateQuizConfiguration();

	protected:
	virtual bool queryClose();

	private slots:
	void read(const KURL &);
	void write(const KURL &);
	void saveScores();
	void next();
	void prev();
	void update();
	void updateGrade();
	void random();
	void add();
	void addAll();
	void add(Dict::Entry, bool noEmit = false);
	void del();
	void cheat();
	void tabChanged(TQWidget *);
	void itemSelectionChanged();
	void save();
	void saveAs();

	void setDirty();
	void setClean();

	void open();
	void openNew();

	void print();

	void updateQuiz();
	void answerClicked(int);
	void qnew();
	void qKanjiClicked();

	void finishCtor();

	private:
	bool initialized;

	KURL filename;
	bool warnClose();

	TQTabWidget *Tabs;
	TQSplitter *listTop;
	TQWidget *quizTop;

	ResultView *View;
	TQValueList<Dict::Entry> list;
	TQValueListIterator<Dict::Entry> current;

	Dict::Index *index;

	KListView *List;
	KPushButton *Save;
	KPushButton *Del;
	KPushButton *qDel;

	bool isMod;

	// Quiz, an app to itself in a tabwidget :)

	static const int numberOfAnswers;
	TQButtonGroup *answers;
	TQPushButton *qKanji;

	TQListViewItem *prevItem;
	TQListViewItem *curItem;
	// if noEmit == true, don't emit dirty
	void addItem(TQListViewItem *, bool noEmit = false);

	void qupdate();

	int seikai;
	bool nogood;
	TQString shortenString(const TQString &);

	// Creates a random meaning not on the lists and adds the meaning
	// to the list.
	TQString randomMeaning(TQStringList &oldMeanings);

	KAction *forwardAct;
	KAction *printAct;
	KAction *backAct;
	KAction *cheatAct;
	KAction *saveAct;
	KAction *openAct;
	KAction *newAct;
	KAction *saveAsAct;
	KAction *addAct;
	KAction *addAllAct;
	KAction *removeAct;
	KAction *randomAct;
	KListAction *gradeAct;

	int getCurrentGrade(void);
	void setCurrentGrade(int grade);

	int quizOn;
	int guessOn;
	inline void numChanged();
};

#endif
