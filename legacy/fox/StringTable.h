// StringTable.h
// 

#ifndef STRINGTABLE_H
#define STRINGTABLE_H 

#include "StringTable.h"
#include <string>
#include <map>

class StringTable {
	void operator= (StringTable&);
	StringTable (const StringTable&);
	StringTable ();
	virtual ~StringTable ();
public:
	static StringTable* instance ();
	std::string StringTable::get (std::string key);
private:
	static StringTable* _s;
	std::map <std::string, std::string> _table;
};

#endif	// STRINGTABLE_H 

// EOF
