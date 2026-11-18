// Exponator.h
// created: Sun Jan 29 20:20:42 2006
// 

#ifndef EXPONATOR_H
#define EXPONATOR_H 

#include "SoundInfo.h"
#include "SoundEffect.h"
#include "PulseGenerator.h"
#include "LoopOscillator.h"
#include "SignalTable.h"

class Exponator : public SoundEffect {
private:
	Exponator& operator= (Exponator&);
	Exponator (const Exponator&);
public: 
	Exponator (SoundInfo* info, double index = 1, 
		double frequency = 1, double speed = 1);
	virtual ~Exponator ();
	double& process (double& s);
	void index (double i);
	void frequency (double i);	
	void speed (double i);
	void update (Subject* s) {}	
private:
	double m_expon;
	double m_index;
	PulseGenerator* m_pulse;
	SignalTable* m_envelope;	
	LoopOscillator* m_osc;
};

#endif	// EXPONATOR_H 

// EOF

