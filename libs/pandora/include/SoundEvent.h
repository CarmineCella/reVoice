// SoundEvent.h
// 

#ifndef SOUNDEVENT_H
#define SOUNDEVENT_H 

#include "SoundGenerator.h"
#include "SoundFrame.h"
#include "SoundInfo.h"

#define ALWAYS (-1)

//! Base class for the musical events.
/*!
	This class knows information about start time and duration and lets
	the user to inherit from it to create actual events. It knows about
	the spatial position of the sound and it's a specialization of
	SoundGenerator with the type SoundFrame for the tick () function.
*/
class SoundEvent : public SoundGenerator<SoundFrame> {
private:
	//! Assignment operator; it is private.
	SoundEvent& operator= (SoundEvent&);
	//! Copy constructor; it is private.
	SoundEvent (const SoundEvent&);
protected:
	//! Class constructor.
	/*!
		The user must provide the tick () function for the event in the derived class.
	*/
	SoundEvent (SoundInfo* info, double st, double dur);
	//! This method sets the start time of the SoundEvent; it is \b protected.
	virtual void start (double st);
	//! This method sets the global duration of the SoundEvent; it is \b protected.
	virtual void duration (double dur);
public:
	//! Class destructor. Must be public.
	virtual ~SoundEvent ();
	//! This method returns the start time of the SoundEvent.
	virtual double start () const;
	//! This method returns the duration of the SoundEvent.
	virtual double duration () const;
	//! This method is called by the time scheduler. It returns true if the instrument is active at sample 'index'.
	virtual bool isActive (long index);
	// Gets the spatial position of the object in the soundscape.
	/*!
		The spatial position is a value between 0 and 1. Note that every class
        can use this value as it needs. For example TableGranulator uses this
        value as spatial width for the grains.
	*/
	virtual double spatialPosition () const ;
	//! Sets the spatial position (between 0 and 1).
	virtual void spatialPosition (double position);
	//! This class removes from the formal interface the method 'update' by defining it.
	/*!
		In this way the derived classes must not implement this method;
		this because in the tick () function of the derived classes should
		be present some objects of type SoundObject which should manage the update method. 
	*/
	virtual void update (Subject* s);
protected:
	//! Variable to return in the tick () function.
    SoundFrame* m_frame;
	double m_leftPosition;
	double m_rightPosition;
	double m_st, m_dur;
	long m_startTime, m_samples;
};

#endif	// SOUNDEVENT_H 

// EOF

