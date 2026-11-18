// SignalTable.cpp
//

#include "../include/SignalTable.h"

SignalTable::SignalTable (int size) : m_ptr (0), m_read (0), m_get (0), 
	m_del (0) { 
	if (size > 0)
		m_l = size; 
	else
		m_l = TABLEN;
	//m_ptr = (m_l - 1) / 2;
	m_t = new double[m_l]; clear (); 
}

SignalTable::~SignalTable () { delete [] m_t; }

long SignalTable::size () const { return m_l; }

double SignalTable::naturalFreq (double sr) { 
	double f = sr;
	return f / m_l; 
}

void SignalTable::clear () {
	for (long i = 0; i < m_l; i++) {
		m_t[i] = 0;
	}
}

void SignalTable::gen (int n, double amp) {
	for (long i = 0; i < m_l; i++) {
		m_t[i] += (double) sin ((((double) i * n) / 
			(double) (m_l)) * TWOPI) * amp;
	}
}

void SignalTable::square (int n) {
	for (long j = 1; j < n; j += 2)
		gen (j, (double) 1 / j);
}

void SignalTable::load (double* array, long len) {
	assert (array);
	
	if (len > m_l)
		len = m_l;
	for (long n = 0; n < len; n++) {
		m_t[n] += array[n];
	}
}

void SignalTable::delay (double d) {
	m_del = d;
	double delSamples = m_del * (double) (m_l);
	m_ptr = (long) delSamples % m_l;
}

double SignalTable::delay () const { return m_del; }

void SignalTable::drawSegment (long x0, double y0, long x1, double y1, double alpha) {
	long cnt = (x1 - x0);
	
	if ((cnt + x0) >= m_l)
		throw std::runtime_error ("invalid table position");
	
	double val = y0;
	double nxtval = y1;
	long index = 0;
	
	if (alpha == 0.0) {
		double c1 = (nxtval - val) / cnt;
		while (cnt-- > 0) {
			m_t[index++ + x0] += val = val + c1;
		}
	}
	else {
		double c1 = (nxtval - val) / (1.0 - (double) exp ((double) alpha));
		double x;
		alpha /= cnt;
		x = alpha;
		while (cnt-- > 0) {
			m_t[index++ + x0] += val + c1 * (1.0 - (double) exp ((double) (x)));
			x += alpha;
		}
	}
}

void SignalTable::adsr (double attack, double decay, double alpha) {
	drawSegment (0, 0, m_l / 4, attack, alpha);
	drawSegment (m_l / 4, attack, m_l / 2, decay, alpha);
	drawSegment (m_l / 2, decay, 3 * (m_l / 4), decay, alpha);
	drawSegment (3 * (m_l / 4), decay, m_l - 1, 0, alpha);
}

void SignalTable::rectangle (double peak) {
	drawSegment (0, peak, m_l - 1, peak, 0);
}

void SignalTable::bartlett (double peak) {
	double arg = 2.0 / m_l, i;
	int index = 0;
	for (i = 0.0; i < m_l / 2.0; i++)
		m_t[index++] += (double)(i * arg * peak);
	for (; i < m_l; i++)
		m_t[index++] += (double)((2.0 - i * arg) * peak);
}

void SignalTable::gauss (double peak) {
	double arg = 12.0 / m_l;
	int index = 0;
	for (double i = -6.0; i < 0.0; i += arg)
		m_t[index++] += (double)(peak * (pow (2.71828, - (i * i) / 2.0)));
	for (double i = arg; i < 6.0; i += arg)
		m_t[index++] += (double)(peak * (pow (2.71828, - (i * i) / 2.0)));
}

void SignalTable::hamming (double peak) {
	double cf[4];
	cf[0] = 0.54;
	cf[1] = 0.46;
	cf[2] = cf[3] = 0.0;
	
	makeWindows (peak, cf);
}

void SignalTable::hanning (double peak) {
	double cf[4];
	cf[0] = cf[1] = 0.5;
	cf[2] = cf[3] = 0.0;

	makeWindows (peak, cf);
}

void SignalTable::blackman (double peak) {
	double cf[4];
	cf[0] = 0.42;
	cf[1] = 0.5;
	cf[2] = 0.08;
	cf[3] = 0.0;

	makeWindows (peak, cf);
}

void SignalTable::blackmanHarris (double peak) {
	double cf[4];
	cf[0] = 0.35878;
	cf[1] = 0.48829;
	cf[2] = 0.14128;
	cf[3] = 0.01168;

	makeWindows (peak, cf);
}

void SignalTable::makeWindows (double peak, double* cf) {
	double arg = TWOPI / m_l;
	long index = 0;

	for (double i = 0.0; i < TWOPI; i += arg)
		m_t[index++] += (double)(peak * (cf[0] - cf[1] * cos (i) + cf[2] * 
			cos (2.0 * i) - cf[3] * cos (3.0 * i)));
}

// EOF

