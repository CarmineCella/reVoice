// harmonizationLogic.h
//

#ifndef HARMONIZATIONLOGIC_H
#define HARMONIZATIONLOGIC_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for harmonization logic.
	enum harmonizationLogic {
		CUSTOM_CHORD,
		DOMINANT_SEVENTH,
		MINOR_SEVENTH,
		MAJOR_SEVENTH,
		DIMINISHED_SEVENTH,
		AUGMENTED_SEVENTH,
		QUARTAL_CHORD,
		SCALIC,
		INTELLICHORD,
		MINOR_WITH_SIXTH,
		SINGLECHORD
	};
}

#endif	// HARMONIZATIONLOGIC_H

// EOF

