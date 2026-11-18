// Harmonizer.cpp
// 

#include "../include/Harmonizer.h"

Harmonizer::Harmonizer (SoundInfo* info, SignalTable* tab, 
	AbstractPitchDetector* pdet,
	double start, double duration) 
	: SoundEvent (info, start, duration),
	m_logic (Pandora::MINOR_SEVENTH), m_style (Pandora::SMALL_VIBRATO), m_randoms (Pandora::SMALL_RANDOM),
	m_disposition (Pandora::TWO_TWO), m_choral (Pandora::CUSTOM_CHORAL),
	m_realFreq (false), m_antiRumble (false), m_sourceLevel (.5), 
	m_harmonyLevel (1), m_bypass (false), m_masterKey (Pandora::C), m_scaleSize (0),
	m_scaleType (Pandora::MAJOR1), m_curr (-1), m_old (0), m_freeze (false), 
	m_indexCopy (-1) {
//   		if (pdets.size () < MAX_VOICES)
//     		throw std::runtime_error ("invalid pitch detectors array"); 

	m_fundamental = 1;
	m_pshifts = new PitchShifter*[MAX_VOICES];
	for (int i = 0; i < MAX_VOICES; i++) {
		m_ratios[i] = 1;
		m_amps[i] = .25;
		m_vibFreqs[i] = 4 + ((double) i * 0.11);
		m_vibAmps[i] = 0;
		m_randVibFreqs[i] = 0;
		m_randVibAmps[i] = 0;
		m_pshifts[i] = new PitchShifter (info, tab, pdet,
			start, duration);
		m_pshifts[i]->ratio (m_ratios[i]);
		m_pshifts[i]->amplitude (m_amps[i]);
		m_activeVoices[i] = true;
		m_pannings[i] = .5;
		m_formants[i] = 1;
		m_pshifts[i]->formant (m_formants[i]);		
	}   		  
	
	double fn = tab->naturalFreq (info->samplingRate ());
	m_osc = new LoopOscillator (info, tab);
	m_osc->frequency (fn);  
	m_pdet = pdet;
	
	m_rumble = new VariState (info);
	m_rumble->type (Pandora::HIGHPASS);
	m_rumble->cutoff (80);
	
	m_masterPitch = m_pgrid.get (m_masterKey, 4);
	scale (m_scaleType);
	logic (m_logic);
	style (m_style);
	randomStyle (m_randoms);
	disposition (m_disposition);
	choral (m_choral);
	
//	m_usample = 16384;
//	m_ucurr = 0;
}

Harmonizer::~Harmonizer () {
	delete [] m_pshifts;
	delete m_osc;
	delete m_rumble;
}

void Harmonizer::fundamental (double f, bool realFreq) {
	m_fundamental = f; 
	m_realFreq = realFreq;
	setRatios ();  
	updateRatios (m_indexCopy);
}

void Harmonizer::logic (Pandora::harmonizationLogic l) {
	m_logic = l;
	setRatios ();  
}

Pandora::harmonizationLogic Harmonizer::logic () const {
	return m_logic;
}

void Harmonizer::style (Pandora::styles s) {
	m_style = s;
	setStyle ();   
}

void Harmonizer::randomStyle (Pandora::randoms r) {
	m_randoms = r;
	setRandStyle ();   
}

void Harmonizer::disposition (Pandora::dispositions d) {
	m_disposition = d;
	setDisposition ();    
}

void Harmonizer::choral (Pandora::chorals c) {
	m_choral = c;
	setChoral ();    
}

void Harmonizer::status (int voice, bool active) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}
	m_activeVoices[voice] = active; 
}

void Harmonizer::highpass (int voice, bool hp) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}
	m_pshifts[voice]->highpass (hp);		
}

void Harmonizer::ratios (int voice, double amp, double ratio, bool realFreq) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}
	m_ratios[voice] = ratio;
	m_amps[voice] = amp;
	m_pshifts[voice]->amplitude (amp);
	//m_realFreq = realFreq;
	if (realFreq == false)
		m_pshifts[voice]->ratio (ratio * m_fundamental);  
	else
		m_pshifts[voice]->frequency (ratio * m_fundamental);
}

void Harmonizer::vibrato (int voice, double amp, double freq) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}        
	m_vibAmps[voice] = amp;
	m_vibFreqs[voice] = freq;
	m_pshifts[voice]->vibrato (amp, freq);        
}

void Harmonizer::randomVibrato (int voice, /* double amp, */ double freq) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}        
	//m_randVibAmps[voice] = amp;
	m_randVibFreqs[voice] = freq;
	m_pshifts[voice]->randomVibrato (/* amp,*/ freq);        
}

void Harmonizer::panning (int voice, double p) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}        
	m_pannings[voice] = p;
	m_pshifts[voice]->panning (p);
}

void Harmonizer::formant (int voice, double p) {
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}        
	m_formants[voice] = p;
	m_pshifts[voice]->formant (p);
}

void Harmonizer::scale (Pandora::scales s) {
	clearScale ();
	switch (s) {
		case Pandora::MAJOR1: // major
			m_scale.push_back (1); // 0
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL)); // 1
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)); // 2
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)); // 3
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)); // 4
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL)); // 5
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL)); // 6
		break;
		case Pandora::MAJOR2: // lydian
			m_scale.push_back (1); 
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::TRITONE_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL));
		break;
		case Pandora::MINOR1: // natural
			m_scale.push_back (1); 
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL));
		break;
		case Pandora::MINOR2: // melodic
			m_scale.push_back (1); 
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL));
		break;
		case Pandora::MINOR3: // harmonic
			m_scale.push_back (1); 
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL));
			m_scale.push_back (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL));
		break;
		case Pandora::BLUES1:
			m_scale.push_back (1);  // 0 
			m_scale.push_back (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)); // 1
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)); // 2
			m_scale.push_back (m_tempered.get (Pandora::TRITONE_INTERVAL)); // 3
			m_scale.push_back (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)); // 4
			m_scale.push_back (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL)); // 5
		break;
//    		case OCTOPHONIC1: // ht - t
//   		   		m_scale.push_back (1); 
//				m_scale.push_back (m_tempered.get (MINOR_SECOND_INTERVAL));
//				m_scale.push_back (m_tempered.get (MINOR_THIRD_INTERVAL));
//				m_scale.push_back (m_tempered.get (MAJOR_THIRD_INTERVAL));
//				m_scale.push_back (m_tempered.get (TRITONE_INTERVAL));
//				m_scale.push_back (m_tempered.get (PERFECT_FIFTH_INTERVAL));
//				m_scale.push_back (m_tempered.get (MAJOR_SIXTH_INTERVAL));
//				m_scale.push_back (m_tempered.get (MINOR_SEVENTH_INTERVAL));
//			break;
	}
	
	m_scaleType = s;
	m_scaleSize = m_scale.size ();
	updateRatios (m_indexCopy);
}

void Harmonizer::addScaleRatio (double r) { 
	m_scale.push_back (r); 
	m_scaleSize = m_scale.size ();
}

void Harmonizer::clearScale ()  { 
	m_scale.clear (); 
	m_scaleSize = 0;
}

//	void Harmonizer::temperedTuning (bool t) { m_tempered.temperedTuning (t); }

void Harmonizer::freqThreshold (double t) { 
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->freqThreshold (t);   
	} 
}

void Harmonizer::attackDecay (double a, double d) {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->attackDecay (a, d);   
	}    	 
}

void Harmonizer::grainEnvelope (Pandora::windowType w) {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->grainEnvelope (w);   
	} 
}

void Harmonizer::cyclesPerGrain (int cpg) {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->cyclesPerGrain (cpg);   
	} 	
}

void Harmonizer::pitchDetector (AbstractPitchDetector* pdet) {
//   		if (pdets.size () < MAX_VOICES)
//     		throw std::runtime_error ("invalid pitch detectors array");
	
	assert (pdet);
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->pitchDetector (pdet);   
	}      
	m_pdet = pdet;
}

void Harmonizer::autoMode (bool a) {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->autoMode (a);   
	}   
}

double Harmonizer::pitch () const { return m_pshifts[0]->pitch (); }

void Harmonizer::table (SignalTable* tab) {
	assert (tab);
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->table (tab);
	}   
	m_osc->table (tab);
	double fn = tab->naturalFreq (m_info->samplingRate ());
	m_osc->frequency (fn);
}

int Harmonizer::overlap (int voice) const { 
	if (voice >= MAX_VOICES || voice < 0) {
		voice = 0;
	}  		
	return m_pshifts[voice]->overlap (); 
}

//void Harmonizer::smoothingRate (double f) {
//	m_pdet->smoothingRate (f);
//}
//
//void Harmonizer::smoothingQuality (double f) {
//	m_pdet->smoothingQuality (f);
//}

void Harmonizer::antiRumble (bool a) {
	m_antiRumble = a;
	if (a == true) {
		m_rumble->reset ();
	}   	
}

void Harmonizer::sourceLevel (double l) {
	m_sourceLevel = l;    
}

void Harmonizer::harmonyLevel (double l) {
	m_harmonyLevel = l;    
}

void Harmonizer::bypass (bool b) {
	m_bypass = b;
}

void Harmonizer::freeze (bool f) {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->freeze (f); 
	}    
	m_freeze = f;
}

void Harmonizer::masterKey (Pandora::notes m) { 
	m_masterKey = m; 
	m_masterPitch = m_pgrid.get (m_masterKey, 4);
	m_old = -1; // forces to update
}

int Harmonizer::lastTone () const { return m_indexCopy; }

int Harmonizer::lastNote () const {
	double p = m_pshifts[0]->pitch ();
	if (p > MIN_FREQ_LIMIT) {
		// FIXME: cl non riconosce log2!!!
		//return (int) (69 + (double) 12 * log (p / 440));
		//return (int) (440.0 * exp ((note - 69) * (LN2 / 12)));
		return (int) ((12 * log (p / 220.0) / log (2.0)) + 57.01);
	}
	else return 0;
}

void Harmonizer::reset () {
	for (int i = 0; i < MAX_VOICES; i++) {
		m_pshifts[i]->reset ();
	}
	m_rumble->reset ();
	m_osc->phase (0);
	m_old = -1; // forces to update
	updateRatios (m_indexCopy);
}

int Harmonizer::voices () const {
	int count = 0;
	for (int i = 0; i < MAX_VOICES; i++) {		
		if (m_activeVoices[i] == true) {
			count++;
		}
	}
	
	return count;
}

void Harmonizer::setRatios () {
	switch (m_logic) {
		case Pandora::CUSTOM_CHORD:
			for (int i = 0; i < MAX_VOICES; i++) {
				if (m_realFreq == true) {
					m_pshifts[i]->frequency (m_ratios[i] * m_fundamental);  
				}
				else {
					m_pshifts[i]->ratio (m_ratios[i] * m_fundamental); 
				}   	
			}
		break;
		case Pandora::DOMINANT_SEVENTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					* m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
					* m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}  
		break;
		case Pandora::MINOR_SEVENTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) 
					* m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}    
		break;
		case Pandora::MAJOR_SEVENTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
					* m_fundamental);
			}      
		break;
		case Pandora::DIMINISHED_SEVENTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
					 * m_fundamental);
			}      
		break;
		case Pandora::AUGMENTED_SEVENTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
					* m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
					 * m_fundamental);
			}     
		break;
		case Pandora::QUARTAL_CHORD:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) 
					 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL)  / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
					* m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
			}     
		break;
		// ... //
		case Pandora::INTELLICHORD:
		break;
		case Pandora::SCALIC:
		break;
		// ... //
		case Pandora::MINOR_WITH_SIXTH:
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[1]->frequency (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) 
					 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					 * m_fundamental);
				m_pshifts[3]->frequency (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
					 * m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[1]->ratio (m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
					 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2 
					* m_fundamental);
				m_pshifts[3]->ratio (m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
					 * m_fundamental);
			}     
		break;
		case Pandora::SINGLECHORD:
		break;		
	} 
  
	m_pshifts[0]->amplitude (.25);    
	m_pshifts[1]->amplitude (.35);    
	m_pshifts[2]->amplitude (.45);    
	m_pshifts[3]->amplitude (.50);    
}
void Harmonizer::setStyle () {
	switch (m_style) {
		case Pandora::CUSTOM_VIBRATO:
			for (int i = 0; i < MAX_VOICES; i++) {
				m_pshifts[i]->vibrato (m_vibAmps[i], m_vibFreqs[i]); 
			}   	        	    
		break;
		case Pandora::SMALL_VIBRATO:
			m_pshifts[0]->vibrato (2.5, 4.31);
			m_pshifts[1]->vibrato (2.6, 4.23);
			m_pshifts[2]->vibrato (2.7, 3.92);
			m_pshifts[3]->vibrato (2.8, 3.76);
		break;
		case Pandora::MEDIUM_VIBRATO:
			m_pshifts[0]->vibrato (3.2, 3.65);
			m_pshifts[1]->vibrato (3.3, 3.21);
			m_pshifts[2]->vibrato (3.4, 2.98);
			m_pshifts[3]->vibrato (3.5, 2.31);
		break;
		case Pandora::LARGE_VIBRATO:
			m_pshifts[0]->vibrato (5.1, 2.54);
			m_pshifts[1]->vibrato (5.2, 2.51);
			m_pshifts[2]->vibrato (5.3, 2.12);
			m_pshifts[3]->vibrato (5.4, 1.97);
		break;
	}        
}
void Harmonizer::setRandStyle () {
	switch (m_randoms) {
		case Pandora::CUSTOM_RANDOM:
			for (int i = 0; i < MAX_VOICES; i++) {
				m_pshifts[i]->randomVibrato (/* m_randVibAmps[i],  */
					m_randVibFreqs[i]); 
			}   	        	    
		break;
		case Pandora::SMALL_RANDOM:
			m_pshifts[0]->randomVibrato (/* .1, */ 5.2);
			m_pshifts[1]->randomVibrato (/* .21, */ 4.15);
			m_pshifts[2]->randomVibrato (/* .19, */ 3.23);
			m_pshifts[3]->randomVibrato (/* .05, */ 2.11);
		break;
		case Pandora::MEDIUM_RANDOM:
			m_pshifts[0]->randomVibrato (/* .32, */ 11.26);
			m_pshifts[1]->randomVibrato (/* .35, */ 9.31);
			m_pshifts[2]->randomVibrato (/* .34, */ 8.41);
			m_pshifts[3]->randomVibrato (/* .31, */ 8.34);
		break;
		case Pandora::LARGE_RANDOM:
			m_pshifts[0]->randomVibrato (/* .51, */ 16.4);
			m_pshifts[1]->randomVibrato (/* .62, */ 15.7);
			m_pshifts[2]->randomVibrato (/* .73, */ 14.8);
			m_pshifts[3]->randomVibrato (/* .84,  */13.9);
		break;
	}        
}
void Harmonizer::setDisposition () {
	switch (m_disposition) {
		case Pandora::CUSTOM_POSITION:
			for (int i = 0; i < MAX_VOICES; i++) {
				m_pshifts[i]->panning (m_pannings[i]); 
			}   	        	    
		break;
		case Pandora::TWO_TWO:
			m_pshifts[0]->panning (0);
			m_pshifts[1]->panning (.2);
			m_pshifts[2]->panning (.9);
			m_pshifts[3]->panning (.5);
		break;
		case Pandora::ONE_THREE:
			m_pshifts[0]->panning (.1);
			m_pshifts[1]->panning (.8);
			m_pshifts[2]->panning (.9);
			m_pshifts[3]->panning (1);
		break;
		case Pandora::THREE_ONE:
			m_pshifts[0]->panning (0);
			m_pshifts[1]->panning (.1);
			m_pshifts[2]->panning (.3);
			m_pshifts[3]->panning (1);
		break;
	}        
}

void Harmonizer::setChoral () {
	switch (m_choral) {
		case Pandora::CUSTOM_CHORAL:
			for (int i = 0; i < MAX_VOICES; i++) {
				m_pshifts[i]->formant (m_formants[i]); 
			}   	        	    
		break;
		case Pandora::MIXED_CHORAL:
			m_pshifts[0]->formant (.95);
			m_pshifts[1]->formant (.97);
			m_pshifts[2]->formant (1.08);
			m_pshifts[3]->formant (1.12);
		break;
		case Pandora::FEMALE_CHORAL:
			m_pshifts[0]->formant (1.05);
			m_pshifts[1]->formant (1.1);
			m_pshifts[2]->formant (1.2);
			m_pshifts[3]->formant (1.3);
		break;
		case Pandora::MALE_CHORAL:
			m_pshifts[0]->formant (.6);
			m_pshifts[1]->formant (.7);
			m_pshifts[2]->formant (.8);
			m_pshifts[3]->formant (.95);
		break;
	}        
}

// EOF

