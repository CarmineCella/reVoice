// AdaptiveEstimator.cpp
// 

#include "../include/AdaptiveEstimator.h"

AdaptiveEstimator::AdaptiveEstimator (SoundInfo* info, double min, 
	double max, int levels, double bias, double threshold,
	double smoothingRate) //, double speed) 
	: AbstractPitchDetector (info), m_min (min), m_max (max), m_levels (levels), 
	m_bias (bias), m_threshold (threshold) /*, m_speed (speed), m_oldIn (0), m_oldOut (0) */ {
	if (m_min > m_max) {
		m_min = m_max;
	}
	
	//m_middle = (m_max - m_min) / 2;
	//m_center = m_middle;
	
	if (m_levels < AT_MIN_LEVELS) {
		m_levels = AT_MIN_LEVELS; // 24 dB / oct
	}
		
//	m_bp = new VariState (info);
//	m_bp->type (Pandora::BANDPASS);
//	m_bp->cutoff (m_center);
//	
	m_lps = new VariState*[m_levels];
	for (int i = 0; i < m_levels; i++) {
		m_lps[i] = new VariState (info);
		m_lps[i]->type (Pandora::LOWPASS);
		m_lps[i]->cutoff (min); //(m_max - m_min) / 2); 
	}        
	
	m_final = new VariState (info);
	m_final->cutoff (smoothingRate);
	
	m_follow = new EnvelopeFollower (info);
	m_zero = new ZeroCrossings (info);
	m_block = new DCBlocker (info);
	
	m_val = m_min; //(m_max - m_min) / 2;
	//m_old = m_val;
}

AdaptiveEstimator::~AdaptiveEstimator () {
	//delete m_bp;
	delete [] m_lps;
	delete m_final;
	delete m_follow;
	delete m_zero;
	delete m_block;
}

void AdaptiveEstimator::threshold (double t) { m_threshold = t; }

void AdaptiveEstimator::reset () {
	for (int k = 0; k < m_levels; k++) {                                   
		m_lps[k]->reset ();
	} 
	//m_bp->reset ();   
	m_final->reset ();	   
	m_follow->reset (); 
	m_zero->reset ();
}

//void AdaptiveEstimator::speed (double s) { m_speed = s; }

void AdaptiveEstimator::filterQuality (double q) {
	for (int i = 0; i < m_levels; i++) {
		m_lps[i]->quality (q);
	}     
}

void AdaptiveEstimator::range (double min, double max) {
	if (min > max) {
		min = max;
	}
	m_min = min;
	m_max = max; 
   
	//m_middle = (m_max - m_min) / 2;
}

void AdaptiveEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void AdaptiveEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

double AdaptiveEstimator::get () const { return m_val; }

double AdaptiveEstimator::min () const { return m_min; }

double AdaptiveEstimator::max () const { return m_max; }

void AdaptiveEstimator::update (Subject* s) {}

// EOF
