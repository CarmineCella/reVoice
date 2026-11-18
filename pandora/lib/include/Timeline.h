// Timeline.h
// 

#ifndef TIMELINE_H
#define TIMELINE_H 

#include "SoundInfo.h"
#include "SoundEvent.h"
#include <vector>

//! This class represents the time line. It schedules SoundEvent objects.
/*!
	You should put every SoundEvent you have create in an object of this type,
	and use the tick () function of this class to call the other tick () functions.
	Beeing a SoundEvent itself, it can be scheduled in another object of 
	the same type. NB: It DELETES the objects contained!
*/
class Timeline : public SoundEvent {
private:
	//! Assignment operator; it is private.
	Timeline& operator= (Timeline&);
	//! Copy constructor; it is private.
	Timeline (const Timeline&);
public:
	//! Class constructor.
	Timeline (SoundInfo* info, double st = 0);
	//! Class destructor.
	virtual ~Timeline ();
	//! Schedulation function.
	/*!
		To put a SoundEvent in the time line, just call this method. When the tick ()
		method will be called, it will check the status flag of each SoundEvent and
		the activation time.
	*/
	void schedule (SoundEvent* event);
	//! Returns the number of the sample currently elaborated.
	long currentSample () const;
	//! Returns the total duration in seconds.
	double totalDuration () const;
	//! Sound production function. 
	/*!
		It will call tick () function of every scheduled SoundEvent only if
		the status flag is set to true and if the activation time is
		coherent with the current time.
	*/
	virtual SoundFrame& tick ();
private:
	std::vector<SoundEvent*> m_events;
	long m_index;
};

#endif	// TIMELINE_H 

// EOF

