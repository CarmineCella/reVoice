// LoopOscillator.cpp
//

#include "../include/LoopOscillator.h"

LoopOscillator::LoopOscillator (SoundInfo* info, SignalTable *tab, double amp, 
	double freq, double phi) :
	AbstractOscillator (info), m_freq (freq), m_loop (true) {
	table (tab);
	amplitude (amp);
	frequency (freq);
	phase (phi);
}
LoopOscillator::~LoopOscillator () {}

double LoopOscillator::frequency () const {
	return m_freq;
}
double LoopOscillator::phase () const {
	return m_phi / m_tab->size ();
}

void LoopOscillator::table (SignalTable *tab) {
	m_tab = tab;
	m_l = tab->size ();
	m_sicvt = ((double) m_l / m_info->samplingRate ());
	m_incr = m_sicvt * m_freq;
	m_phi = 0;
}

void LoopOscillator::loop (const bool l) {
	m_loop = l;
}

bool LoopOscillator::loop () const { return m_loop; }

void LoopOscillator::update (Subject* s) {
	if (s == m_info) {
		m_sicvt = ((double) m_l / m_info->samplingRate ());
		m_incr = m_sicvt * m_freq;		
	}
}

// EOF

