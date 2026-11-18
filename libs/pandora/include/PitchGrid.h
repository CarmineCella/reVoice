// PitchGrid.h
//

#ifndef PITCHGRID_H
#define PITCHGRID_H

#include "notes.h"
#include <cmath>

//! Class to compute the frequencies for the tonal system.
class PitchGrid {
private:
	PitchGrid& operator= (PitchGrid&);
	PitchGrid (const PitchGrid&);
public:
	PitchGrid ();
	virtual ~PitchGrid ();
	double get (Pandora::notes n, int oct) const;

private:
	double m_base[12];
};

#endif	// PITCHGRID_H

// EOF


