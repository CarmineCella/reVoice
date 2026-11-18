// SoundObject.cpp
// 

#include "../include/SoundObject.h"

SoundObject::SoundObject (SoundInfo* info) : Observer (), m_m (false) { 
	m_info = info; 
	m_info->attach (this);
}

SoundObject::~SoundObject () { 
	m_info->detach (this); 
}

// EOF

