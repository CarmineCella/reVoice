// callbacks.h
//

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "Rose.h"
#include "Remi.h"

#ifdef USE_PORTAUDIO
	int audioCallback32Mono (const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData);
	int audioCallback32Stereo (const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData);
	int audioCallback32NoInput (const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData);
#endif

#ifdef USE_PORTMIDI
    void MIDICallback32 (PtTimestamp timestamp, void *userData);
#endif

#endif	// CALLBACKS_H

// EOF

