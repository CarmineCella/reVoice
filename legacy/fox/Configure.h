// Configure.h
// 

#ifndef CONFIGURE_H
#define CONFIGURE_H 

#include <fx.h>
#include <vector>
#include <string>
#include <fstream>
#include <map>

typedef std::map <std::string, double> numericProperties;
typedef std::map <std::string, std::string> literalProperties;
typedef numericProperties::iterator numIterator;
typedef literalProperties::iterator litIterator;

class Info {
public: 
	Info (std::string name) : _name (name) {}
	virtual ~Info () {}

 	std::string name () const { return _name; } 
    numericProperties _numbers; // public data for simpler use...
    literalProperties _strings; // 
private:
    std::string _name;
};

class Configure {
private:
	Configure& operator= (Configure&);
	Configure (const Configure&);
	Configure ();
	virtual ~Configure ();
public:
	static Configure* instance ();

	void program (std::string p) { _program = p; }
	std::string program () const { return _program; }
	void vendor (std::string v) { _vendor = v; }
	std::string vendor () const { return _vendor; }
	void notes (std::string n) { _notes = n; }
	std::string notes () const { return _notes; }
	
	void number (std::string section, std::string name, double value);
	double number (std::string section, std::string name);
	void literal (std::string section, std::string name, std::string value);
	std::string literal (std::string section, std::string name);

	void serialize (FXRegistry& reg);                        
	void deserialize (FXRegistry& reg);	
	void clearAll (FXRegistry& reg);
	void dump (std::ofstream& out);
	
private: 
	static Configure* _s;
	std::vector <Info> _sections;
	std::string _program;
	std::string _vendor;
	std::string _notes;

	int findSection (std::string section);
};

#endif	// CONFIGURE_H 

// EOF
