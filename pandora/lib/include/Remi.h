// Remi.h
// 

#ifndef REMI_H
#define REMI_H 

#define USE_PORTMIDI

#if defined (USE_PORTMIDI)
	#include "porttime.h"
	#include "portmidi.h"
	
 	#define PM_INPUT_BUFFER_SIZE 512
	#define PM_OUTPUT_BUFFER_SIZE 0
#endif
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cassert>
        
#if defined (USE_PORTMIDI)
	typedef void (*midiCallback) (PtTimestamp timestamp, void *userData);
#else
	typedef void (*midiCallback) (void);
#endif
		
//! Realtime MIDI (Re.MI.)
/*! 
	This class enables the live MIDI I/O and is based
	on a portable MIDI framework (PortMidi).
*/
class Remi {
private:
	Remi& operator= (Remi&);
	Remi (const Remi&);
public:
	Remi ()	: m_output (0), m_input (0), m_running (false), m_thru (true) {
	    #if defined (USE_PORTMIDI)
		    m_pmOutStream = 0;
      		m_pmInStream = 0; 
 			Pm_Initialize ();
 			
	    #endif
	}
	virtual ~Remi () {
	    #if defined (USE_PORTMIDI)
	    	Pm_Terminate ();
 	    #endif
	}
	std::vector <int> inputDevices () const {
	    std::vector <int> v;

		#if defined (USE_PORTMIDI)	    
    	    for (int i = 0; i < Pm_CountDevices (); i++) {
    	        const PmDeviceInfo* info = Pm_GetDeviceInfo (i);
    	        if (info->input == true)
    	        	v.push_back (i);
    		}
		#endif
		return v;
 	}
 	std::vector <int> outputDevices () const {
	    std::vector <int> v;
	    
		#if defined (USE_PORTMIDI)	    
    	    for (int i = 0; i < Pm_CountDevices (); i++) {
    	        const PmDeviceInfo* info = Pm_GetDeviceInfo (i);
    	        if (info->output == true)
    	        	v.push_back (i);
    		}
		#endif
		return v;
  	}
	std::string deviceName  (int number) {
		std::stringstream devName;
    	#if defined (USE_PORTMIDI)
        	int numDevices = Pm_CountDevices ();
    		if (number < 0 || number > numDevices)
    			throw std::runtime_error ("invalid MIDI device number");

   		    const PmDeviceInfo* info = Pm_GetDeviceInfo (number);
			devName << "[" << number << "] " << info->name << " ("
   				<< info->interf << ")";        		
		#endif
		return devName.str ();
   	}
	std::string deviceInfo (int number) {
		std::stringstream devInfo;
    	#if defined (USE_PORTMIDI)
        	int numDevices = Pm_CountDevices ();
    		if (number < 0 || number > numDevices)
    			throw std::runtime_error ("invalid MIDI device number");

   		    const PmDeviceInfo* info = Pm_GetDeviceInfo (number);
			devInfo << "Name: " << info->name << "\n";        		
   			devInfo << "Host API: " << info->interf << "\n";
   			devInfo << "Opened: " << (info->opened ? "yes\n" : "no\n");
		#endif
		return devInfo.str ();
   	}
   	void outputDevice (int number) {
    	m_output = number;    
    }
	int outputDevice () const {
    	return m_output;    
    }
   	void inputDevice (int number) {
    	m_input = number;    
    }
	int inputDevice () const {
    	return m_input;    
    }
   	int start (midiCallback c = 0, void* userData = NULL) {
   	    int e = -1;
 		#if defined (USE_PORTMIDI)
    		PmError err;
    		err = Pm_OpenOutput (&m_pmOutStream, m_output, NULL,
    			PM_OUTPUT_BUFFER_SIZE, NULL, NULL, 0);
    		if (err != pmNoError) {
    			return err;
    		}
                
    		if (c != 0) {
           		Pt_Start (1, c, userData);
        		err = Pm_OpenInput (&m_pmInStream, m_input, NULL,
        			PM_INPUT_BUFFER_SIZE, NULL, NULL);
        		if (err != pmNoError) {
        			return err;
        		}
        		
//        		err = Pm_SetFilter (m_pmInStream, PM_FILT_ACTIVE | 
//        			PM_FILT_SYSEX | PM_FILT_CLOCK);
//        		if (err != pmNoError) {
//        			return err;
//        		}
    		}  		
      		e = (int) err;
        	m_running = true;			
		#endif

		return e;
	}
	int stop () {
	    int e = -1;
		#if defined (USE_PORTMIDI)
			PmError err;
			if (m_pmInStream) {
        		Pt_Stop ();	
        		err = Pm_Close (m_pmInStream);
        		if (err != pmNoError) {
        			return err;
        		}	
        		m_pmInStream = 0;
    		}  		
    		
    		err = Pm_Close (m_pmOutStream);
    		if (err != pmNoError) {
    			return err;
    		}
    		m_pmOutStream = 0;
    		
			e = (int) err;
			m_running = false;
		#endif
		
		return e;     
	}
	int read (void* buffer, int len) {
    	int e = -1;
    	#if defined (USE_PORTMIDI)
    		e = Pm_Read (m_pmInStream, (PmEvent*) buffer, len);
       	#endif
          
       	if (m_thru) {
       		write (buffer, len);
     	}  		
      	return e; 
 	}
	int write (void* buffer, int len) {
    	int e = -1;
    	#if defined (USE_PORTMIDI)
    		e = Pm_Write (m_pmOutStream, (PmEvent*) buffer, len);
       	#endif  
      	return e; 
 	}
	int poll () {
	    int p = -1;
     	#if defined (USE_PORTMIDI)
     		p = (int) Pm_Poll (m_pmInStream);
      	#endif 
    	return (int) p;
 	}
	int isRunning () {
		return (int) m_running;
	}
	std::string decodeError (int error) {
    	std::string s;
 		#if defined (USE_PORTMIDI)
 			s = Pm_GetErrorText ((PmError) error);
 		#endif
       	return s; 
	}
	int setFilters (int filters) {
	    int err = -1;
 		#if defined (USE_PORTMIDI)
			err = (int) Pm_SetFilter (m_pmInStream, filters);
		#endif
	    return err;
	}
	int translateInputNumber (unsigned int number) {
    	std::vector <int> inputs = inputDevices ();
    	
    	int n = -1;
		if (number < 0 || number >= inputs.size ())
			return n;
		else
			return inputs[number];
	}
	int translateOutputNumber (unsigned int number) {
    	std::vector <int> outputs = outputDevices ();
    	
    	int n = -1;
		if (number < 0 || number >= outputs.size ())
			return n;
		else
			return outputs[number];
	}
	void enableThru (bool t) { m_thru = t; }
	std::string buildString (long status, long data1, long data2 = 0) {
	    std::stringstream msg;
		msg << "[" << status << " " << data1 << " " << data2 << "]";	    
   		switch (status) {
			case 0x80:
			case 0x81:
			case 0x82:
			case 0x83:
			case 0x84:
			case 0x85:
			case 0x86:
			case 0x87:
			case 0x88:
			case 0x89:
			case 0x8A:
			case 0x8B:
			case 0x8C:
			case 0x8D:
			case 0x8E:
			case 0x8F:
				msg << ": Note off";
			break;
	
			case 0x90:
			case 0x91:
			case 0x92:
			case 0x93:
			case 0x94:
			case 0x95:
			case 0x96:
			case 0x97:
			case 0x98:
			case 0x99:
			case 0x9A:
			case 0x9B:
			case 0x9C:
			case 0x9D:
			case 0x9E:
			case 0x9F:
				if (data2 != 0) {
					msg << ": Note on";
				}
				else {
					msg << ": Note off";
				}
			break;
			
			case 0xA0:
			case 0xA1:
			case 0xA2:
			case 0xA3:
			case 0xA4:
			case 0xA5:
			case 0xA6:
			case 0xA7:
			case 0xA8:
			case 0xA9:
			case 0xAA:
			case 0xAB:
			case 0xAC:
			case 0xAD:
			case 0xAE:
			case 0xAF:
				msg << ": Key aftertouch";
			break;
	
			case 0xB0:
			case 0xB1:
			case 0xB2:
			case 0xB3:
			case 0xB4:
			case 0xB5:
			case 0xB6:
			case 0xB7:
			case 0xB8:
			case 0xB9:
			case 0xBA:
			case 0xBB:
			case 0xBC:
			case 0xBD:
			case 0xBE:
			case 0xBF:
				msg << ": Control change";
			break;
	
			case 0xC0:
			case 0xC1:
			case 0xC2:
			case 0xC3:
			case 0xC4:
			case 0xC5:
			case 0xC6:
			case 0xC7:
			case 0xC8:
			case 0xC9:
			case 0xCA:
			case 0xCB:
			case 0xCC:
			case 0xCD:
			case 0xCE:
			case 0xCF:
				msg << ": Program change";
			break;
	
			case 0xD0:
			case 0xD1:
			case 0xD2:
			case 0xD3:
			case 0xD4:
			case 0xD5:
			case 0xD6:
			case 0xD7:
			case 0xD8:
			case 0xD9:
			case 0xDA:
			case 0xDB:
			case 0xDC:
			case 0xDD:
			case 0xDE:
			case 0xDF:
				msg << ": Channel aftertouch";
			break;
	
			case 0xE0:
			case 0xE1:
			case 0xE2:
			case 0xE3:
			case 0xE4:
			case 0xE5:
			case 0xE6:
			case 0xE7:
			case 0xE8:
			case 0xE9:
			case 0xEA:
			case 0xEB:
			case 0xEC:
			case 0xED:
			case 0xEE:
			case 0xEF:
				msg << ": Pitch bend";
			break;
	
			case 0xF8:
				msg << ": Timing clock";
			break;
	
			case 0xFE:
				msg << ": Active sensing";
			break;
			
			default:
				msg << ": Unknown message";
			break;
		}
		
		return msg.str ();
	}
public:
    #if defined (USE_PORTMIDI)
   		PmStream* m_pmOutStream; 
    	PmStream* m_pmInStream;
    #endif
private:    
    int m_output;
    int m_input;
    bool m_running;
    bool m_thru;
};

#endif	// REMI_H 

// EOF

