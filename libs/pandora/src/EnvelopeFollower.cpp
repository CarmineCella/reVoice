// EnvelopeFollower.cpp
//

#include "../include/EnvelopeFollower.h"

EnvelopeFollower::EnvelopeFollower (SoundInfo* info, double feedback) 
	: Tone (info, feedback) {}

EnvelopeFollower::~EnvelopeFollower () {}

double EnvelopeFollower::get () const { return m_val; }

void EnvelopeFollower::update (Subject* s) {
	// don't call Tone::update ()!
}

// EOF


