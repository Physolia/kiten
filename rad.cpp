#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kapp.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qstring.h>

#include "rad.h"

Rad::Rad() : QObject()
{
	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("appdata", "radkfile");
	if (radkfile == QString::null)
	{
		KMessageBox::error(0, i18n("Kanji radical information file not installed, so radical searching cannot be used."));
		return;
	}

	QFile f(radkfile);
	QString s;
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
	}

	QTextStream t(&f);
	while (!t.eof())
	{
		s = t.readLine();

		loadLine(s);
	}

	f.close();

	list.setAutoDelete(true); // I should do this, no?
}

void Rad::loadLine(QString &s)
{
	QChar first = s.at(0);
	if (first == '#') // comment!
		return;
	if (first == '$') // header
	{
		//first entry is trim(last 4 chars).. <rad><space><strokes>

		unsigned int strokes = s.right(2).stripWhiteSpace().toUInt();
		QString radical = QString(s.at(2));
		curRadical= new Radical(radical, strokes);
		list.append(curRadical);
		return;
	}

	curRadical->addKanji(s);
}

QStringList Rad::radByStrokes(unsigned int strokes)
{
	QStringList ret;
	bool hadOne = false;
	QPtrListIterator<Radical> it(list);
	Radical *cur;

	while ((cur = it.current()) != 0)
	{
		++it;

		if (cur->strokes() == strokes)
		{
			ret.append(cur->radical());
			hadOne = true;
			continue;
		}

		// if we've hadOne, and now we don't, there won't be anymore
		if (hadOne)
			break;
	}

	return ret;
}

QStringList Rad::kanjiByRad(QString &text)
{
	QStringList ret;
	QPtrListIterator<Radical> it(list);
	Radical *cur;

	while ((cur = it.current()) != 0)
	{
		++it;
		if (cur->radical() == text)
			break;
	}

	QString kanji = cur->kanji();

	unsigned int i;
	for (i = 0; i < kanji.length(); ++i)
	{
		ret.append(QString(kanji.at(i)));
	}

	return ret;
}

Rad::~Rad()
{
}

///////////////////////////////////////////////

RadWidget::RadWidget(Rad *_rad, QWidget *parent, const char *name) : QWidget(parent, name)
{
	rad = _rad;
	QHBoxLayout *hlayout = new QHBoxLayout(this, 6);
	QVBoxLayout *layout = new QVBoxLayout(hlayout, 6);
	layout->addWidget(new QLabel(i18n("<strong>Radical</strong> strokes:"), this));
	strokesSpin = new QSpinBox(1, 17, 1, this);
	layout->addWidget(strokesSpin);
	
	layout->addStretch(2);

	layout->addWidget(new QLabel(i18n("<strong>All</strong> strokes:"), this));
	nonradSpin = new QSpinBox(1, 20, 1, this);
	layout->addWidget(nonradSpin);
	
	layout->addStretch(5);

	ok = new QPushButton(i18n("&OK"), this);
	connect(ok, SIGNAL(clicked()), SLOT(apply()));
	layout->addWidget(ok);
	cancel = new QPushButton(i18n("&Cancel"), this);
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	layout->addWidget(cancel);

	List = new QListBox(this);
	hlayout->addWidget(List);
	connect(strokesSpin, SIGNAL(valueChanged(int)), SLOT(updateList(int)));

	setCaption(kapp->makeStdCaption(i18n("Radical Selector")));

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");
	strokesSpin->setValue(config->readNumEntry("Strokes", 1));
	nonradSpin->setValue(config->readNumEntry("NonRad Strokes", 1));
}

RadWidget::~RadWidget()
{
}

void RadWidget::updateList(int strokes)
{
	List->clear();

	List->insertStringList(rad->radByStrokes(static_cast<unsigned int>(strokes)));
}

void RadWidget::apply()
{
	QString text = List->currentText();
	if (text == QString::null)
		return;

	emit set(text, nonradSpin->value());

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");
	config->writeEntry("Strokes", strokesSpin->value());
	config->writeEntry("NonRad Strokes", nonradSpin->value());
	config->sync();

	close();
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
