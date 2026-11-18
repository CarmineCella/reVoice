// AbstractFilter.h
// 

#ifndef ABSTRACTFILTER_H
#define ABSTRACTFILTER_H 

#include "SoundInfo.h"
#include "SoundEffect.h"

//! Base class for all the filters.
/*!
	It is an abstract class. It is not possible to create 
	objects from this class and must be inherited. In this
	way it is possible to ensure a common interface for
	all the filters.
*/
class AbstractFilter : public SoundEffect {
private:
	AbstractFilter& operator= (AbstractFilter&);
	AbstractFilter (const AbstractFilter&);
protected: 
	AbstractFilter (SoundInfo* info) : SoundEffect (info) {}
	virtual ~AbstractFilter () {}
public:
	virtual void cutoff (double c) = 0;
	virtual void reset () = 0;
};

#endif	// ABSTRACTFILTER_H 

// EOF

