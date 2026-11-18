// CrossFade.cpp
// 

#include "../include/CrossFade.h"

CrossFade::CrossFade (SoundInfo* info, double start, double duration,
	SoundEvent* e1, SoundEvent* e2, double curve)
	: SoundEvent (info, start, duration) {
	m_e1 = e1;
	m_e2 = e2;
	
	m_env = new Envelope (info, TABLEN);
	m_env->drawSegment (0, 1, 4095, 0, curve);
	m_env->mapTo (duration);
}

CrossFade& CrossFade::curve (double c) {
	m_env->clear ();
	m_env->drawSegment (0, 1, 4095, 0, c);
	m_env->mapTo (m_dur);
	
	return *this;
}
CrossFade::~CrossFade () {
	delete m_env;
}

void CrossFade::update (Subject* s) {
}

// EOF


