// SpectralEstimator.cpp
// 

#include "../include/SpectralEstimator.h"

SpectralEstimator::SpectralEstimator (SoundInfo* info, int size, double min, 
	double max, int levels, double split, double bias, 
	double threshold, double smoothingRate) :
	AbstractPitchDetector (info), m_index (0), 
	m_min (min), m_max (max), m_levels (levels), m_bin (0), 
	m_bias (bias), m_threshold (threshold) {
	if ((((~size + 1) & size) != size) || size < 2) {
		throw std::runtime_error ("invalid window size for FFT (not a power of 2)");
	}
	else {	
		m_size = size;
		m_size2 = m_size >> 1;
	}
		
	if (m_levels < SE_MIN_LEVELS) {
		m_levels = SE_MIN_LEVELS;
	}
		
	m_bin = (m_info->samplingRate () / 2) / m_size;
	
	if (m_min > m_max) {
		m_min = m_max;
	}
	
	if (split > 0)
		m_split = split;
	else
		m_split = 1.5;
		
	m_minPos = (int) (m_min / m_bin);
	m_minPos /= 2; // making even
	m_minPos *= 2;
	
	m_maxPos = (int) (m_max / m_bin);
	m_maxPos /= 2; // making even
	m_maxPos *= 2;
	
	m_totBins = (m_maxPos - m_minPos) / 2;
	
	m_data = new double[m_size];
	memset (m_data, 0, sizeof (double) * m_size);
	
//	m_lp = new VariState (info);		// FIXME
//	m_lp->cutoff ((m_max - m_min) / 2);
//	m_lp->type (Pandora::LOWPASS);

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
	
	m_val = m_min; //(m_max - m_min) / 2;
	m_peak = 0;
}

SpectralEstimator::~SpectralEstimator () {
	delete [] m_data;
	delete m_final;
	//delete m_lp;
	delete [] m_filters;
	delete m_zero;
	delete m_block;
	//m_out->close ();
}

void SpectralEstimator::threshold (double t) { 
	m_threshold = t; 
}

void SpectralEstimator::reset () {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->reset ();
	}    	
	m_final->reset ();
}

void SpectralEstimator::filterQuality (double q) {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->quality (q);
	}     
}

void SpectralEstimator::range (double min, double max) {
	if (min > max) {
		min = max;
	}
	m_min = min;
	m_max = max; 
   
	m_minPos = (int) (m_min / m_bin);
	m_minPos /= 2; // making even
	m_minPos *= 2;
	
	m_maxPos = (int) (m_max / m_bin);
	m_maxPos /= 2; // making even
	m_maxPos *= 2;
	   
	reset ();
}

void SpectralEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void SpectralEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

void SpectralEstimator::split (double s) {
	if (s > 0) {
		m_split = s;
	}
}

double SpectralEstimator::get () const { return m_val; }
double SpectralEstimator::min () const { return m_min; }
double SpectralEstimator::max () const { return m_max; }
void SpectralEstimator::update (Subject* s) {}

// EOF


