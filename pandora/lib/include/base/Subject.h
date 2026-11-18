// Subject.h
//

#ifndef SUBJECT_H
#define SUBJECT_H

#include "Observer.h"
#include <vector>

//! Subject class for the Subject - Observer pattern.
class Subject {
protected:
	//! Default constructor; it is protected.
	Subject () {}
public:
	//! Class destructor; it is virtual.
	virtual ~Subject () {}
	//! This method attach an Observer to this Subject
	virtual void attach (Observer* o) {
		observers.push_back (o);
	}
	//! This method breaks the link between an Observer and this Subject
	virtual bool detach (Observer* o) {
		int n = find (o);

		if (n > -1) {
			observers.erase (observers.begin () + n);
			return true;
		}

		return false;
	}
	//! By calling this method the Subject notify all its Observers.
	virtual void notify () {
		for (unsigned int i = 0; i < observers.size (); i++)
			observers[i]->update (this);
	}
private:
	std::vector <Observer*> observers;
	//! The Subject uses this method to find a specific Observer.
	int find (Observer* o) {
		int it = -1;

		for (unsigned int i = 0; i < observers.size (); i++) {
			if (observers[i] == o) {
				it = i;
				break;
			}
		}

		return it;
	}
};

#endif	// SUBJECT_H

// EOF


