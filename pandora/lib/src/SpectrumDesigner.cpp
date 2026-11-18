// SpectrumDesigner.cpp
// 

#include "../include/SpectrumDesigner.h"

SpectrumDesigner::SpectrumDesigner (SoundInfo* info, SignalTable* table, double start, 
	double duration, double min, double max, int partials) 
	: AbstractSpectrum (info, start, duration) {			
	assert (table);
	m_table = table;
	
	if (min > max) min = max;
	
	m_min = min;
	m_max = max;
	
	if (partials < 2) partials = 2;
	
	m_partials = partials;
	
	m_bank = new LoopOscillator*[m_partials];
	for (long x = 0; x < m_partials; x++) {
		m_bank[x] = new LoopOscillator (info, m_table);
	}		
	
	m_noise = new Renoise (info, 5, 5);
	m_rand = new UnitRandomNumberGenerator (info);
	m_birand = new RangedRandomNumberGenerator (info, -0.5, 0.5);
	m_pans = new double[m_partials];

	design (Pandora::RANDOM_SPECTRUM);
	jitter (0.5);
}

SpectrumDesigner::~SpectrumDesigner () { 
	delete [] m_bank;
	delete m_noise;
	delete m_rand;
	delete m_birand;
	delete [] m_pans;
}

void SpectrumDesigner::design (Pandora::spectralType type) {
	std::vector <double> v;
	design (type, v);
}

void SpectrumDesigner::design (Pandora::spectralType type, 
	std::vector <double> params) {
	switch (type) {
		case Pandora::RANDOM_SPECTRUM:
			for (long x = 0; x < m_partials; x++) {
				double freq = (m_rand->tick () * (m_max - m_min)) + m_min;
				double amp = (m_rand->tick ()  * (1.0 / (double) m_partials));		
				double phi = m_rand->tick ();	
				m_bank[x]->amplitude (amp);
				m_bank[x]->frequency (freq);
				m_bank[x]->phase (phi);			
			}				
		break;
		case Pandora::ALPHA_SPECTRUM: {
			if (params.size () < 1) return;
			
			double alpha = params[0];
			double c1 = (m_max - m_min);
			if (alpha == 0.0) {
				c1 /= m_partials;
			}
			else {
				c1 /= (1.0 - (double) exp ((double) alpha));
			}
	
			alpha /= m_partials;
			double curr = alpha;
			double freq = m_min;
			double amp = (1.0 / (double) m_partials);
			m_bank[0]->frequency (m_min);
			m_bank[0]->amplitude (amp);
			m_bank[0]->phase (0);
			for (long x = 1; x < m_partials; x++) {
				if (alpha == 0.0) {
					freq += c1;
					}
				else {
					freq = m_min + c1 * (1.0 - (double) exp ((double) (curr)));
				}
	
				m_bank[x]->amplitude (amp);
				m_bank[x]->frequency (freq);
				m_bank[x]->phase (0);
				
				curr += alpha;
			}				
		}
		break;
		case Pandora::LISSAJOUS_SPECTRUM: {
			if (params.size () < 2) return;
			
			double alpha = params[0];
			double beta = params[1];
			
			for (long x = 0; x < m_partials; x++) {
				double freq = fabs (m_max * sin (alpha * x) * cos (beta * x)) 
					+ m_min;
				double amp = fabs ((1.0 / (double) m_partials) * 
					sin (alpha * x) * sin (beta * x));
				m_bank[x]->amplitude (amp);
				m_bank[x]->frequency (freq);
				m_bank[x]->phase (0);		
			}			
		}
		break;
		case Pandora::POLY_SPECTRUM: {
			double plotDelta = 200;
			double plotIncr = plotDelta / m_partials;
		 	double plotFreq = -100;
	
		 	std::vector <double> amps;
		 	std::vector <double> freqs;
			double plotMin = 0, plotMax = 0; 
	
			std::reverse (params.begin (), params.end ());
			for (long x = 0; x < m_partials; x++) {
				int n = params.size () - 1, j;
				double p = params[j = n];
			
				while (j > 0) {
					p = (p * plotFreq + params[--j]);
				}
		
				amps.push_back (p);
				if (p <= plotMin) plotMin = p;
				if (p >= plotMax) plotMax = p;
	
				freqs.push_back (plotFreq);
				plotFreq += plotIncr;
			}
			
			double ampStep = (1.0 / (double) m_partials);
		 	double freqDelta = m_max - m_min;
			for (long x = 0; x < m_partials; x++) {
				m_bank[x]->amplitude (((amps[x] + fabs (plotMin)) / (plotMax - plotMin)) * ampStep);
				m_bank[x]->frequency (((freqs[x] + 100) / 200) * freqDelta + m_min);
				m_bank[x]->phase (0);
			}
		}	
		break;
		case Pandora::FMLIKE_SPECTRUM: {
			double modulator = (m_max - m_min) / (double) m_partials;
			// index == partials !!
			double amps[m_partials];
			for (long x = 0; x < m_partials; x++) {
				long bessOrd = abs (((m_partials / 2) - x));
				amps[x] = fabs (jn (bessOrd, m_partials));
			}		
			double norm = amps[0];
			for (long x = 0; x < m_partials; x++) {
				if (amps[x] >= norm) { norm = amps[x]; }
			}		
			
			double ampFrac = (1.0 / (double) m_partials);	
			for (long x = 0; x < m_partials; x++) {
				double freq = m_min + (x * modulator);
				double amp = (amps[x] / norm) * ampFrac;
				m_bank[x]->amplitude (fabs (amp));
				m_bank[x]->frequency (freq);
				m_bank[x]->phase (0);
			}				
		}
		break;
	}
}

void SpectrumDesigner::purify (double ratio) {
	for (long x = 0; x < m_partials; x++) {
		for (long j = x + 1; j < m_partials; j++) {
			if (m_bank[j]->frequency () / m_bank[x]->frequency () <= ratio) {
				m_bank[x]->amplitude (0);
			}
		}
	}		
}

void SpectrumDesigner::noise (double band, double gain) {
	m_noise->band (band);
	m_noise->gain (gain);
}

void SpectrumDesigner::jitter (double j) {
	if (j >= 0 && j <= 1) {
		for (int i = 0; i < m_partials; i++) {
			double t = m_birand->tick ();
			t *= j;
			m_pans[i] = 0.5 + t;
		}
	}
}

void SpectrumDesigner::scramblePhases (bool clear) {
	for (int i = 0; i < m_partials; i++) {
		if (clear) {
			m_bank[i]->phase (0);
		}
		else {
			m_bank[i]->phase (m_rand->tick ());
		}
	}
}

double SpectrumDesigner::min () const { return m_min; }

double SpectrumDesigner::max () const { return m_max; }

long SpectrumDesigner::partials () const { return m_partials; }	

LoopOscillator* SpectrumDesigner::oscillator (int num) const { 
	if (num >= m_partials) num = 0;
	return m_bank[num];
}

// EOF


