// RandomGenerators.cpp
//

#include "../include/RandomGenerators.h"

PRNG_31bit::PRNG_31bit () : m_a (16807), m_m (2147483647L) {	    
	seed (time (NULL));
}

void PRNG_31bit::seed (long s) {
	m_seed = s & m_m;
	if (m_seed == 0) m_seed = 1;
}

UnitRandomNumberGenerator::UnitRandomNumberGenerator (SoundInfo* info) :
	SoundUnit (info) {
	m_unitScale = 1. / (double) 0x7FFFFFFF;		
}

void UnitRandomNumberGenerator::seed (long s) {
	m_rng.seed (s);
}

void UnitRandomNumberGenerator::update (Subject* s) {}

RangedRandomNumberGenerator::RangedRandomNumberGenerator (SoundInfo* info, 
	double min, double max) : SoundUnit (info) { 
	m_unitScale = 1. / (double) 0x7FFFFFFF;
	range (min, max); 
}

void RangedRandomNumberGenerator::range (double min, double max) {
	m_min = min;
	m_scale = (max - min) * m_unitScale;
}

void RangedRandomNumberGenerator::seed (long s) {
	m_rng.seed (s);
}

void RangedRandomNumberGenerator::update (Subject* s) {}

// EOF

