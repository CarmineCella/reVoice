// LoopOscillator.h
//

#ifndef LOOPOSCILLATOR_H
#define LOOPOSCILLATOR_H

#include "AbstractOscillator.h"
#include "SoundInfo.h"
#include "SignalTable.h"

//! Interpolated oscillator class. 
/*! 
	With this class you can perform table-driven synthesis of any kind.
*/
class LoopOscillator : public AbstractOscillator {
private:
	//! Assignment operator; it is private.
	LoopOscillator& operator= (LoopOscillator&);
	//! Copy constructor; it is private.
	LoopOscillator (const LoopOscillator&);
public:
	//! Class constructor.
	/*!
		You must provide the pointer to an object of type SignalTable and the
		pointer to an object of type SoundInfo. In this way this class
		implements the Subject - Observer pattern. It is the Observer.
		By default, it plays the table at 440 Hz, with maximum amplitude (1).
	*/
	LoopOscillator (SoundInfo* info, SignalTable *tab, double amp = 1, 
		double freq = 440, double phi = 0);
	virtual ~LoopOscillator ();
	//! Sets the amplitude for the oscillator. Must be between 0 and 1.
	void amplitude (double amp) {
		m_amp = amp;
	}
	//! Sets the frequency for the oscillator.
	void frequency (double freq) {
		m_freq = freq;
		m_incr = m_sicvt * m_freq;		
	}
	//! Gets the frequency for the oscillator.
	double frequency () const;	
	//! Sets the phase for the oscillator.
	virtual void phase (double phi) {
		if (phi >= 0 && phi <= 1)
			m_phi = phi * m_tab->size ();
		else {
			m_phi = 0;
		}			
	}
	//! Gets the phase for the oscillator.
	double phase () const;
	//! Sets a new table for the oscillator.
	void table (SignalTable *tab);
	//! Sets the loop flag status.
	void loop (const bool l);
    //! Gets the loop flag status.
    bool loop () const;
	//! This is the generation function, shared by all the objects of type SoundObject.
	/*!
		The output is a sample between -1 and 1 stored in a double. You can cast or 
		convert it to the type you prefer.
	*/
	virtual double& tick () {
		if (!m_m) {
			m_phi += m_incr;
	
			if (m_phi >= m_l) {
				if (m_loop) {
					m_phi -= m_l;
				}
				else {
					m_m = true;
					m_val = 0;
					return m_val;
				}
			} 			
			else if (m_phi < 0.f) {
				if (m_loop)
					m_phi += m_l;
				else {
					m_m = true;
					m_val = 0;
					return m_val;
				} 			
			}
	
			int index = (int) m_phi;
			int next;
			if (index == m_l)
				next = 0;
			else
				next = index + 1;
			double frac = m_phi - index;
			double lo = (*m_tab)[index];
			double hi = (*m_tab)[next];
			m_val = (lo + frac * (hi - lo)) * m_amp;
		}
		else
			m_val = 0;
	
		return m_val;
	}
	//! This is the fast generation function.
	/*!
		The output is a sample between -1 and 1 stored in a double. This functions
		does not interpolate between samples and can't accept negative phase.
	*/	
	virtual double& fastTick () {
		if (!m_m) {
			m_phi += m_incr;
	
			if (m_phi >= m_l) {
				if (m_loop) {
					m_phi -= m_l;
				}
				else {
					m_m = true;
					m_val = 0;
					return m_val;
				}
			} 			    
			int index = (int) m_phi;
			m_val = (*m_tab)[index] * m_amp;
		}
		else
			m_val = 0;
	
		return m_val;
	}	
	//! This is the implementation of the update function of the Observer.
	/*!
		It recalculates the table increment factor by dividing the SignalTable length with
		the main sampling rate.
	*/
	virtual void update (Subject* s);
private:
	SignalTable* m_tab;
	long m_l;
	double m_amp, m_freq, m_phi, m_sicvt, m_incr;
	bool m_loop;
};

#endif	// OSCILLATOR_H

// EOF

