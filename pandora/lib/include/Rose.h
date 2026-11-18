// Rose.h
// 

#ifndef ROSE_H
#define ROSE_H 

#define USE_PORTAUDIO

#include "SoundInfo.h"
#include "SoundObject.h"
#if defined (USE_PORTAUDIO)
	#include "portaudio.h"
	#if defined (USE_ASIO)
		#include "pa_asio.h"
	#endif
#elif defined (USE_RTAUDIO)
	#error RtAudio non implemented yet!!
#endif
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cassert>
        
#if defined (USE_PORTAUDIO)
    typedef int (*audioCallback)(const void *inputBuffer, void *outputBuffer,
    	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    	PaStreamCallbackFlags statusFlags,  void *userData);
#else
	typedef int (*audioCallback) (void);
#endif
		
//! Realtime oriented sound engine (R.o.s.e.)
/*! 
	This class enables the live audio I/O and is based
	on a portable audio framework (PortAudio).
*/
class Rose : public SoundObject {
private:
	Rose& operator= (Rose&);
	Rose (const Rose&);
public:
	Rose (SoundInfo* info, int playback = 0,
 		int record = 0, int frames = 256, int buffers = 4) 
   		: SoundObject (info), m_playback (playback),
 		m_record (record), m_framesPerBuffer (frames), m_buffers (buffers) {
	    #if defined (USE_PORTAUDIO)
	    	PaError err;
 			if ((err = Pa_Initialize ()) != paNoError) {
        		std::stringstream s;
        		s << Pa_GetErrorText (err);
        		std::runtime_error (s.str ());
      		}
	    #endif
	}
	virtual ~Rose () {
	    #if defined (USE_PORTAUDIO)
	    	Pa_Terminate ();
	    #endif
	}
	std::vector <int> recordDevices () const {
	    std::vector <int> v;

		#if defined (USE_PORTAUDIO)	    
    	    for (int i = 0; i < Pa_GetDeviceCount (); i++) {
    	        const PaDeviceInfo* info = Pa_GetDeviceInfo (i);
    	        if (info->maxInputChannels > 0)
    	        	v.push_back (i);
    		}
		#endif
		return v;
 	}
 	std::vector <int> playbackDevices () const {
	    std::vector <int> v;
	    
	    #if defined (USE_PORTAUDIO)
    	    for (int i = 0; i < Pa_GetDeviceCount (); i++) {
    	        const PaDeviceInfo* info = Pa_GetDeviceInfo (i);
    	        if (info->maxOutputChannels > 0)
    	        	v.push_back (i);
    		}
		#endif
		return v;
  	}
  	std::string deviceName (int number) {
		std::stringstream devName;
    	#if defined (USE_PORTAUDIO)
    		int numDevices = Pa_GetDeviceCount ();
    		if (number < 0 || number > numDevices)
    			throw std::runtime_error ("invalid audio device number");
    		
       		const PaDeviceInfo *info;	
    		info = Pa_GetDeviceInfo (number);
    
            devName << "[" << number << "] " << info->name << " ("
            	<< Pa_GetHostApiInfo (info->hostApi)->name << ")";
		#endif
		return devName.str ();       
   	}
	std::string deviceInfo (int number) {
		std::stringstream devInfo;
    	#if defined (USE_PORTAUDIO)
    		int numDevices = Pa_GetDeviceCount ();
    		if (number < 0 || number > numDevices)
    			throw std::runtime_error ("invalid audio device number");
    		
       		const PaDeviceInfo *info;	
    		info = Pa_GetDeviceInfo (number);
    
    	    PaStreamParameters inputParameters, outputParameters;
            devInfo << "Device name: " << info->name << "\n";
            
    		if (info->maxInputChannels > 0) {
    			devInfo << "Host API: " << Pa_GetHostApiInfo (info->hostApi)->name 
       				<< "\n";
    			devInfo << "Max inputs: " << info->maxInputChannels << "\n";
    			devInfo << "Default low input latency: " 
       				<< info->defaultLowInputLatency << "\n";
    			devInfo << "Default high input latency: " 
       				<< info->defaultHighInputLatency << "\n";
				#if defined (USE_ASIO)
    			if (Pa_GetHostApiInfo (info->hostApi)->type == paASIO) {
    				long minLatency, maxLatency, preferredLatency, granularity;
    
    				PaAsio_GetAvailableLatencyValues (number, &minLatency, 
        				&maxLatency, &preferredLatency, &granularity);
    				devInfo << "ASIO minimum buffer size: " << 
        				minLatency << "\n";
    				devInfo << "ASIO maximum buffer size: " << 
    					maxLatency << "\n";
    				devInfo << "ASIO preferred buffer size: " << 
        				preferredLatency << "\n";
    
    				if (granularity == -1)
    					devInfo << "ASIO buffer granularity: power of 2\n";
    				else
    					devInfo << "ASIO buffer granularity: " << 
         					granularity << "\n";
    			}
				#endif
    			inputParameters.device = number;
    			inputParameters.channelCount = info->maxInputChannels;
    			inputParameters.sampleFormat = paInt16;
    			inputParameters.suggestedLatency = 0;
    			inputParameters.hostApiSpecificStreamInfo = NULL;
            	devInfo << 
             		"Supported standard sample rates\n for half-duplex 16 bit "
    				<< info->maxInputChannels << " channel input = \n";
    			std::string t = supportedStandardSampleRates (
       				&inputParameters, NULL);
   				devInfo << t;
    		}	
    		else if (info->maxOutputChannels > 0) {
    			devInfo << "Host API: " << Pa_GetHostApiInfo (info->hostApi)->name 
       				<< "\n";
    			devInfo << "Max outputs: " << info->maxOutputChannels 
       				<< "\n";
    			devInfo << "Default low input latency: " << 
       				info->defaultLowOutputLatency << "\n";
    			devInfo << "Default high input latency: " << 
       				info->defaultHighOutputLatency << "\n";
				
				#if defined (USE_ASIO)
    			if (Pa_GetHostApiInfo (info->hostApi)->type == paASIO) {
    				long minLatency, maxLatency, preferredLatency, granularity;   
    				PaAsio_GetAvailableLatencyValues (number, &minLatency, 
        				&maxLatency, &preferredLatency, &granularity);
    
    				devInfo << "ASIO minimum buffer size: " << minLatency 
        				<< "\n";
    				devInfo << "ASIO maximum buffer size: " << maxLatency 
    					<< "\n";
    				devInfo << "ASIO preferred buffer size: " << 
        				preferredLatency << "\n";
    
    				if (granularity == -1)
    					devInfo << "ASIO buffer granularity: power of 2\n";
    				else
    					devInfo << "ASIO buffer granularity: " << 
     						granularity << "\n";
    			}
    			#endif
			
    			outputParameters.device = number;
    			outputParameters.channelCount = info->maxOutputChannels;
    			outputParameters.sampleFormat = paInt16;
    			outputParameters.suggestedLatency = 0;
    			outputParameters.hostApiSpecificStreamInfo = NULL;
            	devInfo << 
             		"Supported standard sample rates\n for half-duplex 16 bit "
    				<< info->maxOutputChannels << " channel output = \n";
    			std::string t = supportedStandardSampleRates (NULL, 
       				&outputParameters);
   				devInfo << t;
           }
		#endif
		return devInfo.str ();
   	}
   	void playbackDevice (int number) {
    	m_playback = number;    
    }
	int playbackDevice () const {
    	return m_playback;    
    }
   	void recordDevice (int number) {
    	m_record = number;    
    }
	int recordDevice () const {
    	return m_record;    
    }
  	void frames (int f) {
    	m_framesPerBuffer = f; 
 	}    
  	int frames () const {
    	return m_framesPerBuffer; 
 	}
  	void buffers (int b) {
    	m_buffers = b; 
 	}    
  	int buffers () const {
    	return m_buffers; 
 	}    
   	int start (audioCallback c, int inputChannels = 0, void* userData = NULL) {
   	    int e = -1;
 		#if defined (USE_PORTAUDIO)
 			assert (c);
        	PaError err;
    	    PaStreamParameters inputParameters, outputParameters;
    
    		inputParameters.device = m_record;
    		inputParameters.channelCount = inputChannels;
    		inputParameters.sampleFormat = paFloat32;
    		inputParameters.suggestedLatency = 
    			Pa_GetDeviceInfo (inputParameters.device)->defaultLowInputLatency;
    		inputParameters.hostApiSpecificStreamInfo = NULL;
 
    		outputParameters.device = m_playback;
    		outputParameters.channelCount = m_info->channels ();
    		outputParameters.sampleFormat = paFloat32;
    		outputParameters.suggestedLatency = 
    			Pa_GetDeviceInfo (outputParameters.device)->defaultLowOutputLatency;
    		outputParameters.hostApiSpecificStreamInfo = NULL;
    	
    		PaStreamCallback* audioCallback = c;
    		if (inputChannels == 0) {
  				err = Pa_OpenStream (&m_paStream, NULL, &outputParameters, 
    				m_info->samplingRate (),  m_framesPerBuffer, 
    				paClipOff, audioCallback, userData);
   			}
      		else { 			
    			err = Pa_OpenStream (&m_paStream, &inputParameters, &outputParameters, 
    				m_info->samplingRate (),  m_framesPerBuffer, 
    				paClipOff, audioCallback, userData);
			}					
    		if  (err != paNoError) {
	   			return (int) err;
    		}
    		
    		err = Pa_StartStream (m_paStream);
    		if (err != paNoError) {
    			return (int) err;
    		}
    		
    		e = err;
		#endif

		return e;
	}
	int stop () {
	    int e = -1;
		#if defined (USE_PORTAUDIO)
			PaError err;
			err = Pa_CloseStream (m_paStream);
		
			if (err != paNoError) {
				return (int) err;
			}
			e = err;
		#endif
		
		return e;     
	}
	int latency () const {
    	return (int) ((double) (m_framesPerBuffer * 1000) / m_info->samplingRate ()); 
 	}
	double loadedCPU () const {
     	#if defined (USE_PORTAUDIO)
     		return (double) Pa_GetStreamCpuLoad (m_paStream);
     	#endif
       	return 0; 
	}
	int isRunning () {
	    #if defined (USE_PORTAUDIO)
    		return Pa_IsStreamActive (m_paStream);
		#endif
		
		return -1;
	}
	std::string decodeError (int error) {
    	std::string s;
 		#if defined (USE_PORTAUDIO)
 			s = Pa_GetErrorText ((PaError) error);
 		#endif
       	return s; 
	}
	int translateRecordNumber (unsigned int number) {
    	std::vector <int> inputs = recordDevices ();
    	
    	int n = -1;
		if (number < 0 || number >= inputs.size ())
			return n;
		else
			return inputs[number];
	}
	int translatePlaybackNumber (unsigned int number) {
    	std::vector <int> outputs = playbackDevices ();
    	
    	int n = -1;
		if (number < 0 || number >= outputs.size ())
			return n;
		else
			return outputs[number];
	}
	void update (Subject* s) {
	}
private:
    #if defined (USE_PORTAUDIO)
    	PaStream* m_paStream;
    #endif
    
    int m_playback;
    int m_record;
    int m_framesPerBuffer;
    int m_buffers;
    
    #if defined (USE_PORTAUDIO)
	std::string supportedStandardSampleRates (
 		const PaStreamParameters *inputParameters,
    	const PaStreamParameters *outputParameters) {
		double standardSampleRates[] = {
    		8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
    		44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1
		};
        std::stringstream s;

        PaError err;
        int printCount = 0;
        for (int i = 0; standardSampleRates[i] > 0; i++) {
            err = Pa_IsFormatSupported (inputParameters, outputParameters, 
            	standardSampleRates[i]);
            if (err == paFormatIsSupported) {
                if (printCount == 0) {
                    s << " " << standardSampleRates[i];
                    printCount = 1;
                }
                else if (printCount == 4) {
                    s << ",\n " << standardSampleRates[i];
                    printCount = 1;
                }
                else {
                    s << ", " << standardSampleRates[i];
                    ++printCount;
                }
            }
        }
        if (!printCount)
    		s << "\tNone\n";
        else
            s << "\n";
        
        return s.str ();
	}	
    #endif
};

#endif	// ROSE_H 

// EOF

