// RandomGenerators.h
//

#ifndef RANDOMGENERATORS_H
#define RANDOMGENERATORS_H

#include "SoundUnit.h"
#include <time.h> // compatibility issues

//! 31 bit Park-Miller linear congruential PRNG.
class PRNG_31bit {
private:
	//! Copy constructor; it's private.
	PRNG_31bit (const PRNG_31bit&);
	//! Assignment operator; it's private.
	PRNG_31bit& operator= (PRNG_31bit&);
public:
	PRNG_31bit ();
    //! Sets the seed of the generator.
	void seed (long s);
    //! Generates the random number.
	long generate () {
		unsigned long lo = m_a * (long) (m_seed & 0xFFFF);
		unsigned long hi = m_a * (long) ((unsigned long) m_seed >> 16);
		lo += (hi & 0x7FFF) << 16;
		if (lo > m_m) {
			lo &= m_m;
			++lo;
		}
		lo += hi >> 15;
		if (lo >m_m) {
			lo &= m_m;
			++lo;
		}
		m_seed = (long) lo;
		return m_seed;
	}
	long fastGenerate () {
		return rand ();
	}	
private:
	int m_a;
	unsigned long m_m;
	long m_seed;
};

//! UnitRandomNumberGenerator returns random numbers from 0. to 1.
class UnitRandomNumberGenerator : public SoundUnit {
private:
	//! Copy constructor; it's private.
	UnitRandomNumberGenerator (const UnitRandomNumberGenerator&);
	//! Assignment operator; it's private.
	UnitRandomNumberGenerator& operator= (UnitRandomNumberGenerator&);
public:
    UnitRandomNumberGenerator (SoundInfo* info);
    //! Sets the seed of the generator.
	void seed (long s);
	virtual double& tick () { 
		m_val = m_unitScale * (double) m_rng.generate ();
		return m_val; 
	}
	virtual double& fastTick () { 
		m_val = m_unitScale * (double) m_rng.fastGenerate ();
		return m_val; 
	}
	void update (Subject* s);
private:
	double m_unitScale;
	
	PRNG_31bit m_rng;
};

//! RangedRandomNumberGenerator returns random numbers in [min, max].
class RangedRandomNumberGenerator : public SoundUnit {
private:
	//! Copy constructor; it's private.
	RangedRandomNumberGenerator (const RangedRandomNumberGenerator&);
	//! Assignment operator; it's private.
	RangedRandomNumberGenerator& operator= (RangedRandomNumberGenerator&);
public:
    //! Class constructor with [min, max] parameters.
	RangedRandomNumberGenerator (SoundInfo* info, double min = 0, 
        double max = 1);
    //! Sets the range of the generator.
	void range (double min, double max);
    //! Sets the seed of the generator.
	void seed (long s);
	//! Generates a random number.
	virtual double& tick () { 
		m_val = m_min + m_scale * (double) m_rng.generate (); 
		return m_val;
	}
	virtual double& fastTick () { 
		m_val = m_min + m_scale * (double) m_rng.fastGenerate (); 
		return m_val;
	}	
	void update (Subject* s);

private:
	double m_min;
	double m_scale;
	double m_unitScale;

	PRNG_31bit m_rng;
};

#endif	// RANDOMGENERATORS_H

// EOF

