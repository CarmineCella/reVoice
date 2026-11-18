// windowType.h
//

#ifndef WINDOWTYPE_H
#define WINDOWTYPE_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for the window types.
	enum windowType {
	    RECTANGLE,
	    BARTLETT,
	    GAUSSIAN,
	    HAMMING,
	    HANNING,
	    BLACKMAN,
	    BLACKMAN_HARRIS,
	    ADSR
	};
}

#endif  // WINDOWTYPE_H

// EOF



