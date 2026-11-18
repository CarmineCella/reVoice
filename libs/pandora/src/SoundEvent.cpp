// SoundEvent.cpp
// 

#include "../include/SoundEvent.h"

SoundEvent::SoundEvent (SoundInfo* info, double st, double dur) 
	: SoundGenerator<SoundFrame> (info), m_leftPosition (.5), 
		m_rightPosition (.5) {
	if (m_info->channels () == 2)
		m_frame = new SoundFrame (true);
	else
		m_frame = new SoundFrame (false);  
	start (st);
	duration (dur);
}

SoundEvent::~SoundEvent () {
	delete m_frame; 
}

void SoundEvent::start (double st) {
	if (st > 0)
		m_st = st;
	else
		m_st = 0;
	m_startTime = (long) (m_info->samplingRate () * m_st);
}

double SoundEvent::start () const {
	return m_st;
}

void SoundEvent::duration (double dur) {
	if (dur > 0)
		 m_dur = dur;
	else
		m_dur = 1;
	m_samples = (long) (m_info->samplingRate () * m_dur);
}

double SoundEvent::duration () const {
	return m_dur;
}

bool SoundEvent::isActive (long index) {
	if (m_samples <= 0)
		return true;
	else if ((index >= m_startTime) && (index <= m_samples + m_startTime))
		return true;
	return false;
}

double SoundEvent::spatialPosition () const { return m_rightPosition; }

void SoundEvent::spatialPosition (double position) {
	if (position >= 0 && position <= 1) {
		m_rightPosition = position;
		m_leftPosition = 1 - m_rightPosition;
	}
}

void SoundEvent::update (Subject* s) {
	m_samples = (long) (m_info->samplingRate () * m_dur);
	m_startTime = (long) (m_info->samplingRate () * m_st);
	if (m_info->channels () == 2)
		m_frame->isStereo (true);
	else
		m_frame->isStereo (false);
}

// EOF

