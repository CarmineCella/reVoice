// Renoise.cpp
// created: Mon Mar 26 16:17:06 2007
// 

#include "../include/Renoise.h"

Renoise::Renoise (SoundInfo* info, double band, double gain) :
	SoundEffect (info) {
	m_rng.seed ((long) time (NULL));
	
	if (gain > 0) {
		m_gain = gain;
	}
	else {
		m_gain = 1;
	}
	m_var = new VariState (info, band, .707, Pandora::LOWPASS);
	m_rand = new RangedRandomNumberGenerator (info, -1, 1);
}

Renoise::~Renoise () {
	delete m_var;
	delete m_rand;
}

void Renoise::band (double b) {
	m_var->cutoff (b);
}

void Renoise::gain (double g) {
	if (g >= 0) {
		m_gain = g;
	}
}

// EOF

