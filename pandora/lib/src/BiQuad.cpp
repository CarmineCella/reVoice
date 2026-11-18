// BiQuad.cpp
//

#include "../include/BiQuad.h"

BiQuad::BiQuad (SoundInfo* info, double cutoff, 
	double quality, Pandora::filterType type) : 
	AbstractFilter (info), m_cutoff (cutoff), 
	m_quality (quality), m_type (type) {
	calcCoeff ();
	reset ();
}

BiQuad::~BiQuad () {}
	
void BiQuad::cutoff (double c) { m_cutoff = c; calcCoeff (); }

void BiQuad::quality (double q) { m_quality = q; calcCoeff (); }

void BiQuad::type (Pandora::filterType t) { m_type = t; calcCoeff (); }

void BiQuad::reset () {
	m_in[0] = m_in[1] = m_in[2] = m_out[2] = m_out[1] = m_out[0] = 0;
}

double& BiQuad::process (double& sig) {
	m_in[0] = m_in[1];
	m_in[1] = m_in[2];
	m_in[2] = sig;

	m_out[0] = m_out[1];
	m_out[1] = m_out[2];
	m_out[2] = m_a0 * m_in[2] + m_a1 * m_in[1] + m_a2 * 
		m_in[0] - m_b1 * m_out[1] - m_b2 * m_out[0];
	return m_out[2];
}

void BiQuad::update (Subject* s) {
	if (s == m_info) {
		calcCoeff ();
	}
}

void BiQuad::calcCoeff () {
	if (m_type == Pandora::LOWPASS) {
		double c = 1.0 / tan (M_PI * m_cutoff / m_info->samplingRate ());
		m_a0 = 1.0 / ( 1.0 + m_quality * c + c * c);
		m_a1 = 2 * m_a0;
		m_a2 = m_a0;
		m_b1 = 2.0 * ( 1.0 - c * c) * m_a0;
		m_b2 = (1.0 - m_quality * c + c * c) * m_a0;
	} else {
		double c = tan (M_PI * m_cutoff / m_info->samplingRate ());
		m_a0 = 1.0 / (1.0 + m_quality * c + c * c);
		m_a1 = -2 * m_a0;
		m_a2 = m_a0;
		m_b1 = 2.0 * ( c * c - 1.0) * m_a0;
		m_b2 = (1.0 - m_quality * c + c * c) * m_a0;
	}
}

// EOF


