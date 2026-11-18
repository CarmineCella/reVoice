// SoundFrame.h
// 

#include "../include/SoundFrame.h"

SoundFrame::SoundFrame (const SoundFrame& right)
	: m_stereo (right.m_stereo), m_max (right.m_max) {
	m_buffer[0] = right.get (Pandora::LEFT);
	m_buffer[1] = right.get (Pandora::RIGHT);
}    

SoundFrame::SoundFrame () : m_stereo (false) {
	m_max = std::numeric_limits<short int>::max ();
	clear ();
}

SoundFrame::SoundFrame (bool stereo) : m_stereo (stereo) {
	m_max = std::numeric_limits<short int>::max ();
	clear ();
}

SoundFrame::~SoundFrame () {}

void SoundFrame::isStereo (const bool stereo) { 
	m_stereo = stereo; 
	clear ();
}

bool SoundFrame::isStereo () const { return m_stereo; }

void SoundFrame::clear () {
	m_buffer[0] = 0;
	m_buffer[1] = 0;
}

// EOF

