// intervals.h
//

#ifndef INTERVALS_H
#define INTERVALS_H

//! Main namespace for the framework.
/*! It containts enumerations only at the moment.
 */
namespace Pandora {
	//! Enumeration for tempered intervals.
	enum intervals {
		UNISON_INTERVAL = 1,
		MINOR_SECOND_INTERVAL = 2,
		MAJOR_SECOND_INTERVAL = 3,
		MINOR_THIRD_INTERVAL = 4,
		MAJOR_THIRD_INTERVAL = 5,
		PERFECT_FOURTH_INTERVAL = 6,
		TRITONE_INTERVAL = 7,
		PERFECT_FIFTH_INTERVAL = 8,
		MINOR_SIXTH_INTERVAL = 9,
		MAJOR_SIXTH_INTERVAL = 10,
		MINOR_SEVENTH_INTERVAL = 11,
		MAJOR_SEVENTH_INTERVAL = 12
	};
}
#endif	// INTERVALS_H

// EOF

