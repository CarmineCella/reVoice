// SoundObject.h
// 

#ifndef SOUNDOBJECT_H
#define SOUNDOBJECT_H 

#include "base/Observer.h"
#include "SoundInfo.h"

//! Base class for all the generators.
/*!
	It implements the Subject - Observer pattern, acting as Observer.
	It cannot be istantiated directly and must be inherited.
*/
class SoundObject : public Observer {
private:
	//! Assignment operator; it's private.
	SoundObject& operator= (SoundObject&);
	//! Copy constructor; it's private.
	SoundObject (const SoundObject&);
protected:
	//! Default constructor; it's protected.
	/*!
		Needs the pointer to a SoundInfo object. In this way it can implement
		the Subject-Observer pattern.
	*/
	SoundObject (SoundInfo* info);
	//! Class destructor; it is virtual.
	virtual ~SoundObject ();
public:
	//! With this method you can get the mute flag of the SoundObject.
	/*!
		If the return value is true then the SoundObject is NOT
		scheduled or produces zeroes.
	*/
	virtual bool mute () const {
		return m_m;
	}
	//! Sets the mute flag of the SoundEvent.	
	virtual void mute (bool m) {
		m_m = m;
	}
protected:
	SoundInfo* m_info;
	bool m_m;
};

#endif	// SOUNDOBJECT_H 

// EOF

