// SoundProcessor.h
// 

#ifndef SOUNDPROCESSOR_H
#define SOUNDPROCESSOR_H 

#include "SoundObject.h"
#include "SoundInfo.h"

//! Base class for all the processors.
/*!
	Provides a common interface for the child objects, defining
	the general sampling rate and the virtual function process ().
*/
template <typename T>
class SoundProcessor : public SoundObject {
private:
	//! Assignment operator; it's private.
	SoundProcessor& operator= (SoundProcessor&);
	//! Copy constructor; it's private.
	SoundProcessor (const SoundProcessor&);
protected:
	//! Default constructor; it's protected.
	/*!
		Needs the pointer to a SoundInfo object.
	*/
	SoundProcessor (SoundInfo* info) : SoundObject (info) {}
	virtual ~SoundProcessor () {}
public:
	//! Pure virtual method that every child class MUST implement.
	/*!
		The output of this function should be the sample value specific to each
		class, while the input must be another sample.
	*/
	virtual T& process (T& sig) = 0;
};

#endif	// SOUNDPROCESSOR_H 

// EOF

