// VariState.cpp
// 

#include "../include/VariState.h"

VariState::VariState (SoundInfo* info, double cutoff, 
	double quality, Pandora::filterType t) 
	: AbstractFilter (info), m_type (t) {
	m_cutoff = std::isfinite (cutoff) && cutoff > 0 ? cutoff : 440;
	this->quality (std::isfinite (quality) && quality > 0 ? quality : .707);
	reset ();
	
	m_sicvt = M_PI / m_info->samplingRate ();
}

VariState::~VariState () {}

void VariState::quality (double q) { 
	if (std::isfinite (q) && q > 0) {
		m_q1 = 1 / q;	
		// The Chamberlin recurrence is stable only below this Q-dependent
		// frequency coefficient. Use the rationalized form to avoid cancellation.
		m_maxF = .99 * 4 / (std::sqrt (m_q1 * m_q1 + 4) + m_q1);
		cutoff (m_cutoff);
	}
}

void VariState::reset () { 
	m_data[Pandora::LOWPASS] = m_data[Pandora::HIGHPASS] = 
		m_data[Pandora::BANDPASS] = m_data[Pandora::NOTCH] = 0;    
}

void VariState::type (Pandora::filterType t) { m_type = t; }

void VariState::update (Subject* s) {
	cutoff (m_cutoff);
	m_sicvt = M_PI / m_info->samplingRate (); 
}

// EOF


