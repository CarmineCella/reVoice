// AbstractPitchDetector.h
// 

#ifndef ABSTRACTPITCHDETECTOR_H
#define ABSTRACTPITCHDETECTOR_H 

#include "SoundInfo.h"
#include "SoundEffect.h"

#define MIN_FREQ_LIMIT 20

//! Base class for all the pitch detectors.
/*!
	It is an abstract class. It is not possible to create 
	objects from this class and must be inherited. In this
	way it is possible to ensure a common interface for
	all the pitch detectors.
*/
class AbstractPitchDetector : public SoundEffect {
private:
	AbstractPitchDetector& operator= (AbstractPitchDetector&);
	AbstractPitchDetector (const AbstractPitchDetector&);
protected:
	AbstractPitchDetector (SoundInfo* info) : SoundEffect (info) {}
public:
	virtual ~AbstractPitchDetector () {}
    virtual void threshold (double t) = 0;
	virtual void reset () = 0;
	virtual double get () const = 0;
	virtual double min () const = 0;
	virtual double max () const = 0;
	virtual void smoothingRate (double f) = 0;
	virtual void smoothingQuality (double f) = 0;
};

#endif	// ABSTRACTPITCHDETECTOR_H 

// EOF

