// jason@katzbrown.com

#include "dict.h"
#include <kdebug.h>
#include <qfile.h>
#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <qtextstream.h> 
#include <qfileinfo.h> 
#include <iostream.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qcstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstrlist.h>

#include <cassert>

// TODO: check which of these C headers are stll needed
#include <unistd.h> 
#include <stdio.h> 
#include <sys/mman.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>

namespace
{
void msgerr(const QString &msg, const QString &dict = QString::null)
{
	QString output = msg;
	if(dict != QString::null) output = msg.arg(dict);
	KMessageBox::error(0, output);
}
}

using namespace Dict;

File::File(QString path, QString n)
	: myName(n)
	, dictFile(path)
	, dictPtr((const unsigned char *)MAP_FAILED)
	, indexFile(KGlobal::dirs()->saveLocation("appdata", "xjdx/", true) + QFileInfo(path).baseName() + ".xjdx")
	, indexPtr((const uint32_t *)MAP_FAILED)
	, valid(false)
{
	if(!indexFile.exists())
	{
		// find the index generator executable
		KProcess proc;
		proc << KStandardDirs::findExe("kitengen") << path << indexFile.name();
		// TODO: put up a status dialog and event loop instead of blocking
		proc.start(KProcess::Block, KProcess::NoCommunication);
	}

	if(!dictFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open dictionary %1."), path);
		return;
	}

	dictPtr = (const unsigned char *)mmap(0, dictFile.size(), PROT_READ, MAP_SHARED, dictFile.handle(), 0);
	if(dictPtr == MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1."), path);
		return;
	}

	if(!indexFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open index for dictionary %1."), path);
		return;
	}

	indexPtr = (const uint32_t*)mmap(0, indexFile.size(), PROT_READ, MAP_SHARED, indexFile.handle(), 0);
	if(indexPtr == MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1's index file."), path);
		return;
	}

	valid = true;
}

File::~File(void)
{
	if(dictPtr != MAP_FAILED)
		munmap((void *)dictPtr, dictFile.size());
	dictFile.close();

	if(indexPtr != MAP_FAILED)
		munmap((void *)indexPtr, indexFile.size());
	indexFile.close();
}

QString File::name(void)
{
	return myName;
}

Array<const unsigned char> File::dict(void)
{
	assert(valid);
	return Array<const unsigned char>(dictPtr, dictFile.size());
}

Array<const uint32_t> File::index(void)
{
	assert(valid);
	return Array<const uint32_t>(indexPtr, indexFile.size());
}

int File::dictLength(void)
{
	return dictFile.size();
}

int File::indexLength(void)
{
	return indexFile.size();
}

bool File::isValid(void)
{
	return valid;
}

// returns specified character from a dictionary
unsigned char File::lookup(unsigned i, int offset)
{
	uint32_t pos = indexPtr[i] + offset - 1;
	if(pos > dictFile.size()) return 10;
	return dictPtr[pos];
}

// And last, Index itself is the API presented to the rest of Kiten
Index::Index()
	: QObject()
{
	dictFiles.setAutoDelete(true);
	kanjiDictFiles.setAutoDelete(true);
}

Index::~Index()
{
}

void Index::setDictList(const QStringList &list, const QStringList &names)
{
	loadDictList(dictFiles, list, names);
}

void Index::setKanjiDictList(const QStringList &list, const QStringList &names)
{
	loadDictList(kanjiDictFiles, list, names);
}

void Index::loadDictList(QPtrList<File> &fileList, const QStringList &dictList, const QStringList &dictNameList)
{
	fileList.clear();

	// check if we have a dict
	if (dictList.size() < 1)
	{
		msgerr(i18n("No dictionaries in list!"));
		return;
	}

	QStringList::ConstIterator it;
	QStringList::ConstIterator dictIt;
	for (it = dictList.begin(), dictIt = dictNameList.begin(); it != dictList.end(); ++it, ++dictIt)
	{
		File *f = new File(*it, *dictIt);
		// our ugly substitute for exceptions
		if(f->isValid())
			fileList.append(f);
		else
			delete f;
	}
}

QStringList Index::doSearch(File &file, QString text)
{
	// Do a binary search to find an entry that matches text
	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QCString eucString = codec.fromUnicode(text);

	QString prevResult;

	Array<const uint32_t> index = file.index();
	Array<const unsigned char> dict = file.dict();
	int lo = 0;
	int hi = index.size() - 1;
	unsigned cur;
	int comp = 0;

	do
	{
		cur = (hi + lo) / 2;
		comp = stringCompare(file, cur, eucString);

		if(comp < 0)
			hi = cur - 1;
		else if(comp > 0)
			lo = cur + 1;
	}
	while(hi >= lo && comp != 0);
	QStringList results;
	// A match?
	if(comp == 0)
	{
		// wheel back to make sure we get the first matching entry
		while(cur - 1 && 0 == stringCompare(file, cur - 1, eucString))
			--cur;

		// output every matching entry
		while(cur < index.size() && 0 == stringCompare(file, cur, eucString))
		{
			// because the index doesn't point
			// to the start of the line, find the
			// start of the line:
			int i = 0;
			while(file.lookup(cur, i - 1) != 0x0a) --i;

			QByteArray bytes(0);
			while(file.lookup(cur, i) != 0x0a) // get to end of our line
			{
				const char eucchar = file.lookup(cur, i);
				bytes.resize(bytes.size() + 1);
				bytes[bytes.size() - 1] = eucchar;
				++i;
			}

			QString result = codec.toUnicode(bytes) + QString("\n");
			if (prevResult != result)
			{
				results.append(result);
				prevResult = result;
			}

			++cur;
		}
	}

	// return all the entries found, or null if no match
	return results;
}

SearchResult Index::scanResults(QRegExp regexp, QStringList results, bool common)
{
	unsigned int num = 0;
	unsigned int fullNum = 0;

	SearchResult ret;
	
	//ret.results = results; //not here..
	
	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT " || (*itr).left(8) == "HEADING ")
		{
			ret.list.append(parse(*itr));
			continue;
		}

		int found = regexp.search(*itr);

		if (found >= 0)
		{
			++fullNum;
			if ((*itr).find(QString("(P)")) >= 0 || !common)
			{
				ret.results.append(*itr); // we append HERE, so we get the exact
				                          // results we have in ret.list
				ret.list.append(parse(*itr));
				++num;
			}
		}
	}

	ret.count = num;
	ret.outOf = fullNum;
	ret.common = common;
	return ret;
}

SearchResult Index::search(QRegExp regexp, QString text, bool common)
{
	QStringList results;
	for(QPtrListIterator<File> file(dictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	SearchResult res = scanResults(regexp, results, common);
	res.text = text;
	return res;
}

SearchResult Index::scanKanjiResults(QRegExp regexp, QStringList results, bool common)
{
	unsigned int num = 0;
	unsigned int fullNum = 0;
	const bool jmyCount = false; // don't count JinMeiYou as common
	SearchResult ret;
	ret.results = results;

	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT " || (*itr).left(8) == "HEADING ")
		{
			ret.list.append(kanjiParse(*itr));
			continue;
		}

		int found = regexp.search(*itr);

		if (found >= 0)
		{
			++fullNum;
			// common entries have G[1-8] (jouyou)
			QRegExp comregexp(jmyCount ? "G[1-9]" : "G[1-8]");
			if ((*itr).find(comregexp) >= 0 || !common)
			{
				ret.list.append(kanjiParse(*itr));
				++num;
			}
		}
	}

	ret.count = num;
	ret.outOf = fullNum;
	ret.common = common;
	return ret;
}

SearchResult Index::searchKanji(QRegExp regexp, QString text,  bool common)
{
	QStringList results;
	for(QPtrListIterator<File> file(kanjiDictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	SearchResult res = scanKanjiResults(regexp, results, common);
	res.text = text;
	return res;
}

SearchResult Index::searchPrevious(QRegExp regexp, QString text, SearchResult list, bool common)
{
	SearchResult res;

	if((*list.list.at(0)).extendedKanjiInfo())
		res = scanKanjiResults(regexp, list.results, common);
	else
		res = scanResults(regexp, list.results, common);

	res.text = text;
	return res;
}

// effectively does a strnicmp on two "strings" 
// except it will make katakana and hiragana match (EUC A4 & A5)
int Index::stringCompare(File &file, int index, QCString str)
{
	for(unsigned i = 0; i < str.length(); ++i)
	{
		unsigned char c1 = static_cast<unsigned char>(str[i]);
		unsigned char c2 = file.lookup(index, i);
		if ((c1 == '\0') || (c2 == '\0'))
			return 0;

		if ((i % 2) == 0)
		{
			if (c1 == 0xA5)
				c1 = 0xA4;

			if (c2 == 0xA5)
				c2 = 0xA4;
		}

		if ((c1 >= 'A') && (c1 <= 'Z')) c1 |= 0x20; /*fix ucase*/
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 |= 0x20;

		if (c1 != c2)
			return (int)c1 - (int)c2;
	}

	return 0;
}

Entry Index::parse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Entry(raw.right(length - 5));
	if (raw.left(8) == "HEADING ")
		return Entry(raw.right(length - 8), true);

	QString reading;
	QString kanji;
	QStringList meanings;
	QString curmeaning;
	bool firstmeaning = true;
	QCString parsemode("kanji");

	unsigned int i;
	for (i = 0; i < length; i++)
	{
		QChar ichar(raw.at(i));

		if (ichar == '[')
		{
			parsemode = "reading";
		}
		else if (ichar == ']')
		{
			// do nothing
		}
		else if (ichar == '/')
		{
			if (!firstmeaning)
			{
				meanings.prepend(curmeaning);
				curmeaning = "";
			}
			else
			{
				firstmeaning = false;
				parsemode = "meaning";
			}
		}
		else if (ichar == ' ')
		{
			if (parsemode == "meaning") // only one that needs the space
				curmeaning += ' ';
		}
		else if (parsemode == "kanji")
		{
			kanji += ichar;
		}
		else if (parsemode == "meaning")
		{
			curmeaning += ichar;
		}
		else if (parsemode == "reading")
		{
			reading += ichar;
		}
	}

	return (Entry(kanji, reading, meanings));
}

Entry Index::kanjiParse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Entry(raw.right(length - 5));
	if (raw.left(8) == "HEADING ")
		return Entry(raw.right(length - 8), true);

	QStringList readings;
	QString kanji;
	QStringList meanings;
	QString curmeaning;
	QString curreading;

	QString strfreq;
	QString strgrade;
	QString strstrokes;
	QString strmiscount = "";

	bool prevwasspace = true;
	QChar detailname;
	QCString parsemode("kanji");

	// if there are two S entries, second is common miscount
	bool strokesset = false;

	unsigned int i;
	QChar ichar;
	for (i = 0; i < length; i++)
	{
		ichar = raw.at(i);

		if (ichar == ' ')
		{
			if (parsemode == "reading")
			{
				readings.append(curreading);
				curreading = "";
			}
			else if (parsemode == "kanji")
			{
				parsemode = "misc";
			}
			else if (parsemode == "detail")
			{
				if (detailname == 'S')
					strokesset = true;

				parsemode = "misc";
			}
			else if (parsemode == "meaning")
			{
				curmeaning += ichar;
			}
			prevwasspace = true;
		}
		else if (ichar == '{')
		{
			parsemode = "meaning";
		}
		else if (ichar == '}')
		{
			meanings.prepend(curmeaning);
			curmeaning = "";
		}
		else if (parsemode == "detail")
		{
			if (detailname == 'G')
			{
				strgrade += ichar;
			}
			else if (detailname == 'F')
			{
				strfreq += ichar;
			}
			else if (detailname == 'S')
			{
				if (strokesset)
					strmiscount += ichar;
				else
					strstrokes += ichar;
			}
			prevwasspace = false;
		}
		else if (parsemode == "kanji")
		{
			kanji += ichar;
		}
		else if (parsemode == "meaning")
		{
			curmeaning += ichar;
		}
		else if (parsemode == "reading")
		{
			curreading += ichar;
		}
		else if (parsemode == "misc" && prevwasspace)
		{
			if (QRegExp("[A-Za-z0-9]").search(QString(ichar)) >= 0)
				   // is non-japanese?
			{
				detailname = ichar;
				parsemode = "detail";
			}
			else
			{
				curreading = QString(ichar);
				parsemode = "reading";
			}
		}
	}

	return (Entry(kanji, readings, meanings, strgrade.toUInt(), strfreq.toUInt(), strstrokes.toUInt(), strmiscount.toUInt()));
}

QString Dict::prettyMeaning(QStringList Meanings)
{
	QString meanings;
	QStringList::Iterator it;
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
		meanings.append((*it).stripWhiteSpace()).append("; ");

	meanings.truncate(meanings.length() - 2);
	return meanings;
}

QString Dict::prettyKanjiReading(QStringList Readings)
{
	QStringList::Iterator it;
	QString html;

	for (it = Readings.begin(); it != Readings.end(); ++it)
	{
		if ((*it) == "T1")
			html += i18n("In names: ");
		else
		{
			if ((*it) == "T2")
				html += i18n("As radical: ");
			else
			{
				html += (*it).stripWhiteSpace();
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); // get rid of last ,

	return html;
}

///////////////////////////////////////////////////////////////

Entry::Entry(const QString & kanji, const QString & reading, const QStringList &meanings)
	: DictName(QString::fromLatin1("__NOTSET"))
	, Header(QString::fromLatin1("__NOTSET"))
	, Meanings(meanings)
	, Kanji(kanji)
	, KanaOnly(reading.isEmpty())
	, Readings(KanaOnly ? kanji : reading)
	, ExtendedKanjiInfo(false)
	, Grade(0)
	, Strokes(0)
	, Miscount(0)
	, Freq(0)
{
}

Entry::Entry(QString &kanji, QStringList &readings, QStringList &meanings, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount)
	: DictName(QString::fromLatin1("__NOTSET"))
	, Header(QString::fromLatin1("__NOTSET"))
	, Meanings(meanings)
	, Kanji(kanji)
	, KanaOnly(false)
	, Readings(readings)
	, ExtendedKanjiInfo(true)
	, Grade(grade)
	, Strokes(strokes)
	, Miscount(miscount)
	, Freq(freq)
{
}

Entry::Entry(const QString &dictname)
	: KanaOnly(true)
	, ExtendedKanjiInfo(false)
{
	DictName = dictname;
}

Entry::Entry(const QString &headername, bool header)
	: KanaOnly(true)
	, DictName(QString::fromLatin1("__NOTSET"))
	, Header(headername)
	, ExtendedKanjiInfo(false)
{
}

QString Entry::dictName()
{
	return DictName;
}

QString Entry::header()
{
	return Header;
}

bool Entry::kanaOnly()
{
	return KanaOnly;
}

QString Entry::kanji()
{
	return Kanji;
}

QStringList Entry::readings()
{
	return Readings;
}

QString Entry::firstReading()
{
	return *Readings.at(0);
}

QStringList Entry::meanings()
{
	return Meanings;
}

unsigned int Entry::grade()
{
	return Grade;
}

unsigned int Entry::freq()
{
	return Freq;
}

unsigned int Entry::miscount()
{
	return Miscount;
}

unsigned int Entry::strokes()
{
	return Strokes;
}

bool Entry::extendedKanjiInfo()
{
	return ExtendedKanjiInfo;
}

#include "dict.moc"
