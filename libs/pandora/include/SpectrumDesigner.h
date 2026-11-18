// SpectrumDesigner.h
//

#ifndef SPECTRUMDESIGNER_H
#define SPECTRUMDESIGNER_H

#include "SignalTable.h"
#include "AbstractSpectrum.h"
#include "LoopOscillator.h"
#include "SoundInfo.h"
#include "RandomGenerators.h"
#include "Renoise.h"
#include "spectralType.h"
#include <cassert>
#include <vector>
#include <algorithm>

#include <iostream>
#include <cmath>

//! Additive synthesizer by mean of spectral design
/*!
	It can create spectra directly by manipulating
	partial disposition in frequency domain.
*/
class SpectrumDesigner : public AbstractSpectrum {
public:
	SpectrumDesigner (SoundInfo* info, SignalTable* table, double start,
		double duration, double min = 100, double max = 300,
		int partials = 8);
	virtual ~SpectrumDesigner ();
	void design (Pandora::spectralType type);
	void design (Pandora::spectralType type, std::vector <double> params);
	void purify (double ratio);
	void noise (double band, double gain);
	void jitter (double j);
	void scramblePhases (bool clear = false);
	virtual SoundFrame& tick () {
		double left = 0, right = 0;
		if (!m_m) {
			for (int i = 0; i < m_partials; i++) {
				double temp = m_bank[i]->tick ();
		//		std::cout << temp << std::endl;
				left += (m_pans[i] * m_noise->process (temp));
				right += ((1 - m_pans[i]) * m_noise->process (temp));
			}
		}

		m_frame->set (Pandora::LEFT, left);
		m_frame->set (Pandora::RIGHT, right);
		return *m_frame;
	}
	double min () const;
	double max () const;
	long partials () const;
	LoopOscillator* oscillator (int num) const;
	void update (Subject* s) {}
private:
	SignalTable* m_table;
	LoopOscillator** m_bank;
	Renoise* m_noise;
	UnitRandomNumberGenerator* m_rand;
	RangedRandomNumberGenerator* m_birand;
	double* m_pans;
	long m_partials;
	double m_min;
	double m_max;
};
#endif	// SPECTRUMDESIGNER_H

// EOF


