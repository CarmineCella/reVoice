// Observer.h
//

#ifndef OBSERVER_H
#define OBSERVER_H

class Subject;

//! Observer class for the Subject - Observer pattern.
class Observer {
protected:
	//! Default constructor; it is protected.
	Observer () {}
public:
	//! Class destructor; it is virtual.
	virtual ~Observer () {}
	//! Pure method that each derived class must implement
	/*!
		The Subject calls this method in all the Observers to
		manage the modifications of its data.
	*/
	virtual void update (Subject* s) = 0;
};

#endif	// OBSERVER_H

// EOF


