// AbstractGranulator.h
// 

#ifndef ABSTRACTGRANULATOR_H
#define ABSTRACTGRANULATOR_H 

#include "SoundInfo.h"
#include "SoundFrame.h"
#include "SoundEvent.h"
#include "windowType.h"

//! Base class for all the granulators.
/*!
	It is an abstract class. It is not possible to create 
	objects from this class and must be inherited. In this
	way it is possible to ensure a common interface for
	all the granulators.
*/
class AbstractGranulator : public SoundEvent {
private:
	AbstractGranulator& operator= (AbstractGranulator&);
	AbstractGranulator (const AbstractGranulator&);
protected:
	AbstractGranulator (SoundInfo* info, double start,
        double duration) : SoundEvent (info, start, duration) {}
	virtual ~AbstractGranulator () {}
public: 
    virtual void grainAmplitude (double amp) = 0;
    virtual void frequency (double freq, double randFreq,
        double gliss, double randGliss) = 0;
    virtual void density (double pulsesPerSec, double random) = 0;
    // no grain duration
    virtual void grainEnvelope (Pandora::windowType w, double shift) = 0;
    virtual void grainDirection (int sign ) = 0;
    virtual void speedFactor (double speed, long randSamples) = 0;
    virtual void setSelection (double phi0, double phiEnd) = 0;
    // no generation type
    // no cycles per grain
    virtual int overlap () const = 0;
    virtual SoundFrame& tick () = 0;
};

#endif	// ABSTRACTGRANULATOR_H 

// EOF

