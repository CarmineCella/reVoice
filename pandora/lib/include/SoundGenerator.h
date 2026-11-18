// SoundGenerator.h
// 

#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H 

#include "SoundObject.h"

//! Base class for all the generators.
/*!
	Provides a common interface for the child objects, defining
	the general sampling rate and the virtual function tick ().
*/
template <typename T>
class SoundGenerator : public SoundObject {
private:
	//! Assignment operator; it's private.
	SoundGenerator& operator= (SoundGenerator&);
	//! Copy constructor; it's private.
	SoundGenerator (const SoundGenerator&);
protected:
	//! Default constructor; it's protected.
	/*!
		Needs the pointer to a SoundInfo object.
	*/
	SoundGenerator (SoundInfo* info) : SoundObject (info) {}
	virtual ~SoundGenerator () {}
public:
	//! Pure virtual method that every child class MUST implement.
	/*!
		The output of this function should be the sample value specific to each
		class. No parameters are allowed.
	*/
	virtual T& tick () = 0;
};

#endif	// SOUNDGENERATOR_H 

// EOF

