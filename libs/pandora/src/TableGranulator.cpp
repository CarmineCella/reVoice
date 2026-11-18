// TableGranulator.cpp
// 

#include "../include/TableGranulator.h"

TableGranulator::TableGranulator (SoundInfo* info, SignalTable* table, double start, 
	double duration, int maxStreams) 
	: AbstractGranulator (info, start, duration), m_maxStreams (maxStreams),
	m_perGrainAmp (.5),
	m_freq (440), m_randFreq (0), m_percFreq (0),
	m_gliss (440), m_randGliss (0), m_percGliss (0),
	m_density (1), m_randDensity (0), m_percDensity (0),
	m_speed (1), m_percSpeed (0), m_randSamples (0),
	m_type (Pandora::ASYNC), m_cpg (1), m_useJitter (true) {
	assert (table);
	m_rng.seed ((long) time (NULL));
	if (maxStreams > 0) {
		m_maxStreams = maxStreams;
	}
	else {
		m_maxStreams = 128;
	}
		
	m_jitters = new double[m_maxStreams];
	m_granulators = new GranularOscillator*[m_maxStreams];
	for (int i = 0; i < m_maxStreams; i++) {
		m_granulators[i] = new GranularOscillator (info, table);
		m_granulators[i]->amplitude (m_perGrainAmp);
		m_granulators[i]->grainType (Pandora::GAUSSIAN);
		m_granulators[i]->mute (true);
		m_jitters[i] = 0;
	}    

	m_pulse = new PulseGenerator (info);
	m_r1 = new RangedRandomNumberGenerator (info, -1, 1);
	m_r2 = new UnitRandomNumberGenerator (info);

	m_len = table->size ();

	grainDuration (1, 0);
	setSelection (0, 1);
	speedFactor (1, 0);
	calcOverlap ();
}

TableGranulator::~TableGranulator () {
	for (int i = 0; i < m_maxStreams; ++i) delete m_granulators[i];
	delete [] m_granulators;
	delete m_pulse;
	delete m_r1;
	delete m_r2;
	delete [] m_jitters;
}

void TableGranulator::frequency (double freq, double randFreq,
	double gliss, double randGliss) {
	m_freq = freq;
	m_randFreq = randFreq;
	m_percFreq = m_freq * m_randFreq * .5;

	m_gliss = gliss;
	m_randGliss = randGliss;
	m_percGliss = m_gliss * m_randGliss * .5;
	
	 for (int i = 0; i < m_overlap; i++) {
		m_granulators[i]->frequency (m_freq);
	 }
}

//double TableGranulator::maxUsedFrequency () const {
//	double a = (m_speed + m_percSpeed) * (m_freq + m_percFreq);
//	double b = (m_speed + m_percSpeed) * (m_gliss + m_percGliss);
//	
//	if (a >= b)
//		return a;
//	else
//		return b;
//}

void TableGranulator::grainEnvelope (Pandora::windowType w, double shift) {
	for (int i = 0; i < m_maxStreams; i++) {
	   m_granulators[i]->grainType (w, shift);
	}               
}

void TableGranulator::grainDirection (int sign) {
	for (int i = 0; i < m_maxStreams; i++) {
		m_granulators[i]->grainDirection (sign);
	}   
}

//double TableGranulator::maxGrainDuration () const {
//	return m_dur + m_percDur;
//}
	
void TableGranulator::speedFactor (double speed, long randSamples) {
	m_speed = speed;
	m_randSamples = randSamples;
	m_percSpeed = ((double) randSamples) * .5;
	m_percSpeed /= m_len;
	
	calcIncr ();
}

//long TableGranulator::jitterSamples () const {
//	return m_randSamples;
//}

void TableGranulator::setSelection (double phi0, double phiEnd) {
	if (phi0 > .999) {
		phi0 = .999;
	}
	if (phiEnd <= phi0) {
		phiEnd = phi0 + .001;
	}

	m_phi0 = phi0;
	m_phiEnd = phiEnd;
	m_deltaPhi = m_phiEnd - m_phi0;
	m_currentPhi = m_phi0;
	m_frozenPhi = m_phi0;
	m_cyclePhi = 0;
}

void TableGranulator::generation (Pandora::generationType t) { m_type = t; }

int TableGranulator::overlap () const { return m_overlap; }

void TableGranulator::table (SignalTable* tab) {
	assert (tab);
	for (int i = 0; i < m_maxStreams; i++) {
		m_granulators[i]->table (tab);
	}
	
	m_len = tab->size ();
	
	grainDuration (1, 0);
	setSelection (0, 1);
	speedFactor (1, 0);
	calcOverlap ();		
}

void TableGranulator::useJitter (bool b) {
	m_useJitter = b; 
}

void TableGranulator::reset () { 
	m_currentPhi = m_phi0;
	m_frozenPhi = m_phi0;
	m_cyclePhi = 0;
}

void TableGranulator::update (Subject* s) {
	speedFactor (m_speed, m_randSamples);
	calcCycle ();   
	AbstractGranulator::update (s); // calls the update func of the parent class
}

void TableGranulator::calcIncr () {
	m_phiIncr = m_speed;
	m_phiIncr /= ((double) m_len);
}


// EOF


