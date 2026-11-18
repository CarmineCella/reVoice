// SoundEffect.h
// 

#include "../include/SoundEffect.h"

SoundEffect::SoundEffect (SoundInfo* info) : SoundProcessor<double> (info), m_val (0) {}
SoundEffect::~SoundEffect () {}

// EOF

