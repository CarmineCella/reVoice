// Configure.cpp
//

#include "Configure.h"
#include <fx.h>
#include <stdexcept>

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

using namespace std;

Configure::Configure () {}
Configure::~Configure () {}

Configure* Configure::_s = 0;
Configure* Configure::instance () {
	if (_s == 0) {
		_s = new Configure;
	}
	return _s;
}

void Configure::number (string section, string name, double value) {
	int num = findSection (section);
	if (num >= 0) {
		_sections[num]._numbers[name] = value;
	}
	else {
		Info s (section);
		_sections.push_back (s);
		number (section, name, value);
	}
}

double Configure::number (string section, string name) {
	int num = findSection (section);
	if (num >= 0) {
		numIterator iter = _sections[num]._numbers.find (name);

		if (iter != _sections[num]._numbers.end ()) {
			return _sections[num]._numbers[name];
		}
		else
			throw runtime_error ("invalid parameter requested");
	}
	else
		throw runtime_error ("invalid parameter requested");
}

void Configure::literal (string section, string name, string value) {
	int num = findSection (section);
	if (num >= 0) {
		_sections[num]._strings[name] = value;
	}
	else {
		Info s (section);
		_sections.push_back (s);
		literal (section, name, value);
	}
}

string Configure::literal (string section, string name) {
	int num = findSection (section);
	if (num >= 0) {
		litIterator iter = _sections[num]._strings.find (name);

		if (iter != _sections[num]._strings.end ()) {
			return _sections[num]._strings[name];
		}
		else
			throw runtime_error ("invalid parameter requested");
	}
	else
		throw runtime_error ("invalid parameter requested");
}

int Configure::findSection (string section) {
	int found = -1;

	for (unsigned int i = 0; i < _sections.size (); i++) {
		if (_sections[i].name () == section) {
			found = i;
			break;
		}
	}

	return found;
}

void Configure::serialize (FXRegistry& reg) {
	for (unsigned int i = 0; i < _sections.size (); i++) {
		numIterator numIter;
		litIterator litIter;

		for (numIter = _sections[i]._numbers.begin (); numIter !=
			_sections[i]._numbers.end (); ++numIter) {
				reg.writeRealEntry (_sections[i].name ().c_str (),
					numIter->first.c_str (),
					numIter->second);
		}
		for (litIter = _sections[i]._strings.begin (); litIter !=
			_sections[i]._strings.end (); ++litIter) {
				reg.writeStringEntry (_sections[i].name ().c_str (),
					litIter->first.c_str (),
					litIter->second.c_str ());
		}
	}
}

void Configure::deserialize (FXRegistry& reg) {
	for (unsigned int i = 0; i < _sections.size (); i++) {
		for (numIterator numIter = _sections[i]._numbers.begin (); numIter !=
			_sections[i]._numbers.end (); ++numIter) {
				double d = reg.readRealEntry (_sections[i].name ().c_str (),
					numIter->first.c_str (), -12344321);
				if (d != -12344321)
					_sections[i]._numbers[numIter->first] = d;
		}
		for (litIterator litIter = _sections[i]._strings.begin (); litIter !=
			_sections[i]._strings.end (); ++litIter) {
				string s = reg.readStringEntry (_sections[i].name ().c_str (),
					litIter->first.c_str (), "invalidKey");
				if (s != "invalidKey")
					_sections[i]._strings[litIter->first] = s;
		}
	}
}

void Configure::clearAll (FXRegistry& reg) {
	for (unsigned int i = 0; i < _sections.size (); i++) {
		reg.deleteSection (_sections[i].name ().c_str ());
	}
}

void Configure::dump (std::ofstream& out) {
//	out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
//		<< endl << endl;
	out << "// " << vendor () << " " << program () << " (" <<
		notes () << ") - Status dump" << endl << endl;
	for (unsigned int i = 0; i < _sections.size (); i++) {
		numIterator numIter;
		litIterator litIter;

		if (_sections[i]._numbers.size () != 0) {
			out << "[" << _sections[i].name () << ".Numbers]" << endl;
			for (numIter = _sections[i]._numbers.begin (); numIter !=
				_sections[i]._numbers.end (); ++numIter) {
					out << "\t" << numIter->first.c_str () << " = ";
					out << numIter->second << endl;
			}
		}

		if (_sections[i]._strings.size () != 0) {
			out << "[" << _sections[i].name () << ".Literals]" << endl;
			for (litIter = _sections[i]._strings.begin (); litIter !=
				_sections[i]._strings.end (); ++litIter) {
					out << "\t" << litIter->first.c_str () << " = ";
					out << litIter->second << endl;
			}
		}

		out << endl;
	}

	out << "// EOF" << endl;
}


// EOF
