// PulseGenerator.h
// 

#ifndef PULSEGENERATOR_H
#define PULSEGENERATOR_H 

#include "SoundInfo.h"
#include "SoundUnit.h"

//! A class to generate a pulse train.
/*!
	With this class you can generate a pulse train by specifying how many
	pulses per second you need. This class can be used as a control instrument.
*/
class PulseGenerator : public SoundUnit {
private:
	//! Assignment operator; it is private.
	PulseGenerator& operator= (PulseGenerator&);
	//! Copy constructor; it is private.
	PulseGenerator (const PulseGenerator&);
public: 
	//! Class constructor; 'freq' means pulses per second.
	PulseGenerator (SoundInfo* info, double freq = 1, double phi = 0);
	//! Class destructor.
	virtual ~PulseGenerator ();
	//! Sets the number of pulses per second.
	void frequency (double freq) {
		m_freq = freq;
		m_incr = m_freq * m_delta;
	}
	//! Sets the phase value.
	void phase (double phi);
	//! This method returns 1 each time the pulse is on; 0 otherwise.
	virtual double& tick () {
		if (m_phi == 0.0 && m_flag) {
			m_val = 1;
			m_flag = false;
		}
		
		else if ((m_phi += m_incr) >= 1.0) {
			m_val = 1;
			m_phi -= 1.0;
			m_flag = false;
		}
		else 
			m_val = 0;
		
		return m_val;
	}
	//! This is the implementation of the update function of the Observer.
	/*!
		It recalculates the increment factor for the phase.
	*/
	virtual void update (Subject* s);
private:
	double m_delta, m_freq, m_incr, m_phi;
	bool m_flag;
};

#endif	// PULSEGENERATOR_H 

// EOF

