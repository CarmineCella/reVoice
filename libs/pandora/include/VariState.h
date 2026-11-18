// VariState.h
// 

#ifndef VARISTATE_H
#define VARISTATE_H 

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
#ifndef TWOPI
#define TWOPI (6.28318530717958647692)
#endif

#include "filterType.h"
#include "SoundInfo.h"
#include "AbstractFilter.h"
#include <cmath>

//! Chamberlin variable state filter (IIR, 12dB/oct)
class VariState : public AbstractFilter {
private:
	VariState& operator= (VariState&);
	VariState (const VariState&);
public: 
	VariState (SoundInfo* info, double cutoff = 440, 
        double quality = .707, Pandora::filterType t = Pandora::LOWPASS);
	virtual ~VariState ();
	void cutoff (double c) {
		if (c > 0) {
			m_cutoff = c;
			m_f1 = 2 * sin (M_PI * m_cutoff / m_info->samplingRate ());
		}
	}
	//! Fast cutoff calculation; does not check c and uses approximation.
	void fastCutoff (double c) {
		m_cutoff = c;
		m_f1 = 2 * (m_cutoff * m_sicvt);
	}
	virtual void quality (double q);
    void reset ();
	void type (Pandora::filterType t);
	//! Process function; computes all responses.
	virtual double& process (double& sig) {
		m_data[Pandora::LOWPASS] = m_data[Pandora::LOWPASS] + 
			m_f1 * m_data[Pandora::BANDPASS];
		m_data[Pandora::HIGHPASS] = sig - 
			m_data[Pandora::LOWPASS] - m_q1 * m_data[Pandora::BANDPASS];
		m_data[Pandora::BANDPASS] = m_f1 * m_data[Pandora::HIGHPASS] + 
			m_data[Pandora::BANDPASS];
		m_data[Pandora::NOTCH] = m_data[Pandora::HIGHPASS] + m_data[Pandora::LOWPASS];
		
		return m_data[m_type];
	}
	//! Fast process function; does not compute the notch response.
	virtual double& fastProcess (double& sig) {
		m_data[Pandora::LOWPASS] = m_data[Pandora::LOWPASS] + 
			m_f1 * m_data[Pandora::BANDPASS];
		m_data[Pandora::HIGHPASS] = sig - 
			m_data[Pandora::LOWPASS] - m_q1 * m_data[Pandora::BANDPASS];
		m_data[Pandora::BANDPASS] = m_f1 * m_data[Pandora::HIGHPASS] + 
			m_data[Pandora::BANDPASS];
		
		return m_data[m_type];
	}	
	virtual void update (Subject* s);

private:
	double m_cutoff;
	Pandora::filterType m_type;
	double m_data[4];
	double m_q1;
	double m_f1;
	double m_sicvt;
};

#endif	// VARIFILTER_H 

// EOF


