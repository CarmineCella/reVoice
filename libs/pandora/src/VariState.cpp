// VariState.cpp
// 

#include "../include/VariState.h"

VariState::VariState (SoundInfo* info, double cutoff, 
	double quality, Pandora::filterType t) 
	: AbstractFilter (info), m_type (t) {
	if (cutoff > 0) {
		m_cutoff = cutoff;
		m_f1 = 2 * sin (M_PI * m_cutoff / info->samplingRate ());
	}
	else {
		m_f1 = 2 * sin (M_PI * 440 / info->samplingRate ());
	}
	
	if (quality > 0) {
		m_q1 = 1 / quality;
	}
	else {
		m_q1 = 1 / .707;
	}
	reset ();
	
	m_sicvt = M_PI / m_info->samplingRate ();
}

VariState::~VariState () {}

void VariState::quality (double q) { 
	if (q > 0) {
		m_q1 = 1 / q;	
	}
}

void VariState::reset () { 
	m_data[Pandora::LOWPASS] = m_data[Pandora::HIGHPASS] = 
		m_data[Pandora::BANDPASS] = m_data[Pandora::NOTCH] = 0;    
}

void VariState::type (Pandora::filterType t) { m_type = t; }

void VariState::update (Subject* s) {
	m_f1 = 2 * sin (M_PI * m_cutoff / m_info->samplingRate ());
	m_sicvt = M_PI / m_info->samplingRate (); 
}

// EOF


