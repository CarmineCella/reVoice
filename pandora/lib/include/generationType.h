// generationType.h
//

#ifndef GENERATIONTYPE_H
#define GENERATIONTYPE_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for generation types.
	enum generationType {
	    ASYNC = 0,
	    GRAINLET = 1,
	    PITCH_SYNC = 2
	};
}

#endif  // GENERATIONTYPE_H

// EOF


