// PulseGenerator.h
// 

#include "../include/PulseGenerator.h"

PulseGenerator::PulseGenerator (SoundInfo* info, double freq, double phi) 
	: SoundUnit (info) {
	m_delta = ((double) 1 / m_info->samplingRate ());	
	m_freq = freq;
	m_incr = m_freq * m_delta;
	
	if (phi >= 0 && phi <= 1)
		m_phi = phi;
	else
		m_phi = 0;
	
	// to start with value 1
	m_flag = true;
}

PulseGenerator::~PulseGenerator () {}

void PulseGenerator::phase (double phi) {
	m_phi = phi;
}

void PulseGenerator::update (Subject* s) {
	if (s == m_info) {
		m_delta = ((double) 1 / m_info->samplingRate ());		
		m_incr = m_freq * m_delta;
	}
}

// EOF

