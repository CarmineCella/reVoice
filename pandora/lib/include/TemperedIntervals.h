// TemperedIntervals.h
// created: Mon Jan 10 15:27:55 2005
// 

#ifndef TEMPEREDINTERVALS_H
#define TEMPEREDINTERVALS_H 

#include "intervals.h"
#include <cmath>

//! Class for the tempered/natural ratios.
class TemperedIntervals {
private:
	TemperedIntervals& operator= (TemperedIntervals&);
	TemperedIntervals (const TemperedIntervals&);
public: 
	TemperedIntervals (bool tempered = true);
	virtual ~TemperedIntervals ();
	void temperedTuning (bool t);
	double get (Pandora::intervals i) const {
		return m_base[(int) i - 1]; 
	}
private:
	void calcRatios (bool tempered);

	double m_base[12];
};

#endif	// TEMPEREDINTERVALS_H 

// EOF


