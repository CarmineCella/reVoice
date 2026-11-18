// Timeline.cpp
// 

#include "../include/Timeline.h"

Timeline::Timeline (SoundInfo* info, double st) : SoundEvent (info, st, 1) { 
	start (st); m_index = 0;
}

Timeline::~Timeline () {
	for (unsigned int i = 0; i < m_events.size (); i++) {
		delete m_events[i];
	}
	
	m_events.clear ();
}

void Timeline::schedule (SoundEvent* event) {
	m_events.push_back (event);
	double d = event->duration ();
	double s = event->start ();
	if ((d + s) > this->duration ())
		this->duration (d + s);
}

long Timeline::currentSample () const {
	return m_index;
}

double Timeline::totalDuration () const {
	return this->duration ();
}

SoundFrame& Timeline::tick () {
	m_frame->clear ();
	for (unsigned int i = 0; i < m_events.size (); i++) {
		if (!m_events[i]->mute () && m_events[i]->isActive (m_index))
			*m_frame += m_events[i]->tick ();
	}		

	m_index++;
	return *m_frame;
}

// EOF

