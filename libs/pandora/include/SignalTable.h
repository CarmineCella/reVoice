// SignalTable.h
//

#ifndef SIGNALTABLE_H
#define SIGNALTABLE_H

#include <stdexcept>
#include <cmath>
#include <cassert>

#ifndef M_PI
//! PI mathematic constant.
#define M_PI (3.14159265358979323846)
#endif
#ifndef TWOPI
//! Two times the PI.
#define TWOPI (6.28318530717958647692)
#endif

//! Default length for the tables.
#define TABLEN 4096

//! Base class for all the tables.
/*!
	With this class you can build tables for waveforms, envelopes, etc.
	It has not a sampling rate because it is not linked to the time.
	With this class you can generate many types of signals, including
	sum of sines, squared wave, Gaussian windows, Hamming windows, 
	straight convex/concave line segments, etc.
	The table is incremental, ie it NOT clear the data between operations,
    so you should do if necessary.
*/
class SignalTable {
private:
	//! Assignment operator; it is private.
	SignalTable& operator= (SignalTable&);
	//! Copy constructor; it is private.
	SignalTable (const SignalTable&);
public:
	//! Class constructor.
	/*!
		You must pass the length of the table you want to build.
	*/
	SignalTable (int size = TABLEN);
	//! Class destructor.
	virtual ~SignalTable ();
	//! Returns the size of the table.
	long size () const;
	//! Returns the natural frequency of the table based on the given SR.
	double naturalFreq (double sr);
	//! Clears the data in the array, by putting them to 0.
	void clear ();
	//! Returns the x-th element of the table.
	/*!
		The actual element returned is the (x mod len)th. This modulus operation
		while slowing down the perfomance of the class, ensures that no out-of-bounds
		requests will be delivered to the array.
	*/
	double& operator[] (long x) const {
		return m_t[x % m_l];
	}
	//! Fills the table with harominc sines, which frequency ratio is n.
	/*!
		The generation functions of this class \e DO NOT destroy the previous 
		data in the table, so they add the data to the table.
	*/
	void gen (int n, double amp);
	//! Creates a square waveform with n partials.
	void square (int n);
	//! Loads data from the specified array. NB: You must check the bounds of the given array!
	void load (double* array, long len);
	//! Sets the indivual element of the table.
	void set (long n, double val) {
		m_t[n % m_l] = val; 
	}
    //! Sets the individual element of the table at automatic position.
    /*!
        Use this method if you plan to make the SignalTable work as a
        circular buffer. 
	*/
	void set (double val) {
		m_t[m_ptr] = val;
		m_ptr = (m_ptr + 1) % m_l;
	}
    //! Gets the individual element of the table at automatic position.
	double& get () {
		m_get = m_t[m_read];
		m_read = (m_read + 1) % m_l;
		return m_get;
	}	
	void delay (double d);
	double delay () const;
	//! Draws a segment in the table.
	/*!
		It creates straight or convex/concave line segments (depending on the alpha angle) 
		between the point (x0, y0) and the point (x1, y1). If alpha == 0 it draws stright 
		line segments, ie. it performs linear interpolation. This parameters assumes 
		always the same meaning in all the functions.
		The x coordinates ARE the location of the table, so they \e MUST be smaller the 
		the table length.
	*/
	void drawSegment (long x0, double y0, long x1, double y1, double alpha);
	//! Creates an ADSR curve, with the values specified.
	void adsr (double attack, double decay, double alpha);
	//! Creates a rectangular window, by keeping the peak from the beginning to the end of the table.
	void rectangle (double peak);
	//! Creates a Bartlett window (triangular), with the peak specified.
	void bartlett (double peak);
	//! Creates a Gaussian window, with the peak specified.
	void gauss (double peak);
	//! Creates a Hamming window, with the peak specified.
	void hamming (double peak);
	//! Creates a Hanning window, with the peak specified.
	void hanning (double peak);
	//! Creates a Blackman window, with the peak specified.
	void blackman (double peak);
	//! Creates a Blackman-Harris window, with the peak specified.
	void blackmanHarris (double peak);

protected:
	double* m_t;
	long m_ptr;
	long m_read;
	long m_l;
	double m_get;
	double m_del;

	//! Effectively builds the window with the parameters specified by the calling method.
	void makeWindows (double peak, double* cf);
};

#endif	// SIGNALTABLE_H

// EOF

