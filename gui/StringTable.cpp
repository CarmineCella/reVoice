// StringTable.cpp
//

#include "StringTable.h"
#include <map>
#include <string>

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

using namespace std;

StringTable::StringTable () {
	_table["STRING_NOT_FOUND"] = "String not found";
	#if  defined (ITALIAN_LANG)
		#include "stringsIta.h"
	#else
		#include "stringsEng.h"
	#endif
}

StringTable::~StringTable () {
}

StringTable* StringTable::_s = 0;
StringTable* StringTable::instance () {
	if (_s == 0)
		_s = new StringTable;
	return _s;
}

string StringTable::get (const string key) {
	map<string, string>::iterator iter;

	iter = _table.find (key);
	if (iter != _table.end ())
		return iter->second;
	else
		return _table["STRING_NOT_FOUND"];
}

// EOF
