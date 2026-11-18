// SoundInfo.h
// 

#ifndef SOUNDINFO_H
#define SOUNDINFO_H 

#include "base/Subject.h"
#include <stdexcept>

//! This is the default sampling rate for all the classes in the framework.
extern const double SAMPLING_RATE;
//! This is the default number of channels for all the classes in the framework.
extern const int CHANNELS;

//! Base configuration class for the framework.
/*!
	Every time-related object in the framework (for example oscillators) must
	refer to a SoundInfo class to query real-time information, such as the
	sampling rate. Actually this class implements the Singleton pattern, 
	so you can have only a single instance of SoundInfo.
*/
class SoundInfo : public Subject {
private:
	//! Default constructor; it's private.
	SoundInfo ();
	//! Assignment operator.
	SoundInfo& operator= (SoundInfo&);
	//! Copy constructor.
	SoundInfo (const SoundInfo&);
	//! Class destructor.
	virtual ~SoundInfo ();
public:
	//! This method returns an unique instance of this class (singleton).
	static SoundInfo* instance ();
	//! Returns the current sample rate.
	double samplingRate () const { return m_sr; }
	//! Sets the current sample rate.
	/*!
		The default sampling rate is defined by SAMPLING_RATE in SoundObject.cpp.
		If you wish to use a rate which is different from the default, 
		you must set it \e BEFORE objects are instantiated.
	*/
	void samplingRate (double sr) { 
		if (sr < 0)
			throw std::runtime_error ("invalid sampling rate");
			
		m_sr = sr;
		notify (); 
	}
	//! Returns the number of channels.
	int channels () const { return m_ch; }
	//! Sets the number of channels.
	/*!
		The default number of channels is defined by CHANNELS in SoundObject.h.
		If you wish to use a rate which is different from the default, 
		you must set it \e BEFORE objects are instantiated.
	*/
	void channels (int ch) { 
		if (ch < 1)
			throw std::runtime_error ("invalid number of channels");
			
		m_ch = ch;
		notify (); 
	}
private: 
	static SoundInfo* m_s;
	double m_sr;
	int m_ch;
};

#endif	// SOUNDINFO_H 

// EOF

