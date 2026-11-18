// TableGranulator.h
// 

#ifndef TABLEGRANULATOR_H
#define TABLEGRANULATOR_H 

#include "SignalTable.h"
#include "AbstractGranulator.h"
#include "LoopOscillator.h"
#include "PulseGenerator.h"
#include "GranularOscillator.h"
#include "RandomGenerators.h"
#include "generationType.h"
#include "windowType.h"
#include "SoundInfo.h"
#include "SoundFrame.h"
#include <time.h> // compatibility issues
#include <cassert>

//! Main class for granular synthesis.
/*!
	It can perform various types of granular synthesis, including
	asynchronous, pitch-synchronous, glissons, grainlets.
*/
class TableGranulator : public AbstractGranulator  {
private:
	TableGranulator& operator= (TableGranulator&);
	TableGranulator (const TableGranulator&);
public: 
	TableGranulator (SoundInfo* info, SignalTable* table, double start, double duration, 
        int maxStreams = 128);
	virtual ~TableGranulator ();
	virtual void grainAmplitude (double amp) { 
		m_perGrainAmp = amp; 
		for (int i = 0; i < m_maxStreams; i++) {
			m_granulators[i]->amplitude (m_perGrainAmp);
		}           
	}
    virtual void frequency (double freq, double randFreq,
        double gliss, double randGliss);
//	virtual double maxUsedFrequency () const;
	virtual void density (double pulsesPerSec, double random) {
		if (pulsesPerSec > 0) {
			m_density = (pulsesPerSec);
			m_randDensity = random; 
			m_percDensity = m_density * m_randDensity * .5;
			
			calcOverlap ();
			m_pulse->frequency (m_density);
		}    
	}
	virtual void attackDecay (double a, double d) {
		m_attack = a;
		m_decay = d;    	 
	}	
	virtual void grainDuration (double dur, double random, double attack = .5,
		double decay = .5) {
		m_dur = dur;
		m_randDur = random;
		m_percDur = m_dur * m_randDur * .5;
		m_attack = attack;
		m_decay = decay;
	
		calcCycle ();
		calcOverlap ();
		
		// FIXME: forse va commentato perche' credo che vada settata solo ad attacco grano in tick ()
		for (int i = 0; i < m_overlap; i++) {
			m_granulators[i]->grainDuration (m_dur);
		}
	}
//	virtual double maxGrainDuration () const;
    virtual void grainEnvelope (Pandora::windowType w, double shift = 0);
    virtual void grainDirection (int sign);
    virtual void speedFactor (double speed, long randSamples);
//	virtual long jitterSamples () const;
    virtual void setSelection (double phi0, double phiEnd); 
    virtual void generation (Pandora::generationType t);
	virtual void cyclesPerGrain (int cpg) { 
		m_cpg = cpg; 
		
		calcCycle ();
	}
    virtual int overlap () const;
    virtual void table (SignalTable* tab);
	virtual void useJitter (bool b);
 	virtual void reset ();
	virtual SoundFrame& tick () {
		if (!m_m) {
			double trigger = m_pulse->tick ();            
			if (trigger) {
				double r0 = m_r1->tick ();
				r0 *= m_percDensity;
				double realDens = m_density + r0;
				m_pulse->frequency (realDens);
		
				double r1 = m_r1->tick ();
				double r2 = m_r1->tick ();
				double r3 = m_r1->tick ();
				double r4 = m_r1->tick ();
				double r5 = m_r2->tick (); // between 0 and 1
				
				r1 *= m_percDur;
				r2 *= m_percFreq;
				double realFreq = m_freq + r2;
				r3 *= m_percGliss;
				r4 *= m_percSpeed;            
				r5 *= m_rightPosition;
		
				for (int i = 0; i < m_overlap; i++) {
					if (m_granulators[i]->mute ()) {
						m_granulators[i]->frequency (realFreq, m_gliss + r2 + r3);
						if (m_type == Pandora::ASYNC) {
							m_granulators[i]->grainDuration (m_dur + r1, 
								m_attack, m_decay);
							m_granulators[i]->phase (m_currentPhi + r4);
						}
						else if (m_type == Pandora::GRAINLET) {
							m_granulators[i]->grainDuration (m_cpg / realFreq,
								m_attack, m_decay);                        
							m_granulators[i]->phase (m_currentPhi + r4);
						}
						else {
							m_granulators[i]->grainDuration (m_dur + r1, 
								m_attack, m_decay);
							m_granulators[i]->phase (m_frozenPhi + r4);  
						 }           
						
						m_granulators[i]->mute (false);
						m_jitters[i] = r5;
						break;
					}
				}
			}
		
			double d = 0;
			double left = 0;
			double right = 0;
			for (int i = 0; i < m_overlap; i++) {
				if (!m_granulators[i]->mute ()) {
					d = m_granulators[i]->tick (); // not cumulative
					if (m_frame->m_stereo) {
						if (m_useJitter == true) {
							left += d * (m_jitters[i] + (1 - m_rightPosition));
							right += d * (1 - m_jitters[i]);
						}
						else {
							left += d * (1 - m_rightPosition);
							right += d * m_rightPosition;  
						}    
					}    
					else {
						left += d;
					}    
				}    
			} 
		
			m_cyclePhi += m_phiIncr;
			if (m_cyclePhi >= m_cycleLen) {
				m_cyclePhi = 0;
				m_frozenPhi = m_currentPhi;
			}
			else if (m_cyclePhi < 0) {
				m_cyclePhi = m_cycleLen;      
				m_frozenPhi = m_currentPhi;
			}
		
			m_currentPhi += m_phiIncr;
			if (m_currentPhi >= m_phiEnd) { 
				m_currentPhi -= m_deltaPhi;
			}
			else if (m_currentPhi < m_phi0) {
				m_currentPhi += m_deltaPhi;
			}
			
			m_frame->set (Pandora::LEFT, left);
			m_frame->set (Pandora::RIGHT, right);
		}
		return *m_frame;       
	}
    //! Generates sync-grains quickly. Only stereo.    	 
	virtual SoundFrame& fastTick () {
		double trigger = m_pulse->tick ();            
		if (trigger) {
			m_pulse->frequency (m_density);
	
			for (int i = 0; i < m_overlap; i++) {
				if (m_granulators[i]->mute ()) {
					m_granulators[i]->frequency (m_freq, m_freq);
					m_granulators[i]->grainDuration (m_dur, 
						m_attack, m_decay);
					m_granulators[i]->phase (m_frozenPhi);  
					m_granulators[i]->mute (false);
					break;
				}
			}
		}
	
		double d = 0;
		double left = 0;
		double right = 0;
		for (int i = 0; i < m_overlap; i++) {
			if (!m_granulators[i]->mute ()) {
				d = m_granulators[i]->fastTick (); // not cumulative
				left += d * (1 - m_rightPosition);
				right += d * m_rightPosition;    
			}    
		} 
	
		m_cyclePhi += m_phiIncr;
		if (m_cyclePhi >= m_cycleLen) {
			m_cyclePhi = 0;
			m_frozenPhi = m_currentPhi;
		}
		else if (m_cyclePhi < 0) {
			m_cyclePhi = m_cycleLen;      
			m_frozenPhi = m_currentPhi;
		}
	
		m_currentPhi += m_phiIncr;
		if (m_currentPhi >= m_phiEnd) { 
			m_currentPhi -= m_deltaPhi;
		}
		else if (m_currentPhi < m_phi0) {
			m_currentPhi += m_deltaPhi;
		}
		
		m_frame->set (Pandora::LEFT, left);
		m_frame->set (Pandora::RIGHT, right);
		return *m_frame;       
	} 	
    virtual void update (Subject* s);
private:
    PulseGenerator* m_pulse;
    SignalTable* m_t1;
    GranularOscillator** m_granulators;
    PRNG_31bit m_rng;
    RangedRandomNumberGenerator* m_r1;
    UnitRandomNumberGenerator* m_r2;
    int m_maxStreams;
    double m_perGrainAmp;
    double m_freq, m_randFreq, m_percFreq;
    double m_gliss, m_randGliss, m_percGliss;
    double m_density, m_randDensity, m_percDensity;
    double m_speed, m_percSpeed;
    long m_randSamples;
    double m_dur, m_randDur, m_percDur, m_attack, m_decay; // grainDuration
    double m_phi0, m_phiEnd, m_deltaPhi; // setSelection
    int m_overlap; // calcOverlap
    double m_currentPhi, m_frozenPhi, m_phiIncr, m_cyclePhi; //  setSelection
    double m_cycleLen; // calcCycle
    double* m_jitters; // calculated in constructor
    Pandora::generationType m_type;
    int m_cpg;
    long m_len;  // calculated in constructor
    bool m_useJitter;

    //! Private method to calculate the overlap factor.
    /*! 
        The overlap factor is calculated with the following formula:
            overlap = (density + (density * randomDensity)) *
                (dur + (dur * randomDur))
        and is rounded by adding 2.
    */
	void calcOverlap () {
		m_overlap = (int) (((m_density + (m_density * m_randDensity)) * 
			(m_dur + (m_dur * m_randDur))) + 2);
		if (m_overlap > m_maxStreams)
			m_overlap = m_maxStreams;
		else if (m_overlap <= 0)
			m_overlap = 1;
	}
	void calcCycle () {
		m_cycleLen = m_dur / m_cpg;
		m_cycleLen *= m_info->samplingRate ();
		m_cycleLen /= m_len;
	}
    void calcIncr ();
};

#endif	// TABLEGRANULATOR_H 

// EOF


