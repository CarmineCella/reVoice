// SoundEffect.h
// 

#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H 

#include "SoundInfo.h"
#include "SoundProcessor.h"

//! Base class for the effects.
/*!
	It's a specialization of SoundProcessor with the type double for
	the pure function process ().
*/
class SoundEffect : public SoundProcessor<double> {
private:
	SoundEffect& operator= (SoundEffect&);
	SoundEffect (const SoundEffect&);
protected:
	//! Variable to return in the tick () function.
	double m_val;

	SoundEffect (SoundInfo* info);
	virtual ~SoundEffect ();
};

#endif	// SOUNDEFFECT_H 

// EOF

