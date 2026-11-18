// SoundUnit.h
// 

#ifndef SOUNDUNIT_H
#define SOUNDUNIT_H 

#include "SoundInfo.h"
#include "SoundGenerator.h"

//! Base class for the unit generators.
/*!
	It's a specialization of SoundGenerator with the type double for
	the pure function tick ().
*/
class SoundUnit : public SoundGenerator<double> {
private:
	SoundUnit& operator= (SoundUnit&);
	SoundUnit (const SoundUnit&);
protected:
	SoundUnit (SoundInfo* info);
	virtual ~SoundUnit ();
	
	//! Variable to return in the tick () function.
	double m_val;
};

#endif	// SOUNDUNIT_H 

// EOF

