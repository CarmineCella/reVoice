// AudioFiles.h
//

#ifndef AUDIOFILES_H
#define AUDIOFILES_H

#include <cstdio>
#include <stdexcept>
#include <string>
#include <cassert>
#include <limits>

#define RIFFSTR "RIFF"
#define WAVESTR "WAVE"
#define FMTSTR "fmt "
#define DATASTR "data"

#ifdef BYTE_ORDER
    #if BYTE_ORDER == BIG_ENDIAN
        #define m_BIG_ENDIAN_
    #endif
#endif
    
#ifndef uint
	typedef unsigned int uint;
#endif           

//! Wav RIFF structure.
typedef struct {
    char riffChar[4];
    int  packageLen;
    char wave[4];
} WavRiff;

//! Wav format structure.
typedef struct {
    char  fmt[4];
    int   formatLen;
    short fixed;
    short channelNumber;
    int   sampleRate;
    int   byteRate;
    short bytePerSample;
    short bitsPerSample;
} WavFormat;

//! Wav data structure.
typedef struct {
    char  dataField[4];
    uint  dataLen;
} WavData;

//! Wav header structure.
typedef struct {
    WavRiff   riff;
    WavFormat format;
    WavData   data;
} WavHeader;

//! Minimal wave file support for input.
class WavInFile {
private:
	//! Assignment operator; it is private.
	WavInFile& operator= (WavInFile&);
	//! Copy constructor; it is private.
	WavInFile (const WavInFile&);
public:
	WavInFile (const char *fileName);
	virtual ~WavInFile ();
	void close ();
    void rewind ();
	uint getNumChannels () const ;
	uint getNumBits () const;
	uint getBytesPerSample() const;
	uint getSampleRate() const;
	uint getDataSizeInBytes() const;
	uint getNumSamples() const;
	uint getLengthMS() const;
	int read (void *buffer, const int bufferSizeInBytes);
	int eof () const;

private:
    FILE *fptr;
    uint dataRead;
    WavHeader header;

	int readWavHeaders ();
    int checkCharTags ();
    int isAlpha (char c);
	int isAlphaStr (char *str);
	int readHeaderBlock ();
	int readRIFFBlock ();
};

//! Minimal wave file support for output (16 bit only).
class WavOutFile {
private:
	//! Assignment operator; it is private.
	WavOutFile& operator= (WavOutFile&);
	//! Copy constructor; it is private.
	WavOutFile (const WavOutFile&);
public:
	WavOutFile (const char *fileName, const int sampleRate, 
		const int bits, const int channels);
	virtual ~WavOutFile ();

	void write (const void *buffer, const int numBytes);
	void close ();

private:
    FILE *fptr;
    WavHeader header;
    int bytesWritten;

	void fillInHeader (uint sampleRate, uint bits, uint channels);
	void finishHeader ();
	void writeHeader ();
};

#endif	// AUDIOFILES_H

// EOF



