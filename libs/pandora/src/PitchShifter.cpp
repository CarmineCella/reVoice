// PitchShifter.cpp
// 

#include "../include/PitchShifter.h"

PitchShifter::PitchShifter (SoundInfo* info, SignalTable* tab, 
	AbstractPitchDetector* pdet, double start, double duration, double ratio)                    
	: SoundEvent (info, start, duration),  m_fn (1), m_ratio (ratio), m_curr (-1),
	m_old (0), m_freq (440), m_cycles (2), m_freqThreshold (1), m_attack (.5),
	m_decay (.5), m_auto (true), m_hp (false), m_vibAmp (0),
	m_vibFreq (4.5), m_randVibAmp (0), m_randVibFreq (0), m_freeze (false),
	m_formant (1) {
	assert (tab);
	m_fn = tab->naturalFreq (info->samplingRate ());
	
	m_gran = new TableGranulator (info, tab, start, duration, 8);
	m_gran->density (m_freq, 0); // default start-up condition
	m_gran->generation (Pandora::PITCH_SYNC);
	m_gran->grainDuration ((double) m_cycles / m_freq, 0);
	m_gran->cyclesPerGrain (m_cycles);
	m_gran->grainEnvelope (Pandora::HANNING);
	m_gran->frequency (m_fn * m_formant, 0, m_fn * m_formant, 0); // formant correction
	m_gran->speedFactor (1, 0);
	m_gran->grainAmplitude (.99);
	m_gran->setSelection (0, 1);
	m_gran->spatialPosition (.5);
	m_gran->useJitter (false);
	
	m_sineTab = new SignalTable (TABLEN / 2); // vibrato table
	m_sineTab->gen (1, 1);
	m_vib = new LoopOscillator (info, m_sineTab);
	m_vib->frequency (m_vibFreq);
	m_vib->amplitude (m_vibAmp);
	
	m_highpassLeft = new VariState (info);
	m_highpassLeft->type (Pandora::HIGHPASS);
	m_highpassLeft->cutoff (m_freq);
	m_highpassRight = new VariState (info);
	m_highpassRight->type (Pandora::HIGHPASS);
	m_highpassRight->cutoff (m_freq);
	
	m_pdet = pdet;
	m_rnd = new RangedRandomNumberGenerator (info, -.5, .5);
}

PitchShifter::~PitchShifter () {
	delete m_gran;
	delete m_vib;
	delete m_sineTab;
	delete m_highpassLeft;
	delete m_highpassRight;
	delete m_rnd;
}

void PitchShifter::freqThreshold (double t) { m_freqThreshold = t; }

void PitchShifter::vibrato (double amp, double freq) {
	m_vibAmp = amp;
	m_vibFreq = freq;
}

void PitchShifter::randomVibrato (/* double amp, */ double freq) {
	//m_randVibAmp = amp;
	m_randVibFreq = freq;  
}

void PitchShifter::formant (double f) {
	if (f >= .5 && f <= 2) {
		m_formant = f;
		m_gran->frequency (m_fn * m_formant, 0, m_fn * m_formant, 0);	
	}
}

void PitchShifter::attackDecay (double a, double d) {
	m_attack = a;
	m_decay = d;    	 
}

void PitchShifter::grainEnvelope (Pandora::windowType w) {
	m_gran->grainEnvelope (w); 
}

void PitchShifter::cyclesPerGrain (int cpg) {
	if (m_auto == false) { 
		m_gran->cyclesPerGrain (cpg);
	}		
}

void PitchShifter::pitchDetector (AbstractPitchDetector* p) {
	assert (p);
	m_pdet = p;    
}

void PitchShifter::autoMode (bool a) {
	m_auto = a;    
}

void PitchShifter::panning (double j) {
	m_gran->spatialPosition (j); 
}

double PitchShifter::pitch () const { return m_old; }

void PitchShifter::table (SignalTable* tab) {
	assert (tab);
	m_fn = tab->naturalFreq (m_info->samplingRate ());
	m_gran->table (tab);
	m_gran->frequency (m_fn * m_formant, 0, m_fn * m_formant, 0);
}
int PitchShifter::overlap () const { return m_gran->overlap (); }

void PitchShifter::freeze (bool f) {
	m_freeze = f;
}

void PitchShifter::highpass (bool hp) { m_hp = hp; }

void PitchShifter::reset () {
	m_gran->reset ();
}

// EOF

