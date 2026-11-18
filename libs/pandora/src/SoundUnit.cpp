// SoundUnit.cpp
// 

#include "../include/SoundUnit.h"

SoundUnit::SoundUnit (SoundInfo* info) : SoundGenerator<double> (info), m_val (0) {}
SoundUnit::~SoundUnit () {}

// EOF

