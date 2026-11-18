// BiQuad.h
//

#ifndef BIQUAD_H
#define BIQUAD_H

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

//! 2nd order LP/HP filter (IIR, 12dB/oct)
class BiQuad : public AbstractFilter {
private:
	BiQuad& operator= (BiQuad&);
	BiQuad (const BiQuad&);
public:
	BiQuad (SoundInfo* info, double cutoff = 440, 
        double quality = .707, Pandora::filterType type = Pandora::LOWPASS);
	virtual ~BiQuad ();
	void cutoff (double c);
	void quality (double q);
	void type (Pandora::filterType t);
	void reset ();
	virtual double& process (double& sig);
	virtual void update (Subject* s);

private:
	double m_a0, m_a1, m_a2;
	double m_b1, m_b2;
	double m_in[3];
	double m_out[3];
	double m_cutoff, m_quality;
	Pandora::filterType m_type;

	void calcCoeff ();
};

#endif	// IIR2ND_H

// EOF


