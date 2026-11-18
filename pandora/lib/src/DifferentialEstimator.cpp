// DifferentialEstimator.cpp
//

#include "../include/DifferentialEstimator.h"

DifferentialEstimator::DifferentialEstimator (SoundInfo* info, int size, double min,
	double max, int levels, double threshold, double ampThreshold, double smoothingRate) :
	AbstractPitchDetector (info), m_index (0),
	m_min (min), m_max (max), m_threshold (threshold), m_ampThreshold (ampThreshold) {
	if (size < 2) {
		throw std::runtime_error ("invalid window size");
	}
	else {
		m_size = size;
	}

	if (m_min > m_max) {
		m_min = m_max;
	}

	if (m_levels < DE_MIN_LEVELS) {
		m_levels = DE_MIN_LEVELS;
	}

	m_data = new double[m_size];
	memset (m_data, 0, sizeof (double) * m_size);

	m_filters = new VariState*[m_levels];
	m_zero = new ZeroCrossings (info);
	for (int i = 0; i < m_levels; i++) {
		m_filters[i] = new VariState (info);
		m_filters[i]->type (Pandora::LOWPASS);
		m_filters[i]->cutoff (min); //(max - min) / 2);
	}

	m_final = new VariState (info);
	m_final->cutoff (smoothingRate);

	m_block = new DCBlocker (info);
	m_follow = new EnvelopeFollower (info);

	m_val = m_min; //(m_max - m_min) / 2;
}

DifferentialEstimator::~DifferentialEstimator () {
	delete m_final;
	delete [] m_data;
	delete m_follow;
	delete [] m_filters;
	delete m_zero;
	delete m_block;
}

void DifferentialEstimator::threshold (double t) {
	m_threshold = t;
}

void DifferentialEstimator::ampThreshold (double t) {
	m_ampThreshold = t;
}

void DifferentialEstimator::range (double min, double max) {
	if (min > max) {
		min = max;
	}
	m_min = min;
	m_max = max;

	reset ();
}

void DifferentialEstimator::reset () {
	m_final->reset ();
}

void DifferentialEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void DifferentialEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

double DifferentialEstimator::get () const { return m_val; }
double DifferentialEstimator::min () const { return m_min; }
double DifferentialEstimator::max () const { return m_max; }
void DifferentialEstimator::update (Subject* s) {}

// EOF


