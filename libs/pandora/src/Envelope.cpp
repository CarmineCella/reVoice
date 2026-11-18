 // Envelope.cpp
// 

#include "../include/Envelope.h"

Envelope::Envelope (SoundInfo* info, long len) : SignalTable (len), SoundUnit (info), 
	m_n (0) {
	m_osc = new LoopOscillator (info, this);
	m_osc->loop (false); // not looping oscillator
	m_sr = m_info->samplingRate ();
	mapTo (1);
}

Envelope::~Envelope () {
	delete m_osc;
}

void Envelope::shift (double amount) {
	if (amount < 0) {
		amount = 0;
	}
	if (amount >= 1) {
		amount = .999;
	}
	
	long samples = (long) ((double) m_l * amount);
	double* temp = new double[m_l];
	
	for (long j = 0; j < m_l; j++) {
		long k = (j + samples) % m_l;
		temp[j] = m_t[k];
	}
	for (long j = 0; j < m_l; j++) {
		m_t[j] = temp[j];
	}
	
	delete [] temp;
}

void Envelope::update (Subject* s) {
	m_sr = m_info->samplingRate ();
	
	long samples = (long) (m_sr * m_dur);
	
	m_attackFreq = .5 / (m_dur * m_attack); // half table
	m_attackSamples = (long) ((m_dur * m_attack) * m_sr);
	 
	m_decayFreq = .5 / (m_dur * m_decay); // half table
	long decaySamples = (long) ((m_dur * m_decay) * m_sr);
	
	m_susSamples = samples - decaySamples;
}

// EOF

