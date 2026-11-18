// CorrelationEstimator.cpp
//

#include "../include/CorrelationEstimator.h"

CorrelationEstimator::CorrelationEstimator (SoundInfo* info, int size, double min,
	double max, int levels, double threshold, double smoothingRate) :
	AbstractPitchDetector (info), m_index (0),
	m_min (min), m_max (max), m_levels (levels),
	m_threshold (threshold) {
	if (size < 2) {
		throw std::runtime_error ("invalid window size");
	}
	else {
		m_size = size;
	}

	if (m_levels < CE_MIN_LEVELS) {
		m_levels = CE_MIN_LEVELS;
	}

	if (m_min > m_max) {
		m_min = m_max;
	}


	m_data = new double[m_size];
	memset (m_data, 0, sizeof (double) * m_size);

	m_back = new float[m_size];
	memset (m_back, 0, sizeof (float) * m_size);

	m_filters = new VariState*[m_levels];
	m_zero = new ZeroCrossings (info);
	for (int i = 0; i < m_levels; i++) {
		m_filters[i] = new VariState (info);
		m_filters[i]->type (Pandora::BANDPASS);
		m_filters[i]->cutoff (min); //(max - min) / 2);
	}

	m_final = new VariState (info);
	m_final->cutoff (smoothingRate);

	m_block = new DCBlocker (info);

	m_val = m_min; //(m_max - m_min) / 2;
}

CorrelationEstimator::~CorrelationEstimator () {
	delete [] m_data;
	delete [] m_back;
	delete m_final;
	//delete m_lp;
	delete [] m_filters;
	delete m_zero;
	delete m_block;
	//m_out->close ();
}

void CorrelationEstimator::threshold (double t) {
	m_threshold = t;
}

void CorrelationEstimator::reset () {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->reset ();
	}
	m_final->reset ();
}

void CorrelationEstimator::filterQuality (double q) {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->quality (q);
	}
}

void CorrelationEstimator::range (double min, double max) {
	if (min > max) {
		min = max;
	}
	m_min = min;
	m_max = max;

	reset ();
}

void CorrelationEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void CorrelationEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

double CorrelationEstimator::get () const { return m_val; }
double CorrelationEstimator::min () const { return m_min; }
double CorrelationEstimator::max () const { return m_max; }
void CorrelationEstimator::update (Subject* s) {}

// EOF


