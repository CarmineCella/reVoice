// SoundInfo.cpp
// 

#include "../include/SoundInfo.h"
#include <iostream>
#include <cassert>

using namespace std;

const double SAMPLING_RATE = 44100.0;
const int CHANNELS = 1;

SoundInfo::SoundInfo () : Subject (), m_sr (SAMPLING_RATE), m_ch (CHANNELS) {}
SoundInfo::~SoundInfo () {}
SoundInfo* SoundInfo::m_s = 0;
SoundInfo* SoundInfo::instance () {
	if (m_s == 0) {
		m_s = new SoundInfo;
	}		
	assert (m_s);
	return m_s;
}

// EOF

