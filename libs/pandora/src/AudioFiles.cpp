// AudioFiles.cpp
//

#include "../include/AudioFiles.h"
    
#ifdef m_BIG_ENDIAN_
    static inline void m_swap32 (unsigned int &dwData) {
        dwData = ((dwData >> 24) & 0x000000FF) | 
                 ((dwData >> 8)  & 0x0000FF00) | 
                 ((dwData << 8)  & 0x00FF0000) | 
                 ((dwData << 24) & 0xFF000000);
    }   
    static inline void m_swap16(unsigned short &wData) {
        wData = ((wData >> 8) & 0x00FF) | 
                ((wData << 8) & 0xFF00);
    }
    static inline void m_swap16Buffer( unsigned short *pData, 
	unsigned int dwNumWords) {
        unsigned long i;

        for (i = 0; i < dwNumWords; i ++) {
            m_swap16(pData[i]);
        }
    }

#else   // BIG_ENDIAN
    static inline void m_swap32(unsigned int &dwData) {}   
    static inline void m_swap16 (unsigned short &wData) {}
    static inline void m_swap16Buffer (unsigned short *pData, 
		unsigned int dwNumBytes) {}
#endif  // BIG_ENDIAN

WavInFile::WavInFile (const char *fileName) {
	int hdrsOk;
	fptr = fopen (fileName, "rb");
	if (fptr == NULL) {
		std::string msg = (std::string) "unable to open file \"";
		msg += fileName;
		msg += "\" for reading";
		throw std::runtime_error (msg);
	}
	hdrsOk = readWavHeaders ();
	if (hdrsOk != 0) {
		std::string msg = (std::string) "file \"";
		msg += fileName;
		msg += "\" is corrupt or not a WAV file";
		close ();
		throw std::runtime_error (msg);
	}

	if (header.format.fixed != 1) {
		std::string msg = (std::string) "file \"";
		msg += fileName;
		msg += "\" uses unsupported encoding";
		close ();
		throw std::runtime_error (msg);
	}

	dataRead = 0;
}

WavInFile::~WavInFile () { 
	if (fptr != NULL) {
		close (); 
	}
}

void WavInFile::close () {
	fclose (fptr);
	fptr = NULL;
}

void WavInFile::rewind () {
	int hdrsOk;
	fseek (fptr, 0, SEEK_SET);
	hdrsOk = readWavHeaders ();
	assert (hdrsOk == 0);
	dataRead = 0;
}

uint WavInFile::getNumChannels () const {
	return header.format.channelNumber;
}

uint WavInFile::getNumBits () const {
	return header.format.bitsPerSample;
}

uint WavInFile::getBytesPerSample() const {
	return getNumChannels() * getNumBits() / 8;
}

uint WavInFile::getSampleRate() const {
	return header.format.sampleRate;
}

uint WavInFile::getDataSizeInBytes() const {
	return header.data.dataLen;
}

uint WavInFile::getNumSamples() const {
	return header.data.dataLen / header.format.bytePerSample;
}

uint WavInFile::getLengthMS() const {
   uint numSamples;
   uint sampleRate;

   numSamples = getNumSamples ();
   sampleRate = getSampleRate ();

   assert (numSamples < UINT_MAX / 1000);
   return (1000 * numSamples / sampleRate);
}

int WavInFile::read (void *buffer, const int bufferSizeInBytes) {
	unsigned int afterDataRead;
	int num;
	num = bufferSizeInBytes;
	afterDataRead = dataRead + num;
	if (afterDataRead > header.data.dataLen) {
		num = header.data.dataLen - dataRead;
		assert (num >= 0);
	}
	num = fread (buffer, 1, num, fptr);
	dataRead += num;
	if (header.format.bitsPerSample == 16) {
		m_swap16Buffer ((unsigned short *) buffer, num / 2);
	}
	else if (header.format.bitsPerSample != 8) {
		 throw std::runtime_error 
			("unsupported encondings requested for reading");
	}
	return num;
}

int WavInFile::eof () const {
	return (dataRead == header.data.dataLen || feof (fptr));
}

int WavInFile::readWavHeaders () {
	int res;
	memset (&header, 0, sizeof (header));

	res = readRIFFBlock ();
	if (res) return 1;
	do {
		res = readHeaderBlock();
		if (res < 0) return 1;  // error in file structure
	} while (res == 0);
	return checkCharTags ();
}
int WavInFile::checkCharTags () {
	if (memcmp (FMTSTR, header.format.fmt, 4) != 0) return -1;
	if (memcmp (DATASTR, header.data.dataField, 4) != 0) return -1;
	return 0;
}

int WavInFile::isAlpha (char c) {
	return (c >= ' ' && c <= 'z') ? 1 : 0;
}

int WavInFile::isAlphaStr (char *str) {
	int c;

	c = str[0];
	while (c) {
		if (isAlpha (c) == 0) return 0;
		str ++;
		c = str[0];
	}

	return 1;
}

int WavInFile::readHeaderBlock () {
	char label[5];
	std::string sLabel;
	fread (label, 1, 4, fptr);
	label[4] = 0;

	if (isAlphaStr (label) == 0) return -1;    // not a valid label
	if (strcmp (label, FMTSTR) == 0) {
		int nLen, nDump;
		memcpy (header.format.fmt, FMTSTR, 4);
		fread (&nLen, sizeof (int), 1, fptr);
		m_swap32 ((unsigned int&) nLen); // int formatLen;
		header.format.formatLen = nLen;
		nDump = nLen - (sizeof (header.format) - 8);
		if (nDump > 0) {
			nLen = sizeof (header.format) - 8;
		}
		fread (&(header.format.fixed), nLen, 1, fptr);
		m_swap16 ((unsigned short&) header.format.fixed); // short int fixed;
		m_swap16 ((unsigned short&) header.format.channelNumber); // short int channelNumber;
		m_swap32 ((unsigned int&) header.format.sampleRate); // int sampleRate;
		m_swap32 ((unsigned int&) header.format.byteRate); // int byteRate;
		m_swap16 ((unsigned short&) header.format.bytePerSample); // short int bytePerSample;
		m_swap16 ((unsigned short&) header.format.bitsPerSample); // short int bitsPerSample;

		if (nDump > 0) {
			fseek (fptr, nDump, SEEK_CUR);
		}
		return 0;
	}
	else if (strcmp (label, DATASTR) == 0) {
		memcpy (header.data.dataField, DATASTR, 4);
		fread (&(header.data.dataLen), sizeof (uint), 1, fptr);
		m_swap32 ((unsigned int&) header.data.dataLen);
		return 1;
	}
	else {
		uint len, i;
		uint temp;
		fread (&len, sizeof (len), 1, fptr);
		for (i = 0; i < len; i ++) {
			fread (&temp, 1, 1, fptr);
			if (feof (fptr)) return -1;   // unexpected eof
		}
	}
	return 0;
}

int WavInFile::readRIFFBlock () {
	fread (&(header.riff), sizeof (WavRiff), 1, fptr);
	m_swap32 ((unsigned int &) header.riff.packageLen);
	if (memcmp (RIFFSTR, header.riff.riffChar, 4) != 0) return -1;
	if (memcmp (WAVESTR, header.riff.wave, 4) != 0) return -1;
	return 0;
}

WavOutFile::WavOutFile (const char *fileName, const int sampleRate, 
	const int bits, const int channels) {
	bytesWritten = 0;
	fptr = fopen (fileName, "wb");
	if (fptr == NULL) {
		std::string msg = "unable to open file \"";
		msg += fileName;
		msg += "\" for writing";
		throw std::runtime_error (msg);
	}

	fillInHeader (sampleRate, bits, channels);
	writeHeader ();
}

WavOutFile::~WavOutFile () {
	if (fptr != NULL) {
		close ();
	}
}

void WavOutFile::write (const void *buffer, const int numBytes) {
	int res;

	if (header.format.bitsPerSample == 16) {
		unsigned short *pTemp;
		pTemp = new unsigned short[numBytes / 2];
		memcpy (pTemp, buffer, numBytes);
		m_swap16Buffer (pTemp, numBytes / 2);

		res = fwrite (pTemp, 1, numBytes, fptr);

		delete[] pTemp;
	}
	else if (header.format.bitsPerSample == 8) {
		res = fwrite (buffer, 1, numBytes, fptr);
	}
	else {
		throw std::runtime_error 
			("unsupported enconding requested for writing");
	}

	if (res != numBytes) {
		throw std::runtime_error ("error while writing to a wav file");
	}

	bytesWritten += numBytes;
}

void WavOutFile::close () {
	finishHeader ();
	fclose (fptr);
	fptr = NULL;
}

void WavOutFile::fillInHeader (uint sampleRate, uint bits, uint channels) {
	memcpy (&(header.riff.riffChar), RIFFSTR, 4);
	header.riff.packageLen = 0;
	memcpy (&(header.riff.wave), WAVESTR, 4);
	memcpy (&(header.format.fmt), FMTSTR, 4);

	header.format.formatLen = 0x10;
	header.format.fixed = 1;
	header.format.channelNumber = (short) channels;
	header.format.sampleRate = sampleRate;
	header.format.bitsPerSample = (short) bits;
	header.format.bytePerSample = (short) (bits * channels / 8);
	header.format.byteRate = header.format.bytePerSample * sampleRate;
	header.format.sampleRate = sampleRate;

	memcpy (&(header.data.dataField), DATASTR, 4);
	header.data.dataLen = 0;
}

void WavOutFile::finishHeader () {
	header.riff.packageLen = bytesWritten + 36;
	header.data.dataLen = bytesWritten;
	writeHeader ();
}

void WavOutFile::writeHeader () {
	WavHeader hdrTemp;
	hdrTemp = header;
	m_swap32 ((unsigned int&) hdrTemp.riff.packageLen);
	m_swap32 ((unsigned int&) hdrTemp.format.formatLen);
	m_swap16 ((unsigned short&) hdrTemp.format.fixed);
	m_swap16 ((unsigned short&) hdrTemp.format.channelNumber);
	m_swap32 ((unsigned int&) hdrTemp.format.sampleRate);
	m_swap32 ((unsigned int&) hdrTemp.format.byteRate);
	m_swap16 ((unsigned short&) hdrTemp.format.bytePerSample);
	m_swap16 ((unsigned short&) hdrTemp.format.bitsPerSample);
	m_swap32 ((unsigned int&) hdrTemp.data.dataLen);

	fseek (fptr, 0, SEEK_SET);
	fwrite (&hdrTemp, sizeof (hdrTemp), 1, fptr);
	fseek (fptr, 0, SEEK_END);
}

// EOF



