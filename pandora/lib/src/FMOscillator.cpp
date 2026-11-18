// FMOscillator.cpp
// 

#include "../include/FMOscillator.h"

FMOscillator::FMOscillator (SoundInfo* info, SignalTable *tab, double amp, double carr, 
	 double ind, double mod, double phi) : AbstractOscillator (info) {
	m_osc1 = new LoopOscillator (info, tab);
	m_osc2 = new LoopOscillator (info, tab);
		 
	m_carr = carr;
	m_mod = mod;
	m_ind = ind;

	m_osc1->amplitude (m_ind * m_mod);
	m_osc1->frequency (m_mod);
	m_osc2->amplitude (amp);
}

FMOscillator::~FMOscillator () {
	delete m_osc1;
	delete m_osc2;
}

void FMOscillator::amplitude (double amp) {
	m_osc1->amplitude (amp);
}

void FMOscillator::frequency (double carr) {
	m_carr = carr;
}

void FMOscillator::index (double ind) {
	m_ind = ind;
	m_osc2->amplitude (m_ind * m_mod);
}

void FMOscillator::modulator (double mod) {
	m_mod = mod;
	m_osc2->frequency (m_mod);
	m_osc2->amplitude (m_ind * m_mod);
}

void FMOscillator::phase (double phi) {
	m_osc1->phase (phi);
	m_osc2->phase (phi);
}

double FMOscillator::phase () const {
	return m_osc2->phase ();
}

void FMOscillator::table (SignalTable* tab) {
	m_osc1->table (tab);
	m_osc2->table (tab);
}

double& FMOscillator::tick () {
	double f = m_osc1->tick ();
	m_osc2->frequency (f + m_carr);
	m_val = m_osc2->tick ();
	return m_val;
}

void FMOscillator::update (Subject* s) {}

// EOF

// FIXME: Forse non funziona correttamente!

