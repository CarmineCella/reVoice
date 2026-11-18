// Tone.cpp
//

#include "../include/Tone.h"

Tone::Tone (SoundInfo* info, double feedback) : 
	AbstractFilter (info), m_cutoff (440) {
	m_feedback = feedback;
	m_gain = 1 - m_feedback;
	reset ();
}

Tone::~Tone () {}
	
void Tone::cutoff (double c) {
	m_cutoff = c;
	double theta = (2 * M_PI * m_cutoff) / m_info->samplingRate ();
	m_feedback = 2 - cos (theta) - sqrt (pow (2 - cos (theta), 2) - 1);
	m_gain = 1 - m_feedback;
}

void Tone::feedback (double f) { m_feedback = f; m_gain = 1 - m_feedback; }

void Tone::reset () {
	m_val = 0;
}

double& Tone::process (double& sig) {
	m_val = m_val * m_feedback + sig * m_gain;
	return m_val;
}

void Tone::update (Subject* s) {
	double theta = (2 * M_PI * m_cutoff) / m_info->samplingRate ();
	m_feedback = 2 - cos (theta) - sqrt (pow (2 - cos (theta), 2) - 1);
	m_gain = 1 - m_feedback;
}

// EOF


