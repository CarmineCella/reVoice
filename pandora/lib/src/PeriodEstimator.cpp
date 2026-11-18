// PeriodEstimator.cpp
// 

#include "../include/PeriodEstimator.h"

PeriodEstimator::PeriodEstimator (SoundInfo* info, double min, 
	double max, int levels, int split,    
	double threshold, double smoothingRate) : AbstractPitchDetector (info), 
	m_min (min), m_max (max), m_levels (levels), m_split (split), 
	m_threshold (threshold), m_num (0) {
	if (m_min > m_max) {
		m_min = m_max;
	}
	if (m_levels < PE_MIN_LEVELS) {
		m_levels = PE_MIN_LEVELS;
	}
	if (m_split < PE_MIN_SPLIT) {
		m_split = PE_MIN_SPLIT;
	}
	
	double current = min;
	while (current < max) {
		m_bins.push_back (current *= 1.4142); // half oct.
		m_num++;
	}
	   
	m_filters = new VariState*[m_num * m_levels];
	m_zeroes = new ZeroCrossings*[m_num];
	m_envFollowers = new EnvelopeFollower*[m_num];
	for (int i = 0; i < m_num; i++) {
		for (int k = 0; k < m_levels; k++) {
			m_filters[(i * m_levels) + k] = new VariState (info);
			m_filters[(i * m_levels) + k]->type (Pandora::LOWPASS);
			m_filters[(i * m_levels) + k]->cutoff (m_bins[i]);
		}    
		m_zeroes[i] = new ZeroCrossings (info);
		m_envFollowers[i] = new EnvelopeFollower (info);
	}

	m_block = new DCBlocker (info);
	
	m_final = new VariState (info);
	m_final->cutoff (smoothingRate);
	
	m_filtered = new double[m_num];
	memset (m_filtered, 0, sizeof (double) * m_num);
	m_periods = new double[m_num];
	memset (m_periods, 0, sizeof (double) * m_num);
	m_followed = new double[m_num];
	memset (m_followed, 0, sizeof (double) * m_num);
	
	m_sr = m_info->samplingRate ();
	m_counter = 0;
	m_period = 0;
}

PeriodEstimator::~PeriodEstimator () {
	delete [] m_filters;
	delete [] m_zeroes;
	delete m_final;
	delete [] m_envFollowers;
	delete m_block;
	delete [] m_filtered;
	delete [] m_periods;
	delete [] m_followed;
}

void PeriodEstimator::threshold (double t) { 
	m_threshold = t; 
}

void PeriodEstimator::reset () {
	m_block->reset ();
	for (int i = 0; i < m_num; i++) {
		for (int k = 0; k < m_levels; k++) {
			m_filters[(i * m_levels) + k]->reset ();
		}  
		m_zeroes[i]->reset ();
	   m_envFollowers[i]->reset (); 
	}        	    
	m_final->reset ();
	memset (m_filtered, 0, sizeof (double) * m_num);
	memset (m_periods, 0, sizeof (double) * m_num);
	memset (m_followed, 0, sizeof (double) * m_num);
}

void PeriodEstimator::split (double s) { 
	if (s < PE_MIN_SPLIT) {
		s = PE_MIN_SPLIT; 
	}
	m_split = s; 
}

void PeriodEstimator::filterQuality (double q) {
	for (int i = 0; i < m_num; i++) {
		for (int k = 0; k < m_levels; k++) {
			m_filters[(i * m_levels) + k]->quality (q);
		}
	} 
}

void PeriodEstimator::scaleBinsTo (double m) {
	for (int i = 0; i < m_num; i++) {
		m_bins[i] *= m;
		for (int k = 0; k < m_levels; k++) {
			m_filters[(i * m_levels) + k]->cutoff (m_bins[i]);
		}   
	}   
}

void PeriodEstimator::feedback (double f) {
	for (int i = 0; i < m_num; i++) {
		m_zeroes[i]->feedback (f);
	}  	
}

void PeriodEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void PeriodEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

double PeriodEstimator::min () const { return m_min; }

double PeriodEstimator::max () const { return m_max; }

void PeriodEstimator::update (Subject* s) {
	m_sr = m_info->samplingRate (); // performance
}

// EOF


