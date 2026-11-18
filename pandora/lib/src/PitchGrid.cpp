// PitchGrid.cpp
//

#include "../include/PitchGrid.h"

PitchGrid::PitchGrid () {
	m_base[0] = (16.352);
	m_base[1] = (17.324);
	m_base[2] = (18.354);
	m_base[3] = (19.445);
	m_base[4] = (20.602);
	m_base[5] = (21.827);
	m_base[6] = (23.125);
	m_base[7] = (24.500);
	m_base[8] = (25.957);
	m_base[9] = (27.500);
	m_base[10] = (29.135);
	m_base[11] = (30.868);
}

PitchGrid::~PitchGrid () {
}

double PitchGrid::get (Pandora::notes n, int oct) const {
	return pow ((double) 2, (double) oct) * m_base[(int) n];
}

// EOF


