// CrossFade.h
// 

#ifndef CROSSFADE_H
#define CROSSFADE_H 

#include "SoundEvent.h"
#include "SoundInfo.h"
#include "SoundFrame.h"
#include "Envelope.h"

//! Symmetric cross-fade between two sources
/*!
	Cross fades between two SoundEvent objects;
	the time of cross-fade alters the individual
	life times of the objects.
*/
class CrossFade : public SoundEvent  {
private:
	CrossFade& operator= (CrossFade&);
	CrossFade (const CrossFade&);
public: 
	CrossFade (SoundInfo* info, double start, double duration,
	SoundEvent* e1, SoundEvent* e2, double curve = 0);
	virtual ~CrossFade ();
	
	CrossFade& curve (double c);
	virtual SoundFrame& tick () {
		if (!m_m) {
			SoundFrame f1 = m_e1->tick ();
			SoundFrame f2 = m_e2->tick ();
			double env = m_env->tick ();
			
			f1.mul (Pandora::LEFT, env);
			f1.mul (Pandora::RIGHT, env);
			
			f2.mul (Pandora::LEFT, 1 - env);
			f2.mul (Pandora::RIGHT, 1 - env);
			*m_frame = f1 + f2; 
		}
		return *m_frame;       
	}
    virtual void update (Subject* s);
private:
	SoundEvent* m_e1;
	SoundEvent* m_e2;
    Envelope* m_env;
};

#endif	// CROSSFADE_H 

// EOF


