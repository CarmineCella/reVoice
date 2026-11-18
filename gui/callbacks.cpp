// callbacks.cpp
//

#include <sstream>
#include "MyApp.h"
#include "callbacks.h"
#include "Configure.h"
#include <fx.h>

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

extern FXList* msgList;
extern double maxCPULoad;
extern bool trackPitch;

static const int MIN_MIDINOTE = 39;
static const int MAX_MIDINOTE = 88;
static const int MAX_MIDIDELAY = 500;

static const int FREQSCALE = 20;

static int point = 0;
static long count = 0;
static bool first = true;
static double amps[MAX_VOICES];
static double freqs[MAX_VOICES];

double pitch2hertz (double pitch) {
    return double (440.0 * pow (2.0, ((pitch - 69.0) / 12.0)));
}

#ifdef USE_PORTAUDIO
	int audioCallback32Mono (const void* inputBuffer, void* outputBuffer, // audio callback implementation
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData) {
		float* in = (float*) inputBuffer;
		float* out = (float*) outputBuffer;
  		MyApp* app = (MyApp*) userData;

		(void) timeInfo;
		(void) statusFlags;

		for (unsigned long i = 0; i < framesPerBuffer; i++) {
    		if (app->_rose->loadedCPU () >= maxCPULoad) {
				return paContinue;
	    	}
	    	if (app->_live == true) {
          		app->_table->set ((double) *in++);
          	}

         	double left, right;
			SoundFrame f = app->_harmon->tick ();
        	left = f.get (Pandora::LEFT);
        	right = f.get (Pandora::RIGHT);

			double leftMorph, rightMorph;
			SoundFrame m = app->_morph->tick ();
			leftMorph = m.get (Pandora::LEFT) * app->_morphizerLevel;
			rightMorph = m.get (Pandora::RIGHT) * app->_morphizerLevel;

			left += leftMorph;
			right += rightMorph;

			left = app->_reverbLeft->tick (left) * app->_outputMultiplier;
			right = app->_reverbRight->tick (right) * app->_outputMultiplier;

			*out++ = left;
			*out++ = right;

   			app->_left->process (left);
			app->_right->process (right);

			if (app->_recording == true) {
				short leftShort = (short) (left * 32767);
				short rightShort = (short) (right * 32767);
				app->_outFile->write ((void*) &leftShort, sizeof (short));
				if (trackPitch == true) {
        			short pitch = (short) (app->_harmon->pitch () * FREQSCALE);
           			app->_outFile->write ((void*) &pitch, sizeof (short));
    			}
    			else {
					app->_outFile->write ((void*) &rightShort, sizeof (short));
				}
			}
		}

	return paContinue;
    }
	int audioCallback32Stereo (const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData) {

		float* in = (float*) inputBuffer;
		float* out = (float*) outputBuffer;
  		MyApp* app = (MyApp*) userData;

		(void) timeInfo;
		(void) statusFlags;

		for (unsigned long i = 0; i < framesPerBuffer; i++) {
			// TODO: Write your generation code here...
			double left = *in++;
			double right = *in++;
			*out++ = left;
			*out++ = right;

   			app->_left->process (left);
			app->_right->process (right);

			if (app->_recording == true) {
				short leftShort = (short) (left * 32768);
				short rightShort = (short) (right * 32768);
				app->_outFile->write ((void*) &leftShort, sizeof (short));
				app->_outFile->write ((void*) &rightShort, sizeof (short));
			}
		}

	return paContinue;
    }
	int audioCallback32NoInput (const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,  void* userData) {

		//float* in = (float*) inputBuffer;
		float* out = (float*) outputBuffer;
  		MyApp* app = (MyApp*) userData;

		(void) timeInfo;
		(void) statusFlags;

		for (unsigned long i = 0; i < framesPerBuffer; i++) {
			// TODO: Write your generation code here...
			double left = 0;
			double right = 0;

			*out++ = left;
			*out++ = right;

			app->_left->process (left);
			app->_right->process (right);

			if (app->_recording == true) {
				short leftShort = (short) (left * 32768);
				short rightShort = (short) (right * 32768);
				app->_outFile->write ((void*) &leftShort, sizeof (short));
				app->_outFile->write ((void*) &rightShort, sizeof (short));
			}
		}

	return paContinue;
    }

#endif

#ifdef USE_PORTMIDI
    void MIDICallback32 (PtTimestamp timestamp, void *userData) {
        MyApp* app = (MyApp*) userData;

    	PmError result;
    	PmEvent buffer;

    	if (!app->_remi->isRunning ())
    		return;

    	do {
    		result = (PmError) app->_remi->poll ();
    		if (result) {
    			long status, data1, data2;
    			if (app->_remi->read (&buffer, 1) > 0){
       				status = Pm_MessageStatus (buffer.message);
    				data1 = Pm_MessageData1 (buffer.message);
    				data2 = Pm_MessageData2 (buffer.message);
    				// TODO: manage MIDI messages here...

    				if (app->_useMIDIFreq == true) {
    					if (status == 144 && data2 != 0) {
    					    double r = pitch2hertz ((double) data1);
                	    	app->_harmon->fundamental (r, true);
            			}
          			}
          			else if (app->_useMIDIStream == true) {
       					if (status == 145) {
                      		if (data1 >= MIN_MIDINOTE && data1 <=
                      			MAX_MIDINOTE
                      			 && data2 != 0
                      			 && point < app->_harmon->voices ()) {
								if (first == true) {
									count = (long) buffer.timestamp;
									first = false;
								}
								if ((long) buffer.timestamp <= (count + MAX_MIDIDELAY)) {
									point++;
									freqs[point - 1] = pitch2hertz ((double) data1);
									amps[point - 1] = (double) 1 / app->_harmon->voices ();
									if (point == app->_harmon->voices ()) {
	                      				first = true;
	                      				point = 0;
	                      				for (int i = 0; i < app->_harmon->voices (); i++) {
	                      					app->_harmon->ratios (i, amps[i],
	                      						freqs[i], true);
	                      				}
    	                  			}
								}
								else {
									point = 1;
									freqs[point - 1] = pitch2hertz ((double) data1);
									amps[point - 1] = (double) 1 / app->_harmon->voices ();
									count = (long) buffer.timestamp;
								}
                      		}
            			}
          			}

    				if (msgList->shown ()) {
						msgList->makeItemVisible (
     						msgList->appendItem (app->_remi->buildString (status,
     						data1, data2).c_str ()));
     				}
    			}
    		}
    	} while (result);
    }
#endif

// EOF


