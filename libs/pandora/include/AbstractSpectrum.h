// AbstractSpectrum.h
// 

#ifndef ABSTRACTSPECTRUM_H
#define ABSTRACTSPECTRUM_H 

#include "SignalTable.h"
#include "SoundEvent.h"
#include "SoundInfo.h"

//! Generic spectrum interface
/*!
	It is an abstract class. It is not possible to create 
	objects from this class and must be inherited. In this
	way it is possible to ensure a common interface for
	all the spectral design classes.
*/
class AbstractSpectrum : public SoundEvent  {
private:
	AbstractSpectrum& operator= (AbstractSpectrum&);
	AbstractSpectrum (const AbstractSpectrum&);
public: 
	AbstractSpectrum (SoundInfo* info, double start, double duration) 
		: SoundEvent (info, start, duration) {}
	virtual ~AbstractSpectrum () {}
	int partials () const;
	double min () const;
	double max () const;
};

#endif	// ABSTRACTSPECTRUM_H 

// EOF


