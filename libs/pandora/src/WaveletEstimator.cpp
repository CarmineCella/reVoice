// WaveletEstimator.cpp
// 

#include "../include/WaveletEstimator.h"

WaveletEstimator::WaveletEstimator (SoundInfo* info, int size, double min, 
	double max, int levels, int coefficients,  
	double bias, double threshold, double smoothingRate) :
	AbstractPitchDetector (info), m_index (0), 
	m_min (min), m_max (max), 
	m_levels (levels), m_coefficients (coefficients), 
	m_bias (bias), m_threshold (threshold) {
	if ((((~size + 1) & size) != size) || size < 2) {
		throw std::runtime_error ("invalid window size for DWT (not a power of 2)");
	}
	else	
		m_size = size;
		
	if (m_levels < WE_MIN_LEVELS) {
		m_levels = WE_MIN_LEVELS;
	}
		
	if (m_min > m_max) {
		m_min = m_max;
	}
	
	m_data.resize (m_size);

	m_exponent = log2 (m_size);
	m_bands = new double[m_exponent];
	memset (m_bands, 0, sizeof (double) * m_exponent);
	
	m_bins = new double[m_exponent];
	m_bins[m_exponent - 1] = info->samplingRate () / 2;
    for (int i = m_exponent - 2; i > -1; --i) {
    	m_bins[i] = m_bins[i + 1] / 2;
    	//std::cout << i << " " << m_bins[i] << std::endl;
	}
    
    // set search limits
	m_minBand = 0;
	while (m_bins[m_minBand] < m_min) ++m_minBand;
	//if (m_minBand > 0) m_minBand -= 1; // lower bound included
	
	m_maxBand = 0;
	while (m_bins[m_maxBand] < m_max) ++m_maxBand;
	if (m_maxBand >= m_exponent) m_maxBand -= 1; // safety check
	
	m_totBand = (m_maxBand - m_minBand);
	if (m_totBand <= 0) m_totBand = 1;
	//std::cout << m_minBand << " " << m_maxBand << " " << m_totBand << std::endl;
	
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
	
	wavelet::pwtset (m_coefficients);

	m_val = m_min; //(m_max - m_min) / 2;
	m_peak = 0;
}

WaveletEstimator::~WaveletEstimator () {
	//delete [] m_data;
	delete m_final;
	//delete m_lp;
	delete [] m_filters;
	delete m_zero;
	delete m_block;
	delete [] m_bands;
	delete [] m_bins;
	//m_out->close ();
	//delete m_out;
}

void WaveletEstimator::threshold (double t) { 
	m_threshold = t; 
}

void WaveletEstimator::reset () {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->reset ();
	}    	
	m_final->reset ();
}

void WaveletEstimator::filterQuality (double q) {
	for (int i = 0; i < m_levels; i++) {
		m_filters[i]->quality (q);
	}     
}

void WaveletEstimator::range (double min, double max) {
	if (min > max) {
		min = max;
	}
	m_min = min;
	m_max = max; 
	   
	reset ();
}

void WaveletEstimator::smoothingRate (double f) {
	m_final->cutoff (f);
}

void WaveletEstimator::smoothingQuality (double f) {
	m_final->quality (f);
}

double WaveletEstimator::get () const { return m_val; }
double WaveletEstimator::min () const { return m_min; }
double WaveletEstimator::max () const { return m_max; }
void WaveletEstimator::update (Subject* s) {}

// EOF


