// DCBlocker.h
// 

#include "../include/DCBlocker.h"

DCBlocker::DCBlocker (SoundInfo* info, double gain) : AbstractFilter (info),
	m_gain (gain), m_cutoff (10) {
	reset ();
}

DCBlocker::~DCBlocker () {}

void DCBlocker::cutoff (double c) {
	m_cutoff = c;
	double theta = (2 * M_PI * m_cutoff) / m_info->samplingRate ();
	m_gain = 1 - theta;
}

void DCBlocker::gain (double g) { m_gain = g; }

void DCBlocker::reset () {
	m_val = 0;
	m_inputs = 0;
}

void DCBlocker::update (Subject* s) {
	double theta = (2 * M_PI * m_cutoff) / m_info->samplingRate ();
	m_gain = 1 - theta;		
}    

// EOF

