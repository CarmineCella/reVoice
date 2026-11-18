// PitchShifter.h
// 

#ifndef PITCHSHIFTER_H
#define PITCHSHIFTER_H 

#include "SignalTable.h"
#include "TableGranulator.h"
#include "AbstractPitchDetector.h"
#include "EnvelopeFollower.h"
#include "windowType.h"
#include "SoundInfo.h"
#include "SoundFrame.h"
#include "SoundEvent.h"
#include "RandomGenerators.h"
#include "VariState.h"
#include <cmath>
#include <cassert>

//! Pitch shifter based on synchronous granular synthesis.
class PitchShifter : public SoundEvent {
private:
	PitchShifter& operator= (PitchShifter&);
	PitchShifter (const PitchShifter&); 
public: 
	PitchShifter (SoundInfo* info, SignalTable* tab, AbstractPitchDetector* pdet, 
        double start, double duration, double ratio = 1);
	virtual ~PitchShifter ();
	virtual void amplitude (double a) { m_gran->grainAmplitude (a); }
	virtual void frequency (double f) {
		double t = f / m_old;
		ratio (t); 
	}
	virtual void ratio (double r) { 
		m_ratio = r; 
		if (m_ratio <= 0)
			m_ratio = 1;
		
		if (m_auto) {
			if (m_ratio >= 2)
				m_cycles = 1;
			else
				m_cycles = 2;
			m_gran->cyclesPerGrain (m_cycles);
		}    
	}
	virtual void freqThreshold (double t);
    virtual void vibrato (double amp, double freq);
    virtual void randomVibrato (/* double amp, */ double freq);
    virtual void formant (double f);    
	virtual void attackDecay (double a, double d);
	virtual void grainEnvelope (Pandora::windowType w);
    virtual void cyclesPerGrain (int cpg);
    virtual void pitchDetector (AbstractPitchDetector* p);
    virtual void autoMode (bool a);
	virtual void panning (double j);
 	virtual double pitch () const;
 	virtual void table (SignalTable* tab);
    virtual int overlap () const;
    virtual void freeze (bool f);
	virtual void highpass (bool hp);
	virtual void reset ();
	virtual SoundFrame& tick () {
		m_curr = m_pdet->get ();
	
		double vib = m_vib->tick ();
		if (m_curr > MIN_FREQ_LIMIT) {
			if (m_hp) {
				m_highpassLeft->cutoff (m_curr);
				m_highpassRight->cutoff (m_curr);		
			}	    
			double v = (double) m_cycles / m_curr;
			m_gran->grainDuration (v, 0, m_attack, m_decay);
			if ((fabs (m_curr - m_old) > fabs ((m_old * m_freqThreshold) - m_old))
				 && m_freeze == false) {
				m_gran->density (m_ratio * (m_curr + vib), 0);
				m_old = m_curr;
					
	//	    		double ramp = m_rnd->tick ();
	//			    ramp *= (m_randVibAmp * m_vibAmp);
				m_vib->amplitude (m_vibAmp);
				double rfreq = m_rnd->fastTick ();
				rfreq *= (m_randVibFreq);
				m_vib->frequency (m_vibFreq + rfreq);
			} 
		}
		
	//    *m_frame = m_gran->tick ();
		*m_frame = m_gran->fastTick ();
		if (m_hp) {
			double l = m_frame->get(Pandora::LEFT);
			double r = m_frame->get(Pandora::RIGHT);
			m_frame->set (Pandora::LEFT, m_highpassLeft->process (l));
			m_frame->set (Pandora::RIGHT, m_highpassRight->process (r));
		}
		return *m_frame;
	}
	
private:
    SignalTable* m_sineTab;
    LoopOscillator* m_vib;
    TableGranulator* m_gran;
    AbstractPitchDetector* m_pdet;
    VariState* m_highpassLeft;
    VariState* m_highpassRight;
    double m_fn;    
    double m_ratio, m_curr, m_old, m_freq;
    int m_cycles;
	double m_freqThreshold;
	double m_attack, m_decay;
	bool m_auto;
	bool m_hp;
	double m_vibAmp;
	double m_vibFreq;
	double m_randVibAmp;
	double m_randVibFreq;
	RangedRandomNumberGenerator* m_rnd;
	bool m_freeze;
	double m_formant;
};

#endif	// PITCHSHIFTER_H 

// EOF

