// ZeroCrossings.cpp
// 

#include "../include/ZeroCrossings.h"

ZeroCrossings::ZeroCrossings (SoundInfo* info, double feedback) : 
	Tone (info, feedback) {
	m_rate = (double) 1 / info->samplingRate ();
	reset ();
}
ZeroCrossings::~ZeroCrossings () {}

void ZeroCrossings::reset (double z) {
	m_val = 0;
	m_z = 0;
	m_posit = 0;
	m_negat = 0;
}

void ZeroCrossings::update (Subject* s) {
	if (s == m_info) {
		m_rate = ((double) 1 / m_info->samplingRate ());		
	}
	// don't call Tone::update () !
}	

// EOF

