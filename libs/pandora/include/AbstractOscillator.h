// AbstractOscillator.h
// 

#ifndef ABSTRACTOSCILLATOR_H
#define ABSTRACTOSCILLATOR_H 

#include "SoundInfo.h"
#include "SignalTable.h"
#include "SoundUnit.h"

//! Base class for all the oscillators.
/*!
	It is an abstract class. It is not possible to create 
	objects from this class and must be inherited. In this
	way it is possible to ensure a common interface for
	all the oscillators.
*/
class AbstractOscillator : public SoundUnit {
private:
	//! Assignment operator; it is private.
	AbstractOscillator& operator= (AbstractOscillator&);
	//! Copy constructor; it is private.
	AbstractOscillator (const AbstractOscillator&);
protected:
	//! Class constructor.
	AbstractOscillator (SoundInfo* info) : SoundUnit (info) {}
	//! Class destructor.
	virtual ~AbstractOscillator () {}
public: 
	//! Virtual method to set amplitude.
	virtual void amplitude (double amp) = 0;
	//! Virtual method to set the frequency.
	virtual void frequency (double freq) = 0;
	//! Virtual method to set the phase.
	virtual void phase (double phi) = 0;
	//! Virtual method to get the phase.
	virtual double phase () const = 0;
	//! Virtual method to set a new table.
	virtual void table (SignalTable* tab) = 0;
};

#endif	// ABSTRACTOSCILLATOR_H 

// EOF

