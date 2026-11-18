// spectralType.h
//

#ifndef SPECTRALTYPE_H
#define SPECTRALTYPE_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for spectral design.
	enum spectralType {
		RANDOM_SPECTRUM,
		ALPHA_SPECTRUM,
		LISSAJOUS_SPECTRUM,
		POLY_SPECTRUM,
		FMLIKE_SPECTRUM
	};
}

#endif	// SPECTRALTYPE_H

// EOF

