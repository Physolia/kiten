#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextcodec.h>
#include <qtextstream.h>

#include "rad.h"

Rad::Rad() : QObject()
{
	loaded = false;
}

void Rad::load()
{
	if (loaded)
		return;

	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("appdata", "radkfile");
	if (radkfile.isNull())
	{
		KMessageBox::error(0, i18n("Kanji radical information file not installed, so radical searching cannot be used."));
		return;
	}

	QFile f(radkfile);
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));
	Radical cur;
	while (!t.eof())
	{
		QString s = t.readLine();

		QChar first = s.at(0);
		if (first == '#') // comment!
		{
			// nothing
		}
		else if (first == '$') // header
		{
			// save previous one
			if( !cur.kanji().isNull() )
				list.append(cur);

			//first entry is trim(last 4 chars).. <rad><space><strokes>
			unsigned int strokes = s.right(2).toUInt();
			QString radical = QString(s.at(2));
			cur = Radical(radical, strokes);
		}
		else // continuation
		{
			cur.addKanji(s);
		}
	}
	
	// we gotta append the last one!!
	// this nagged jasonkb for a bit wondering why fue wasn't showing up ;)
	list.append(cur);

	f.close();

	loaded = true;
}

QStringList Rad::radByStrokes(unsigned int strokes)
{
	load();

	QStringList ret;
	bool hadOne = false;
	QValueListIterator<Radical> it = list.begin();

	do
	{
		if ((*it).strokes() == strokes)
		{
			ret.append((*it).radical());
			hadOne = true;
		}
		else if(hadOne) // shortcut because it's a sorted list
		{
			return ret;
		}
	}
	while (++it != list.end());

	return ret;
}

QStringList Rad::kanjiByRad(const QString &text)
{
	//kdDebug() << "kanjiByRad, text is " << text << endl;
	load();
	QStringList ret;

	QValueListIterator<Radical> it;
	for (it = list.begin(); it != list.end() && (*it).radical() != text; ++it)
	{
		//kdDebug() << "kanjiByRad, looping, radical is " << (*it).radical() << endl;
	}

	QString kanji = (*it).kanji();
	for (unsigned i = 0; i < kanji.length(); ++i)
	{
		//kdDebug() << "kanjiByRad, i is " << i << endl;
		ret.append(QString(kanji.at(i)));
	}

	return ret;
}

QStringList Rad::kanjiByRad(const QStringList &list)
{
	//kdDebug() << "kanjiByRad (list version)\n";

	QStringList ret;
	QValueList<QStringList> lists;

	for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
	{
		//kdDebug() << "loading radical " << *it << endl;
		lists.append(kanjiByRad(*it));
	}

	QStringList first = lists.first();
	lists.pop_front();

	for (QStringList::Iterator kit = first.begin(); kit != first.end(); ++kit)
	{
		//kdDebug() << "kit is " << *kit << endl;
		QValueList<bool> outcomes;
		for (QValueList<QStringList>::Iterator it = lists.begin(); it != lists.end(); ++it)
		{
			//kdDebug() << "looping through lists\n";
			outcomes.append((*it).contains(*kit) > 0);
		}

		const bool containsBool = false;
		if ((outcomes.contains(containsBool) < 1))
		{
			//kdDebug() << "appending " << *kit << endl;
			ret.append(*kit);
		}
		else
		{
			//kdDebug() << "not appending " << *kit << endl;
		}
	}

	return ret;
}

Radical Rad::radByKanji(QString text)
{
	load();
	QString ret;

	QValueListIterator<Radical> it;
	for (it = list.end(); it != list.begin() && (*it).kanji().find(text) == -1; --it);

	return (*it);
}

unsigned int Rad::strokesByRad(QString text)
{
	load();
	QValueListIterator<Radical> it;
	for(it = list.begin(); it != list.end() && (*it).radical() != text; ++it);

	return (*it).strokes();
}

Rad::~Rad()
{
}

///////////////////////////////////////////////

RadWidget::RadWidget(Rad *_rad, QWidget *parent, const char *name) : QWidget(parent, name)
{
	hotlistNum = 3;

	rad = _rad;
	QHBoxLayout *hlayout = new QHBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	QVBoxLayout *vlayout = new QVBoxLayout(hlayout, KDialog::spacingHint());

	hotlistGroup = new QButtonGroup(1, Horizontal, i18n("Hotlist"), this);
	//hotlistGroup->setRadioButtonExclusive(true);
	vlayout->addWidget(hotlistGroup);

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");

	hotlist = config->readListEntry("Hotlist");

	while (hotlist.size() > hotlistNum)
		hotlist.pop_front();

	for (unsigned int i = 0; i < hotlistNum; ++i)
	{
		if (i >= hotlistNum)
			break;

		hotlistGroup->insert(new KPushButton(*hotlist.at(i), hotlistGroup), i);
	}
	connect(hotlistGroup, SIGNAL(clicked(int)), SLOT(hotlistClicked(int)));

	QVBoxLayout *layout = new QVBoxLayout(vlayout, KDialog::spacingHint());
	
	totalStrokes = new QCheckBox(i18n("Search by total strokes"), this);
	connect(totalStrokes, SIGNAL(clicked()), this, SLOT(totalClicked()));
	layout->addWidget(totalStrokes);

	QHBoxLayout *strokesLayout = new QHBoxLayout(layout, KDialog::spacingHint());
	totalSpin = new QSpinBox(1, 30, 1, this);
	strokesLayout->addWidget(totalSpin);
	strokesLayout->addStretch();
	totalErrLabel = new QLabel(i18n("+/-"), this);;
	strokesLayout->addWidget(totalErrLabel);
	totalErrSpin = new QSpinBox(0, 15, 1, this);
	strokesLayout->addWidget(totalErrSpin);
	
	ok = new KPushButton(i18n("&Look Up"), this);
	ok->setEnabled(false);
	connect(ok, SIGNAL(clicked()), SLOT(apply()));
	layout->addWidget(ok);
	cancel = new KPushButton(i18n("&Cancel"), this);
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	layout->addWidget(cancel);

	QVBoxLayout *middlevLayout = new QVBoxLayout(hlayout, KDialog::spacingHint());

	strokesSpin = new QSpinBox(1, 17, 1, this);
	middlevLayout->addWidget(strokesSpin);

	List = new KListBox(this);
	middlevLayout->addWidget(List);
	connect(List, SIGNAL(executed(QListBoxItem *)), this, SLOT(executed(QListBoxItem *)));
	connect(strokesSpin, SIGNAL(valueChanged(int)), this, SLOT(updateList(int)));

	QVBoxLayout *rightvlayout = new QVBoxLayout(hlayout, KDialog::spacingHint());
	selectedList = new KListBox(this);
	rightvlayout->addWidget(selectedList);
	connect(selectedList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

	remove = new KPushButton(i18n("&Remove"), this);
	rightvlayout->addWidget(remove);
	connect(remove, SIGNAL(clicked()), this, SLOT(removeSelected()));
	remove->setEnabled(false);

	clear = new KPushButton(i18n("C&lear"), this);
	rightvlayout->addWidget(clear);
	connect(clear, SIGNAL(clicked()), this, SLOT(clearSelected()));
	clear->setEnabled(false);

	setCaption(kapp->makeStdCaption(i18n("Radical Selector")));

	strokesSpin->setValue(config->readNumEntry("Strokes", 1));
	totalSpin->setValue(config->readNumEntry("Total Strokes", 1));
	totalErrSpin->setValue(config->readNumEntry("Total Strokes Error Margin", 0));
	totalStrokes->setChecked(config->readBoolEntry("Search By Total", false));

	totalClicked();
}

RadWidget::~RadWidget()
{
}

void RadWidget::hotlistClicked(int num)
{
	addToSelected(*hotlist.at(num));
}

void RadWidget::executed(QListBoxItem *item)
{
	addToSelected(item->text());
}

void RadWidget::clearSelected()
{
	selectedList->clear();
}

void RadWidget::removeSelected()
{
	int currentItem = selectedList->currentItem();
	if (currentItem != -1)
	{
		selectedList->removeItem(currentItem);
		selected.remove(selected.at(currentItem));

		numChanged();
		selectionChanged();
	}
}

void RadWidget::numChanged()
{
	ok->setEnabled(selectedList->count() > 0);
	clear->setEnabled(selectedList->count() > 0);
}

void RadWidget::addRadical(const QString &radical)
{
	addToSelected(radical);
}

void RadWidget::addToSelected(const QString &text)
{
	if (!selected.contains(text))
	{
		selectedList->insertItem(text);
		selected.append(text);

		numChanged();
		selectionChanged();
	}
}

void RadWidget::selectionChanged()
{
	//kdDebug() << "selectionChanged()" << endl;
	remove->setEnabled(selectedList->currentItem() != -1);
}

void RadWidget::updateList(int strokes)
{
	List->clear();

	List->insertStringList(rad->radByStrokes(static_cast<unsigned int>(strokes)));
}

void RadWidget::apply()
{
	//kdDebug() << "apply\n";

	if (selected.count() < 1)
	{
		//kdDebug() << "selected.count() is " << selected.count() << endl;
		return;
	}

	emit set(selected, totalStrokes->isChecked() ? totalSpin->value() : 0, totalErrSpin->value());

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");
	config->writeEntry("Strokes", strokesSpin->value());
	config->writeEntry("Total Strokes", totalSpin->value());
	config->writeEntry("Total Strokes Error Margin", totalErrSpin->value());
	config->writeEntry("Search By Total", totalStrokes->isChecked());

	for (QStringList::Iterator it = selected.begin(); it != selected.end(); ++it)
	{
		if (hotlist.find(*it) == hotlist.end())
		{
			if (hotlist.size() >= hotlistNum)
				hotlist.pop_front(); // stupid stl functions in Qt .. ;)
			hotlist.append(*it);

			config->writeEntry("Hotlist", hotlist);
		}
	}
	config->sync();

	close();
}

void RadWidget::totalClicked()
{
	bool enable = totalStrokes->isChecked();
	totalSpin->setEnabled(enable);
	totalErrSpin->setEnabled(enable);
	totalErrLabel->setEnabled(enable);
}

//////////////////////////////////////////////

Radical::Radical(QString text, unsigned int strokes)
{
	_Radical = text;
	Strokes = strokes;
}

void Radical::addKanji(QString &text)
{
	Kanji.append(text);
}

#include "rad.moc"
