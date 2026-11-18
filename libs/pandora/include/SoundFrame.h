// SoundFrame.h
// 

#ifndef SOUNDFRAME_H
#define SOUNDFRAME_H

#include "channel.h"
#include <limits>

//! SoundFrame class.
/*!
	Every SoundObject in the framework should produce samples of this type.
*/
class SoundFrame {
public: 
    //! Public variable for real-time access.
   	bool m_stereo;
	SoundFrame (const SoundFrame& right);  
	SoundFrame ();
	SoundFrame (bool stereo);
	virtual ~SoundFrame ();
    //! Sets the number of channels for the frame (1 or 2).
	void isStereo (const bool stereo);
	//! Returns true if the frame is stereo.
	bool isStereo () const ;
	//! Sets the value for the sample.
	void set (Pandora::channel ch, double sample) { m_buffer[ch] = sample; }
	//! Adds the value of the sample without erasing previous data.
	void add (Pandora::channel ch, double sample) { m_buffer[ch] += sample; }
	//! Multiplies the value of the sample with the previous sample.
	void mul (Pandora::channel ch, double sample) { m_buffer[ch] *= sample; }
	//! Gets the sample value (double).
	virtual double get (Pandora::channel ch) const { return m_buffer[ch]; }
	//! Gets the address of the frame buffer.
	double* getBuffer () {
		return m_buffer;
	}
	//! Gets the sample value as short in the range -32768 to 32767.
	short int get16bit (Pandora::channel ch) {
		double s = m_buffer[ch];
		s *= m_max;
		return ((short int) s);
	}
	//! Gets the sample value as float in the range -1 to 1.
	float get32bit (Pandora::channel ch) {
		return ((float) m_buffer[ch]);
	}
	//! Almost every possible operation with the SoundFrame has an overloaded operator.	
	SoundFrame& operator= (const SoundFrame& r) {
		if (this != &r) {
			m_stereo = r.m_stereo;
			m_max = r.m_max;
			m_buffer[0] = r.get (Pandora::LEFT);
			m_buffer[1] = r.get (Pandora::RIGHT); 
		}
		return *this;
	}
	const SoundFrame operator+ (const SoundFrame& right) {
		SoundFrame f (true);
		f.set (Pandora::LEFT, m_buffer[0] + right.get (Pandora::LEFT));
		f.set (Pandora::RIGHT, m_buffer[1] + right.get (Pandora::RIGHT));
		return f; 
	}
	
	const SoundFrame operator- (const SoundFrame& right) {
		SoundFrame f (true);
		f.set (Pandora::LEFT, m_buffer[0] - right.get (Pandora::LEFT));
		f.set (Pandora::RIGHT, m_buffer[1] - right.get (Pandora::RIGHT));
		return f; 
	}
	
	const SoundFrame operator* (const SoundFrame& right) {
		SoundFrame f (true);
		f.set (Pandora::LEFT, m_buffer[0] * right.get (Pandora::LEFT));
		f.set (Pandora::RIGHT, m_buffer[1] * right.get (Pandora::RIGHT));
		return f; 
	}
	//! Operator / is not checked for zero!	
	const SoundFrame operator/ (const SoundFrame& right) {
		SoundFrame f (true);
		f.set (Pandora::LEFT, m_buffer[0] / right.get (Pandora::LEFT));
		f.set (Pandora::RIGHT, m_buffer[1] / right.get (Pandora::RIGHT));
		return f; 
	}
	
	SoundFrame& operator+= (const SoundFrame& right) {
		if (this != &right) {
			m_buffer[0] += right.get (Pandora::LEFT);
			m_buffer[1] += right.get (Pandora::RIGHT);
		}
				
		return *this;
	}
	
	SoundFrame& operator-= (const SoundFrame& right) {
		if (this != &right) {
			m_buffer[0] -= right.get (Pandora::LEFT);
			m_buffer[1] -= right.get (Pandora::RIGHT);
		}
				
		return *this;
	}
	
	SoundFrame& operator*= (const SoundFrame& right) {
		if (this != &right) {
			m_buffer[0] *= right.get (Pandora::LEFT);
			m_buffer[1] *= right.get (Pandora::RIGHT);
		}
				
		return *this;
	}
	
	SoundFrame& operator/= (const SoundFrame& right) {
		if (this != &right) {
			m_buffer[0] /= right.get (Pandora::LEFT);
			m_buffer[1] /= right.get (Pandora::RIGHT);
		}
				
		return *this;
	}	
	void clear ();
private:
	double m_buffer[2];
	short m_max;
};

#endif	// SOUNDFRAME_H 

// EOF

