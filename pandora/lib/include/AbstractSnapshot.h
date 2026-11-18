// AbastractSnapshot.h
//

#ifndef ABSTRACTSNAPSHOT_H
#define ABSTRACTSNAPSHOT_H

#include <map>
#include <string>

class AbstractSnapshot {
protected:
	AbstractSnapshot (std::string name) : m_name (name) {}
	virtual ~AbstractSnapshot () {}
	std::string m_name;
	std::map <std::string, double> m_values;
};

#endif // ABSTRACTSNAPSHOT_H


// EOF


