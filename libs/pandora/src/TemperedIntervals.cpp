// TemperedIntervals.cpp
// 
#include "../include/TemperedIntervals.h"

TemperedIntervals::TemperedIntervals (bool tempered) {
//		for (int i = 0; i < 12; i++) {
//			double p = pow (2, (double) 1 / 12);
//			m_base[i] = pow (p, (i + 1));
//		}
	calcRatios (tempered);
}

TemperedIntervals::~TemperedIntervals () {}
	
void TemperedIntervals::temperedTuning (bool t) { calcRatios (t); }

void TemperedIntervals::calcRatios (bool tempered) {
	if (tempered == false) {
		m_base[0] = 1;
		m_base[1] = 1.0909;
		m_base[2] = 1.1250;
		m_base[3] = 1.2000;
		m_base[4] = 1.2500;
		m_base[5] = 1.3333;
		m_base[6] = 1.4000;
		m_base[7] = 1.5000;
		m_base[8] = 1.6000;
		m_base[9] = 1.6667;
		m_base[10] = 1.7500;
		m_base[11] = 1.8333;
	}
	else {
		m_base[0] = 1;
		m_base[1] = 1.0595;
		m_base[2] = 1.1225;
		m_base[3] = 1.1892;
		m_base[4] = 1.2599;
		m_base[5] = 1.3348;
		m_base[6] = 1.4142;
		m_base[7] = 1.4983;
		m_base[8] = 1.5874;
		m_base[9] = 1.6818;
		m_base[10] = 1.7818;
		m_base[11] = 1.8897;
	}	
}

// EOF


