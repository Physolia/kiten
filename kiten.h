#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>
#include "dict.h"
#include "rad.h"
#include "deinf.h"

class Rad;
class Entry;
class EditAction;
class QCheckBox;
class QPushButton;
class ResultView;
class ConfigureDialog;
class KGlobalAccel;
class KToggleAction;

class TopLevel : public KMainWindow
{
	Q_OBJECT

	public:
	TopLevel(QWidget *parent = 0, const char *name = 0);

	signals:
	void updateLists();
	void saveLists();
	void add(Dict::Entry);

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void search();
	void ressearch(const QString&);
	void searchBeginning();
	void searchEnd();
	void resultSearch();
	void searchAccel();
	void kanjiSearchAccel();
	void strokeSearch();
	void gradeSearch();
	void back();
	void forward();
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void slotUpdateConfiguration();
	void kanjiDictChange();
	void globalListChanged();
	void globalListDirty();
	void toggleCom();
	void addToList();
	void radicalSearch();
	void radSearch(QString &, unsigned int);

	void createLearn();
	void configureToolBars();
	void newToolBarConfig();

	private:
	Dict::Index _Index;
	Rad _Rad;
	Deinf::Index _DeinfIndex;
	ResultView *_ResultView;
	KToggleAction *kanjiCB;
	KToggleAction *deinfCB;
	KAction *irAction;
	KAction *addAction;
	KToggleAction *comCB;
	KAction *backAction;
	KAction *forwardAction;

	bool wholeWord;
	bool caseSensitive;

	KGlobalAccel *Accel;

	void doSearch(QString text, QRegExp regexp);
	void handleSearchResult(Dict::SearchResult);
	QString clipBoardText();

	ConfigureDialog *optionDialog;

	void setResults(unsigned int, unsigned int);

	bool autoCreateLearn;
	bool isListMod;

	QRegExp searchItems();
	QRegExp readingSearchItems(bool);
	QRegExp kanjiSearchItems(bool = false);

	EditAction *Edit;

	bool readingSearch; // if this is true and no results, try with kanjiSearchItems
	bool beginningReadingSearch;

	Dict::Entry toAddKanji;

	QValueList<Dict::SearchResult> resultHistory;
	QValueListIterator<Dict::SearchResult> currentResult;
	void addHistory(Dict::SearchResult);
	void enableHistoryButtons();

	Dict::Entry firstKanji(Dict::SearchResult);

	QString name;
	QString dicform;
};

#endif
