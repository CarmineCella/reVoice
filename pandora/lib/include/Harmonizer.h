// Harmonizer.h
// 

#ifndef HARMONIZER_H
#define HARMONIZER_H 

#include "harmonizationLogic.h"
#include "styles.h"
#include "randoms.h"
#include "dispositions.h"
#include "chorals.h"
#include "SignalTable.h"
#include "PitchShifter.h"
#include "AbstractPitchDetector.h"
#include "VariState.h"
#include "SoundEvent.h"
#include "TemperedIntervals.h"
#include "PitchGrid.h"
#include "scales.h"
#include <vector>
#include <stdexcept>
#include <cassert>
#include <cmath>

#define MAX_VOICES 4
#define TUNINGSEMITONE 1.059463094359
#define	LN2 0.69314718055994530941723212145818
              
//! Signal harmonizer.
/*!
	This is a complex class that uses virtually all other classes in the
	framework. It is based on synchronous granular synthesis and can create
	up to 4 different voices.
*/
class Harmonizer : public SoundEvent {
private:
	Harmonizer& operator= (Harmonizer&);
	Harmonizer (const Harmonizer&);
public: 
	Harmonizer (SoundInfo* info, SignalTable* tab, 
 		AbstractPitchDetector* pdet,
   		double start, double duration);
	virtual ~Harmonizer ();
  	virtual void fundamental (double f, bool realFreq = false);
 	virtual void logic (Pandora::harmonizationLogic l);
  	virtual Pandora::harmonizationLogic logic () const;
  	virtual void style (Pandora::styles s);
  	virtual void randomStyle (Pandora::randoms r);
   	virtual void disposition (Pandora::dispositions d);
	virtual void choral (Pandora::chorals c);   	
   	virtual void status (int voice, bool active);
    virtual void highpass (int voice, bool hp);
    virtual void ratios (int voice, double amp, double ratio, 
		bool realFreq = false);
    virtual void vibrato (int voice, double amp, double freq);
    virtual void randomVibrato (int voice, /* double amp, */ double freq);
    virtual void panning (int voice, double p);
	virtual void formant (int voice, double p);    
    virtual void scale (Pandora::scales s);
	virtual void addScaleRatio (double r);
	virtual void clearScale ();
//	void temperedTuning (bool t);
	virtual void freqThreshold (double t);
	virtual void attackDecay (double a, double d);
	virtual void grainEnvelope (Pandora::windowType w);
    virtual void cyclesPerGrain (int cpg);
    virtual void pitchDetector (AbstractPitchDetector* pdet);
    virtual void autoMode (bool a);
 	virtual double pitch () const;
 	virtual void table (SignalTable* tab);
    virtual int overlap (int voice) const;
//  	virtual void smoothingRate (double f);
//  	virtual void smoothingQuality (double f);
  	virtual void antiRumble (bool a);
   	virtual void sourceLevel (double l);
   	virtual void harmonyLevel (double l);
    virtual void bypass (bool b);
    virtual void freeze (bool f);
	virtual void masterKey (Pandora::notes m);
	virtual int lastTone () const;
	virtual int lastNote () const;
	virtual void reset ();
	virtual int voices () const;
	virtual SoundFrame& tick () {
		if (!m_m) {
			double d = m_osc->fastTick (); // MUST be not interpolated
			if (m_antiRumble) {
				d = m_rumble->process (d); 
			}
			m_frame->set (Pandora::LEFT, (d * (1 - m_rightPosition)) * m_sourceLevel);    
			m_frame->set (Pandora::RIGHT, (d * m_rightPosition) * m_sourceLevel);
			
			m_curr = m_pdet->process (d);
			if (m_curr > MIN_FREQ_LIMIT &&
				(fabs (m_curr - m_old) > fabs ((m_old * TUNINGSEMITONE) - m_old))) {
				m_old = m_curr;
	//				for (int i = 0; i < MAX_VOICES; i++) {
	//					m_pshifts[i]->freeze (true);
	//				}
						
				double ratio = m_curr / m_masterPitch;
				while (ratio >= 2) {
					ratio /= 2;
				}
				while (ratio < 1) {
					ratio *= 2;
				}
				int index = 0; 
				double maxDiff = 2;
				for (int i = 0; i < m_scaleSize; i++) {
					double diff = 0;
					diff = fabs (ratio - m_scale[i]);
			
					if (diff < maxDiff) {
						maxDiff = diff;
						index = i;
					}
				}
				if (index == m_scaleSize - 1) {
					if (fabs (ratio - (2 * m_scale[0])) < maxDiff) {
						index = 0;
					}
				}
	
				if (index != m_indexCopy) {
	//					for (int i = 0; i < MAX_VOICES; i++) {
	//						m_pshifts[i]->freeze (false);
	//					}
	//									    	
					m_indexCopy = index;
					updateRatios (index);
				}
			}
			for (int i = 0; i < MAX_VOICES; i++) {
				SoundFrame f;
				f = m_pshifts[i]->tick ();
				f.mul (Pandora::LEFT, m_harmonyLevel);
				f.mul (Pandora::RIGHT, m_harmonyLevel);
				
				if (m_bypass == false) {  
					if (m_activeVoices[i] == true) {
						*m_frame += f;    
					}
				}   	
			}   	
		} 
		
//		m_ucurr++;
//		m_ucurr %= m_usample;
		return *m_frame;
	}

private:
    virtual void setRatios ();
    //virtual void updateRatios (int index) {}
	virtual void updateRatios (int index) {
		if (m_logic == Pandora::SCALIC) {
			if (m_realFreq == true) {
				m_pshifts[0]->frequency (1 * m_fundamental);
				m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) 
					* m_fundamental);
			}
			else {
				m_pshifts[0]->ratio (1 * m_fundamental);
				m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
					* m_fundamental);			
			}
			switch (m_scaleType) {
				case Pandora::MAJOR1:
					switch (index) {
						case 0:	// fall thru, maj chord
						case 3:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	 
							else {
								m_pshifts[1]->ratio (
								m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
								* m_fundamental);
							m_pshifts[3]->ratio (
								m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
								* m_fundamental);								
							}    								 
						break;
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}		     						
						break;
						case 1:	// fall thru, min chord
						case 2:
						case 5:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;
						case 6:	// diminished chord
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}
						break;
					}
				break;
				case Pandora::MAJOR2:
					switch (index) {
						case 0:	// fall thru
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	  
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}   								 
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}		   
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}  						
						break;
						case 2:
						case 5:
						case 6:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;
						case 3:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL)
									* m_fundamental);	
							}
							else { 
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL)
									* m_fundamental);									
							}
						break;		     				
					}
				break;
				case Pandora::MINOR1:
					switch (index) {
						case 0:	// fall thru
						case 3:
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else { 
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
							}				 
						break;
						case 1:	// diminished chord
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else { 
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;
						case 2:
						case 5:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}		     								 
						break;	     				
						case 6:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);									
							}     								 
						break;	 		     					
					}
				break;
				case Pandora::MINOR2:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	     			
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}					 
						break;
						case 1:
						case 5:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);		     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);		     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}	     								 
						break;	     				
						case 3:
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}		 
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}    								 
						break;	 
						case 6:
							if (m_realFreq == true) {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;		     							     					
					}
				break;
				case Pandora::MINOR3:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}		     								 
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
							}
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);		     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	  
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);		     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}   								 
						break;	     				
						case 3:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}	
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}	     								 
						break;	 
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);
							}		
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}     						
						break;
						case 5:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	   
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
							}  						
						break;	     						
						case 6:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
							}
						break;		     							     					
					}
				break;
				case Pandora::BLUES1:
					switch (index) {
						case 0:	// fall thru
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	  
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);							
							}   								 
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	
							}
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);							
							}
						break;
						case 2:
						case 4:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	
							}	     
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
							}								 
						break;	     				
						case 3:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	 	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	
							}	 
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);	 	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
							}    								 
						break;	 
						case 5:
							if (m_realFreq == true) {
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->frequency (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL)
									* m_fundamental);
								m_pshifts[2]->ratio (m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);							
							}				
						break;		     							     					
					}
				break;							
			}
		}
		else if (m_logic == Pandora::INTELLICHORD) {
			m_pshifts[2]->ratio (1 * m_fundamental);	 			
			 switch (m_scaleType) {
				case Pandora::MAJOR1:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);								
							}		  			 				
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 			 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   			 						 						 			 			 		
					}
				break;
				case Pandora::MAJOR2:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);								
							}		  			 				
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 			 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   			 						 						 			 			 		
					}
				break;
				case Pandora::MINOR1:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);								
							}		  			 				
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 				 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;     
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   			 						 						 			 			 		
					}
				break;
				case Pandora::MINOR2:
					switch (index) {
						case 0:
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 			 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   			 						 						 			 			 		
					}
				break; 
				case Pandora::MINOR3:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL)
									* m_fundamental);								
							}		  			 				
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 			 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									.5
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									.5
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   	  			 						 						 			 			 		
					}
				break;
				case Pandora::BLUES1:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		 			 			
						break;
						case 1:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									.5
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									.5
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);								
							}		  			 				
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL)
									* m_fundamental);									
							}		  			 				
						break; 			 			
						case 3:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break; 	
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;  
						case 5:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   
						case 6:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);	     								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);										
							}		  			 				
						break;   	  			 						 						 			 			 		
					}
				break;   			 	  			 	 			 	 			 	
			 }
		}
		else if (m_logic == Pandora::SINGLECHORD) {			
			 switch (m_scaleType) {
				case Pandora::MAJOR1:
					switch (index) {
						case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[1]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[1]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break; 			 						 						 			 			 		
					}
				break;
				case Pandora::MAJOR2:
					switch (index) {
							case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break; 			 						 						 			 			 		
					}
				break;
				case Pandora::MINOR1:
					switch (index) {
							case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break; 				 						 						 			 			 		
					}
				break;
				case Pandora::MINOR2:
					switch (index) {
							case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break; 						
					}
				break; 
				case Pandora::MINOR3:
					switch (index) {
							case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_SEVENTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 2:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
					}
				break;
				case Pandora::BLUES1:
					switch (index) {
							case 0:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[1]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FIFTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[1]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MINOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;
						case 1: // the blues scales has 6 degrees
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MAJOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::TRITONE_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_THIRD_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break;		 			
						case 4:
							if (m_realFreq == true) {
								m_pshifts[0]->frequency (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->frequency (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->frequency (m_fundamental);    									 								
								m_pshifts[3]->frequency (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL)
									* m_fundamental);	
							}	     		
							else {
								m_pshifts[0]->ratio (
									m_tempered.get (Pandora::MINOR_SIXTH_INTERVAL) / 2
									* m_fundamental);								
								m_pshifts[1]->ratio (
									m_tempered.get (Pandora::PERFECT_FOURTH_INTERVAL) / 2
									* m_fundamental);    
								m_pshifts[2]->ratio (m_fundamental);    									 								
								m_pshifts[3]->ratio (
									m_tempered.get (Pandora::MAJOR_SECOND_INTERVAL)
									* m_fundamental);							
							}		 			 			
						break; 
					}
				break;   			 	  			 	 			 	 			 	
			}
		}
		
		m_pshifts[0]->amplitude (.25);    
		m_pshifts[1]->amplitude (.35);    
		m_pshifts[2]->amplitude (.45);    
		m_pshifts[3]->amplitude (.50);    	
	}

    virtual void setStyle ();
    virtual void setRandStyle ();
    virtual void setDisposition ();
	virtual void setChoral ();
	
    bool m_activeVoices[MAX_VOICES];
    Pandora::harmonizationLogic m_logic;
    Pandora::styles m_style;
    Pandora::randoms m_randoms;
    Pandora::dispositions m_disposition;
    Pandora::chorals m_choral;
    double m_fundamental;
    double m_ratios[MAX_VOICES];
    double m_amps[MAX_VOICES];
    double m_vibFreqs[MAX_VOICES];
    double m_vibAmps[MAX_VOICES];
    double m_randVibFreqs[MAX_VOICES];
    double m_randVibAmps[MAX_VOICES];
    double m_pannings[MAX_VOICES];
	double m_formants[MAX_VOICES];    
    PitchShifter** m_pshifts;
    AbstractPitchDetector* m_pdet;
    VariState* m_rumble;
    LoopOscillator* m_osc;
    bool m_realFreq;
    bool m_antiRumble;
    double m_sourceLevel;
    double m_harmonyLevel;
    bool m_bypass;
    TemperedIntervals m_tempered;
    PitchGrid m_pgrid;
    Pandora::notes m_masterKey;
    double m_masterPitch;
    std::vector<double> m_scale;
    int m_scaleSize;
    Pandora::scales m_scaleType;
    double m_curr;
    double m_old;
    bool m_freeze;
    int m_indexCopy;
    
//    int m_usample;
//    int m_ucurr;
};

#endif	// HARMONIZER_H 

// EOF

