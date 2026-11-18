// GranularOscillator.cpp
// 

#include "../include/GranularOscillator.h"

GranularOscillator::GranularOscillator (SoundInfo* info, SignalTable *tab, double amp, 
	double initFreq, double finalFreq, double curve,
	double grainDur, double phi) 
	: AbstractOscillator (info), m_centerFreq (initFreq), 
	m_deltaFreq (0), m_grainDirection (1) {
		m_env = new Envelope (info, TABLEN);
		m_env->gauss (1);
		m_env->mapTo (grainDur);
		m_glissGen = new Envelope (info, TABLEN);
		m_glissGen->drawSegment (0, 0, TABLEN - 1, 1, curve);
		m_glissGen->mapTo (grainDur);
		m_osc = new LoopOscillator (info, tab, amp, initFreq);
}

GranularOscillator::~GranularOscillator () {
	delete m_env;
	delete m_glissGen;
	delete m_osc;
 }

double GranularOscillator::phase () const {
	return m_osc->phase ();
}

void GranularOscillator::table (SignalTable* tab) {
	m_osc->table (tab);
}

void GranularOscillator::grainType (Pandora::windowType w, double shift) {
	m_env->clear ();
	switch (w) {
		case Pandora::RECTANGLE:
			m_env->rectangle (1);
		break;
		case Pandora::BARTLETT:
			m_env->bartlett (1);
		break;
		case Pandora::GAUSSIAN:
			m_env->gauss (1);
		break;
		case Pandora::HAMMING:
			m_env->hamming (1);
		break;
		case Pandora::HANNING:
			m_env->hanning (1);
		break;
		case Pandora::BLACKMAN:
			m_env->blackman (1);
		break;
		case Pandora::BLACKMAN_HARRIS:
			m_env->blackmanHarris (1);
		break;
		case Pandora::ADSR:
			m_env->adsr (1, .5, 0);
		break;
	}
	
	if (shift != 0) {
		m_env->shift (shift);
	}
}

void GranularOscillator::curve (double c) {
	m_glissGen->clear();
	m_glissGen->drawSegment (0, 0, TABLEN - 1, 1, c);    
}

void GranularOscillator::grainDirection (int sign) { 
	if (sign >= 0)
		m_grainDirection = 1; 
	else
		m_grainDirection = -1;
}

void GranularOscillator::update (Subject* s) {}

// EOF

