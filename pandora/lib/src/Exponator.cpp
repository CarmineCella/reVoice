// Exponator.cpp
// created: Sun Jan 29 20:20:42 2006
// 

#include "../include/Exponator.h"

Exponator::Exponator (SoundInfo* info, double index, double frequency,
	double speed) : SoundEffect (info), m_expon (1) {
	m_pulse = new PulseGenerator (info, speed);
	m_envelope = new SignalTable ();
	m_envelope->gen (1, 1);
	m_osc = new LoopOscillator (info, m_envelope);
	m_osc->amplitude (index);
	m_index = index;
	m_osc->frequency (frequency);
}

Exponator::~Exponator () {
	delete m_pulse;
	delete m_osc;
	delete m_envelope;
}

double& Exponator::process (double& s) {
//	double curr = 
	if (m_pulse->tick ()) {
		m_expon = m_osc->tick () + m_index + 1; //curr + m_index;
//		TRACE (m_expon)
	}
	
	double f = pow ((double) s, m_expon);
	double a = pow (.5, m_expon);
	m_val = (f - a) / (1 - a);	
	return m_val;
}

void Exponator::index (double i) {
	m_osc->amplitude (i);
	m_index = i;
}

void Exponator::frequency (double s) {
	m_osc->frequency (s);
}

void Exponator::speed (double p) {
	m_pulse->frequency (p);
}

// EOF

