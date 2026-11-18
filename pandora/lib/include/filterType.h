// filterType.h
//

#ifndef FILTERTYPE_H
#define FILTERTYPE_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for filter types.
	enum filterType { 
		LOWPASS = 0,
		HIGHPASS = 1,
		BANDPASS = 2,
		NOTCH = 3
	};
}

#endif  // FILTERTYPE_H

// EOF



