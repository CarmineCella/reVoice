// MyApp.cpp
//

#include <stdexcept>
#include <sstream>
#include "MyApp.h"
#include <stdexcept>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>
#include <cstddef>

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

double maxCPULoad;
bool trackPitch;

double standardSampleRates[] = {
	8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
	44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1
};

const int PRESAMPLES = 44100;

FXDEFMAP (MyApp) MyAppMap[] = {
	FXMAPFUNC (SEL_COMMAND,   (long) -1,       	MyApp::dummy),
};

#if defined (REPITCH_ONLY)
	double tempWaveform = 0;
#endif

FXIMPLEMENT (MyApp, FXApp, MyAppMap, ARRAYNUMBER (MyAppMap))

MyApp::MyApp (const FXString& name, const FXString& vendor) :
    FXApp (name, vendor) {
    _config = Configure::instance ();
    _stringTable = StringTable::instance ();

    _config->notes (_stringTable->get ("AUTHOR"));

   	_splashIcon = new FXBMPIcon (this, splash);
  	_bigIcon = new FXBMPIcon (this, iconBig, FXRGB (192, 192, 192),
 		IMAGE_ALPHACOLOR);
	_smallIcon = new FXBMPIcon (this, iconSmall, FXRGB (192, 192, 192),
 		IMAGE_ALPHACOLOR);
	_nerdIcon = new FXBMPIcon (this, nerdInside, FXRGB (255, 255, 255),
 		IMAGE_ALPHACOLOR);
	_penguinIcon = new FXBMPIcon (this, penguin, FXRGB (192, 192, 192),
 		IMAGE_ALPHACOLOR);
	_openIcon = new FXBMPIcon (this, open1, FXRGB (192, 192, 192),
 		IMAGE_ALPHACOLOR);
	_quitIcon = new FXBMPIcon (this, quit, FXRGB (192, 192, 192),
 		IMAGE_ALPHACOLOR);
    _playIcon = new FXBMPIcon (this, play, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _stopIcon = new FXBMPIcon (this, stopSquare, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _recIcon = new FXBMPIcon (this, rec, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _stopRecIcon = new FXBMPIcon (this, stopRec, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _enableIcon = new FXBMPIcon (this, enable, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _settingsIcon = new FXBMPIcon (this, settings, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _audioIcon = new FXBMPIcon (this, audio, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _liveIcon = new FXBMPIcon (this, live, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _MIDIIcon = new FXBMPIcon (this, midi, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _globalIcon = new FXBMPIcon (this, global, FXRGB (255, 255, 255),
    	IMAGE_ALPHACOLOR);
    _contentsIcon = new FXBMPIcon (this, contents, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _phiIcon = new FXBMPIcon (this, phi, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);

	// reVoice specific...
    _sineIcon = new FXBMPIcon (this, sine, FXRGB (255, 255, 255),
    	IMAGE_ALPHACOLOR);
    _squareIcon = new FXBMPIcon (this, square, FXRGB (255, 255, 255),
    	IMAGE_ALPHACOLOR);
    _harmonIcon = new FXBMPIcon (this, harmon, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _pdetIcon = new FXBMPIcon (this, pdet, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _panicIcon = new FXBMPIcon (this, panic, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _mixIcon = new FXBMPIcon (this, mix, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _voicesIcon = new FXBMPIcon (this, voices, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);
    _morphIcon = new FXBMPIcon (this, morph, FXRGB (192, 192, 192),
    	IMAGE_ALPHACOLOR);

	_config->literal ("Global", "LastDirectory", "");
	_config->number ("Global", "FirstRun", 1);
	_config->number ("Global", "MAXCPULoad", .9);
	_config->number ("Windows", "MainWindowXPos", 200);
	_config->number ("Windows", "MainWindowYPos", 200);
	_config->number ("Windows", "MainWindowWidth", 800);
	_config->number ("Windows", "MainWindowHeight", 600);
	_config->number ("Windows", "ShowSplashScreen", 1);
	_config->number ("Windows", "ToolBarVisible", 1);
	_config->number ("Windows", "StatusBarVisible", 1);
	_config->number ("Windows", "ControlPanelVisible", 1);
	_config->number ("Devices", "AudioRecording", 0);
	_config->number ("Devices", "AudioPlayback", 0);
	_config->number ("Devices", "MIDIInput", 0);
	_config->number ("Devices", "MIDIOutput", 0);
    _config->number ("Devices", "SamplingRate", 8);
    _config->number ("Devices", "EngineFrames", 32);
    _config->number ("Devices", "InputChannels", 1);
    _config->literal ("Devices", "OutputPCMFile", "test.wav");
    _config->number ("Devices", "ConnectEngineOnStartup", 0);
	_config->number ("Devices", "EnableMIDIInput", 1);
	_config->number ("Devices", "EnableMIDIThru", 0);
	_config->number ("Devices", "FilterActiveSensing", 1);
	_config->number ("Devices", "FilterSysEx", 1);
	_config->number ("Devices", "FilterClock", 1);
	_config->number ("Devices", "EnableMIDIOnStartup", 0);

	configInit ();
}

void MyApp::configInit () {
	// reVoice specific...
//	_config->number ("Harmonizer", "ActiveVoices", 3);
	_config->number ("Harmonizer", "Waveform", 3);
	_config->literal ("Harmonizer", "InputFile", "demo.wav");
	_config->number ("Harmonizer", "DelayLine", 262144);
	_config->number ("Harmonizer", "UsedDelay", 1024);
	_config->number ("Harmonizer", "Logic", 3);
	_config->number ("Harmonizer", "VibratoLogic", 1);
	_config->number ("Harmonizer", "RandomVibratoLogic", 1);
	_config->number ("Harmonizer", "PanningLogic", 1);
	_config->number ("Harmonizer", "FormantsLogic", 0);
	_config->number ("Harmonizer", "A1", .25);
	_config->number ("Harmonizer", "F1", 1);
	_config->number ("Harmonizer", "VA1", 0);
	_config->number ("Harmonizer", "VF1", 4.1);
	_config->number ("Harmonizer", "VARAND1", 0);
	_config->number ("Harmonizer", "VFRAND1", 0);
	_config->number ("Harmonizer", "P1", .5);
	_config->number ("Harmonizer", "FORM1", 1);
	_config->number ("Harmonizer", "On1", 1);
	_config->number ("Harmonizer", "HP1", 1);
	_config->number ("Harmonizer", "A2", .25);
	_config->number ("Harmonizer", "F2", 1);
	_config->number ("Harmonizer", "VA2", 0);
	_config->number ("Harmonizer", "VF2", 4.2);
	_config->number ("Harmonizer", "VARAND2", 0);
	_config->number ("Harmonizer", "VFRAND2", 0);
	_config->number ("Harmonizer", "P2", .5);
	_config->number ("Harmonizer", "FORM2", 1);
	_config->number ("Harmonizer", "On2", 1);
	_config->number ("Harmonizer", "HP2", 1);
	_config->number ("Harmonizer", "A3", .25);
	_config->number ("Harmonizer", "F3", 1);
	_config->number ("Harmonizer", "VA3", 0);
	_config->number ("Harmonizer", "VF3", 4.3);
	_config->number ("Harmonizer", "VARAND3", 0);
	_config->number ("Harmonizer", "VFRAND3", 0);
	_config->number ("Harmonizer", "P3", .5);
	_config->number ("Harmonizer", "FORM3", 1);
	_config->number ("Harmonizer", "On3", 1);
	_config->number ("Harmonizer", "HP3", 1);
	_config->number ("Harmonizer", "A4", .25);
	_config->number ("Harmonizer", "F4", 1);
	_config->number ("Harmonizer", "VA4", 0);
	_config->number ("Harmonizer", "VF4", 4.4);
	_config->number ("Harmonizer", "VARAND4", 0);
	_config->number ("Harmonizer", "VFRAND4", 0);
	_config->number ("Harmonizer", "P4", .5);
	_config->number ("Harmonizer", "FORM4", 1);
	_config->number ("Harmonizer", "On4", 1);
	_config->number ("Harmonizer", "HP4", 1);
	_config->number ("Harmonizer", "FundamentalType", 2);
	_config->number ("Harmonizer", "MIDIFreq", 440);
	_config->number ("Harmonizer", "Semitones", 24);
	_config->number ("Harmonizer", "FundamentalRatio", 1);
	_config->number ("Harmonizer", "WindowType", 4);
	_config->number ("Harmonizer", "Attack", 50);
	_config->number ("Harmonizer", "Decay", 50);
	_config->number ("Harmonizer", "AutoMode", 1);
	_config->number ("Harmonizer", "Cycles", 2);
 	_config->number ("Harmonizer", "FreqThreshold", 1); //1.059463094359);
  	_config->number ("Harmonizer", "SemitonesThresh", 13);
   	_config->number ("Harmonizer", "MasterKey", 0);
   	_config->number ("Harmonizer", "Scale", 0);
 	_config->number ("Harmonizer", "Jitter", .5);
	_config->number ("Harmonizer", "PitchDetector", 0);
	_config->number ("Harmonizer", "TrackPitch", 0);
 	_config->number ("Harmonizer", "Bypass", 0);
  	_config->number ("Harmonizer", "Freeze", 0);
 	_config->number ("Harmonizer", "AntiRumble", 1);
    _config->number ("Harmonizer", "SourceLevel", .35);
    _config->number ("Harmonizer", "HarmonyLevel", 1);
    _config->number ("Harmonizer", "EffectsLevel", .15);
    _config->number ("Harmonizer", "T60", 2.5);
    _config->number ("Harmonizer", "OutputMultiplier", 1);

	// pdet here...
  	_config->number ("Harmonizer", "Feedback", .3);
	_config->number ("Harmonizer", "RangeStart", 77.782);
  	_config->number ("Harmonizer", "RangeEnd", 1396.913);
	_config->number ("Harmonizer", "FilterQuality", .707);
	_config->number ("Harmonizer", "Split", 30);
 	_config->number ("Harmonizer", "AmpThreshold", .03);
// 	_config->number ("Harmonizer", "ZCCycles", 2);
  	_config->number ("Harmonizer", "Levels", 6);
//	_config->number ("Harmonizer", "UseBiquad", 0);
 	_config->number ("Harmonizer", "SmoothingQuality", .707);
 	_config->number ("Harmonizer", "SmoothingRate", 2500);
 	// bandpass
 	_config->number ("Harmonizer", "RangeStart1", 77.782);
  	_config->number ("Harmonizer", "RangeEnd1", 1396.913);
	_config->number ("Harmonizer", "FilterQuality1", .707);
 	_config->number ("Harmonizer", "AmpThreshold1", 0.03);
 	//_config->number ("Harmonizer", "Speed1", 2); // not used!
  	_config->number ("Harmonizer", "RangeMin1", 77.789);
  	_config->number ("Harmonizer", "RangeMax1", 1396.913);
  	_config->number ("Harmonizer", "Levels1", 2);
 	_config->number ("Harmonizer", "SmoothingQuality1", .707);
 	_config->number ("Harmonizer", "SmoothingRate1", 2500);
 	// spectral
	_config->number ("Harmonizer", "FFTSize", 1); // index pos
	_config->number ("Harmonizer", "RangeStart2", 77.782);
  	_config->number ("Harmonizer", "RangeEnd2", 1396.913);
	_config->number ("Harmonizer", "FilterQuality2", .707);
	_config->number ("Harmonizer", "Levels2", 2);
 	_config->number ("Harmonizer", "AmpThreshold2", 0.03);
 	_config->number ("Harmonizer", "Split2", 30);
 	_config->number ("Harmonizer", "SmoothingQuality2", .707);
 	_config->number ("Harmonizer", "SmoothingRate2", 2500);

 	// morphizer
 	_config->number ("Morphizer", "Center1", 80); // density
    _config->number ("Morphizer", "Random1", 0);
 	_config->number ("Morphizer", "Center2", .1); // duration
    _config->number ("Morphizer", "Random2", 0);
	_config->number ("Morphizer", "Center3", 1);  // frequency
	_config->number ("Morphizer", "Random3", 0);
	_config->number ("Morphizer", "Center4", 1);  // glissato
	_config->number ("Morphizer", "Random4", 0);
	_config->number ("Morphizer", "WindowType", 2);
	_config->number ("Morphizer", "Attack", 50);
	_config->number ("Morphizer", "Decay", 50);
	_config->number ("Morphizer", "SpatialJitter", .5);
	_config->number ("Morphizer", "Speed", 1);
	_config->number ("Morphizer", "PhaseJitter", 1000);
	_config->number ("Morphizer", "ReverseGrain", 0);
	_config->number ("Morphizer", "Grainlet", 0);
    _config->number ("Morphizer", "OutputLevel", 0);
}

void MyApp::init (int& argc, char** argv, FXbool connect) {
	FXApp::init (argc, argv, connect);
   	_config->deserialize (reg ());
   	maxCPULoad = _config->number ("Global", "MAXCPULoad");
   	_outputMultiplier = _config->number ("Harmonizer", "OutputMultiplier");

    // reVoice specific (audio)...
    _info = SoundInfo::instance ();
    _info->samplingRate (
    	standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
    _info->channels (2); // stereo output (hardcoded)

    _rose = new Rose (_info);
    if (!_rose) {
    	throw std::runtime_error ("can't create the sound engine object");
    }
    _remi = new Remi ();

    _urand = new RangedRandomNumberGenerator (_info, 0, .25);
    _urand->seed (time (NULL) - 1);
    _rrand = new RangedRandomNumberGenerator (_info, .25, 4);
	_rrand->seed (time (NULL) + 2);
    _urand2 = new RangedRandomNumberGenerator (_info, 0, 8);
    _urand2->seed (time (NULL) - 2);
    _rrand2 = new RangedRandomNumberGenerator (_info, 0, 5);
	_rrand2->seed (time (NULL) + 3);
	_urand3 = new RangedRandomNumberGenerator (_info, 0, 15);
	_urand3->seed (time (NULL) - 4);
	_rrand3 = new RangedRandomNumberGenerator (_info, 0, 1);
	_rrand3->seed (time (NULL) + 5);
	_rrand4 = new RangedRandomNumberGenerator (_info, .5, 2);
	_rrand4->seed (time (NULL) - 6);

    _left = new EnvelopeFollower (_info, .99999);
	_right = new EnvelopeFollower (_info, .99999);

	_recording = false;
	_useMIDIFreq = false;
	_useMIDIStream = false;
	_live = false;
	trackPitch = (bool) _config->number ("Harmonizer", "TrackPitch");

	_reverbLeft = new NRev (_config->number ("Harmonizer", "T60"));
	_reverbLeft->setEffectMix (_config->number ("Harmonizer", "EffectsLevel"));
	_reverbRight = new NRev (_config->number ("Harmonizer", "T60"));
	_reverbRight->setEffectMix (_config->number ("Harmonizer", "EffectsLevel"));

#if defined (REPITCH_ONLY)
	tempWaveform = _config->number ("Harmonizer", "Waveform");
	_config->number ("Harmonizer", "Waveform", 2);
#endif
	switch ((int) _config->number ("Harmonizer", "Waveform")) {
	    case 0: {
			_table = new SignalTable ((long)
   				standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
			SignalTable t0 ((long) standardSampleRates[(int)
   				_config->number ("Devices", "SamplingRate")]);
			t0.gen (1, 1);
			LoopOscillator osc (_info, &t0);
			osc.frequency (440);
			for (int i = 0; i < _table->size (); i++) {
        		_table->set (i, osc.tick ());
      		}
    	}
		break;
     	case 1: {
     	   	_table = new SignalTable ((long)
          		standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
			SignalTable t0 ((long)
   				standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
			t0.square (20);
			LoopOscillator osc (_info, &t0);
			osc.frequency (440);
			for (int i = 0; i < _table->size (); i++) {
        		_table->set (i, osc.tick ());
      		}
    	}
 	    break;
 	    case 2: {
 	        try {
            	WavInFile inFile (
                	_config->literal ("Harmonizer", "InputFile").c_str ());
                int bits = inFile.getNumBits ();
                double sampleRate = inFile.getSampleRate ();
                int channels = inFile.getNumChannels ();
                if (channels != 1 || sampleRate != 44100 || bits != 16) {
                    throw std::runtime_error ("invalid input format");
                }
                long p = inFile.getNumSamples ();
           		_table = new SignalTable (p);
                long n = 0;

                while (!inFile.eof()) {
                    short s;
                    inFile.read (&s, sizeof (short));
                    double d = (double) s;
                    d /= 32768;
                    _table->set (n, (double) d);
                    n++;
                }
           	}
           	catch (std::runtime_error& e) {
    	   	    _table = new SignalTable (TABLEN);
				_table->clear ();
            }
 	    }
      	break;
      	case 3:
      		if (_config->number ("Harmonizer",
    			"DelayLine") <= 0)
    			_config->number ("Harmonizer",
    			"DelayLine", 1024);

    		_table = new SignalTable ((long) _config->number ("Harmonizer", "DelayLine"));
    		double r = _config->number ("Harmonizer",
    			"UsedDelay") / _config->number ("Harmonizer",
    			"DelayLine");
    		_table->delay (r);
			_live = true;
  	    break;
	}

//	for (int i = 0; i < PARTIALS; i++) {
		_period = new PeriodEstimator (
     		_info, _config->number ("Harmonizer", "RangeStart"),
       		_config->number ("Harmonizer", "RangeEnd"),
         	(int) _config->number ("Harmonizer", "Levels"),
          	(int) _config->number ("Harmonizer", "Split"),
           	_config->number ("Harmonizer", "AmpThreshold"));
//       	_periods[i]->cycles ((int)
//       		_config->number ("Harmonizer", "ZCCycles"));
      	_period->filterQuality (
      		_config->number ("Harmonizer", "FilterQuality"));
      	_period->feedback (
      		_config->number ("Harmonizer", "Feedback"));
      	_period->smoothingQuality (
      		_config->number ("Harmonizer", "SmoothingQuality"));
      	_period->smoothingRate (
      		_config->number ("Harmonizer", "SmoothingRate"));
//  		double middle = (_period->max () - _period->min ()) / 2;
//  		for (int i = 0; i < PRESAMPLES; i++) {
//        	_period->process (middle);
//        }

     	_autoTune = new AdaptiveTuner (
     		_info, _config->number ("Harmonizer", "RangeStart1"),
       		_config->number ("Harmonizer", "RangeEnd1"),
         	(int) _config->number ("Harmonizer", "Levels1"),
          	_config->number ("Harmonizer", "AmpThreshold1"));
      	_autoTune->filterQuality (
      		_config->number ("Harmonizer", "FilterQuality1"));
      	_autoTune->smoothingQuality (
      		_config->number ("Harmonizer", "SmoothingQuality1"));
      	_autoTune->smoothingRate (
      		_config->number ("Harmonizer", "SmoothingRate1"));
//      	_autoTune->speed (
//      		_config->number ("Harmonizer", "Speed1"));
//  		middle = (_autoTune->max () - _autoTune->min ()) / 2;
//  		for (int i = 0; i < PRESAMPLES; i++) {
//        	_autoTune->process (middle);
//        }

		int size = (int) pow (2,
			(int) _config->number ("Harmonizer", "FFTSize") + 7);
     	_spectral = new SpectralEstimator (
     		_info,
     		(int) size,
     		_config->number ("Harmonizer", "RangeStart2"),
       		_config->number ("Harmonizer", "RangeEnd2"),
       		(int) _config->number ("Harmonizer", "Levels2"),
         	_config->number ("Harmonizer", "Split2"),
          	_config->number ("Harmonizer", "AmpThreshold2"),
			_config->number ("Harmonizer", "SmoothingRate2"));

      	_spectral->filterQuality (
      		_config->number ("Harmonizer", "FilterQuality2"));
      	_spectral->smoothingQuality (
      		_config->number ("Harmonizer", "SmoothingQuality2"));

		if (_config->number ("Harmonizer", "PitchDetector") == 0) {
			//_pdet.push_back (_periods[i]);
			_pdet = _period;
		}
		else if (_config->number ("Harmonizer", "PitchDetector") == 1)  {
			//_pdets.push_back (_autoTunes[i]);
			_pdet = _autoTune;
      	}
      	else if (_config->number ("Harmonizer", "PitchDetector") == 2) {
      		_pdet = _spectral;
      	}
//	}

#if defined (REPITCH_ONLY)
	_pdet = _period;
#endif

 	_harmon = new Harmonizer (_info, _table, _pdet, 0, ALWAYS);
    FXString temp1, temp2;
	for (int i = 0; i < PARTIALS; i++) {
        temp1.format ("On%d", i + 1);
        temp2.format ("HP%d", i + 1);
        _harmon->status (i,
    		(bool) _config->number ("Harmonizer", temp1.text ()));
		_harmon->highpass (i,
     		(bool) _config->number ("Harmonizer", temp2.text ()));
    }
   	for (int i = 0; i < PARTIALS; i++) {
 		temp1.format ("A%d", i + 1);
        temp2.format ("F%d", i + 1);
		_harmon->ratios (i,
    		_config->number ("Harmonizer", temp1.text ()),
     		_config->number ("Harmonizer", temp2.text ()));
   	}
    switch ((int) _config->number ("Harmonizer", "Logic")) {
    	case 0:
         	_harmon->logic (Pandora::CUSTOM_CHORD);
        break;
        case 1:
           	_harmon->logic (Pandora::DOMINANT_SEVENTH);
        break;
    	case 2:
    		_harmon->logic (Pandora::MINOR_SEVENTH);
        break;
        case 3:
           	_harmon->logic (Pandora::MAJOR_SEVENTH);
        break;
    	case 4:
    		_harmon->logic (Pandora::DIMINISHED_SEVENTH);
        break;
    	case 5:
    		_harmon->logic (Pandora::AUGMENTED_SEVENTH);
        break;
    	case 6:
    		_harmon->logic (Pandora::QUARTAL_CHORD);
        break;
    	case 7:
    		_harmon->logic (Pandora::SCALIC);
        break;
    	case 8:
    		_harmon->logic (Pandora::INTELLICHORD);
        break;
        case 9:
			_harmon->logic (Pandora::CUSTOM_CHORD);
        	_useMIDIStream = true;
        break;
    	case 10:
    		_harmon->logic (Pandora::MINOR_WITH_SIXTH);
        break;
    	case 11:
    		_harmon->logic (Pandora::SINGLECHORD);
        break;
 	}
	for (int i = 0; i < PARTIALS; i++) {
        temp1.format ("VA%d", i + 1);
        temp2.format ("VF%d", i + 1);
		_harmon->vibrato (i,
    		_config->number ("Harmonizer", temp1.text ()),
     		_config->number ("Harmonizer", temp2.text ()));
   	}
	switch ((int) _config->number ("Harmonizer", "VibratoLogic")) {
    	case 0:
       		_harmon->style (Pandora::CUSTOM_VIBRATO);
        break;
        case 1:
            _harmon->style (Pandora::SMALL_VIBRATO);
    	break;
    	case 2:
    	    _harmon->style (Pandora::MEDIUM_VIBRATO);
	    break;
	    case 3:
	        _harmon->style (Pandora::LARGE_VIBRATO);
     	break;
 	}
	for (int i = 0; i < PARTIALS; i++) {
        temp1.format ("VARAND%d", i + 1);
        temp2.format ("VFRAND%d", i + 1);
		_harmon->randomVibrato (i,
    		/* _config->number ("Harmonizer", temp1.text ()), */
     		_config->number ("Harmonizer", temp2.text ()));
   	}
	switch ((int) _config->number ("Harmonizer", "RandomVibratoLogic")) {
    	case 0:
       		_harmon->randomStyle (Pandora::CUSTOM_RANDOM);
        break;
        case 1:
            _harmon->randomStyle (Pandora::SMALL_RANDOM);
    	break;
    	case 2:
    	    _harmon->randomStyle (Pandora::MEDIUM_RANDOM);
	    break;
	    case 3:
	        _harmon->randomStyle (Pandora::LARGE_RANDOM);
     	break;
 	}
	for (int i = 0; i < PARTIALS; i++) {
	    temp1.format ("P%d", i + 1);
		_harmon->panning (i,
			_config->number ("Harmonizer", temp1.text ()));
	}
	switch ((int) _config->number ("Harmonizer", "PanningLogic")) {
    	case 0:
         	_harmon->disposition (Pandora::CUSTOM_POSITION);
     	break;
     	case 1:
     	    _harmon->disposition (Pandora::TWO_TWO);
 	    break;
 	    case 2:
       	    _harmon->disposition (Pandora::ONE_THREE);
      	break;
      	case 3:
		    _harmon->disposition (Pandora::THREE_ONE);
  	    break;
  	}
	for (int i = 0; i < PARTIALS; i++) {
	    temp1.format ("FORM%d", i + 1);
		_harmon->formant (i,
			_config->number ("Harmonizer", temp1.text ()));
	}
	switch ((int) _config->number ("Harmonizer", "FormantsLogic")) {
    	case 0:
         	_harmon->choral (Pandora::CUSTOM_CHORAL);
     	break;
     	case 1:
     	    _harmon->choral (Pandora::MIXED_CHORAL);
 	    break;
 	    case 2:
       	    _harmon->choral (Pandora::FEMALE_CHORAL);
      	break;
      	case 3:
		    _harmon->choral (Pandora::MALE_CHORAL);
  	    break;
  	}
	switch ((int) _config->number ("Harmonizer", "FundamentalType")) {
    	case 0:
			_useMIDIFreq = true;
			_harmon->fundamental (1);
        break;
        case 1: {
			int t = (int) _config->number ("Harmonizer", "Semitones");
			t -= 24;
            if (t < 0) {
            	t = abs (t);
        		double r = 1 / pow (TUNINGSEMITONE, (double) t);
            	_harmon->fundamental (r);
            }
            else if (t == 0) {
            	_harmon->fundamental (1);
            }
            else {
        		double r = pow (TUNINGSEMITONE, (double) t);
            	_harmon->fundamental (r);
            }
        }
    	break;
        case 2:
        	_harmon->fundamental (
         		_config->number ("Harmonizer", "FundamentalRatio"));
        break;
    }

    switch ((int) _config->number ("Harmonizer", "WindowType")) {
        case 0:
    		_harmon->grainEnvelope (Pandora::RECTANGLE);
    	break;
        case 1:
    		_harmon->grainEnvelope (Pandora::BARTLETT);
    	break;
        case 2:
    		_harmon->grainEnvelope (Pandora::GAUSSIAN);
    	break;
        case 3:
    		_harmon->grainEnvelope (Pandora::HAMMING);
    	break;
        case 4:
    		_harmon->grainEnvelope (Pandora::HANNING);
    	break;
        case 5:
    		_harmon->grainEnvelope (Pandora::BLACKMAN);
    	break;
        case 6:
    		_harmon->grainEnvelope (Pandora::BLACKMAN_HARRIS);
    	break;
	}
	_harmon->attackDecay (
		_config->number ("Harmonizer", "Attack") / 100,
		_config->number ("Harmonizer", "Decay") / 100);
    _harmon->autoMode ((bool) _config->number ("Harmonizer", "AutoMode"));
	_harmon->cyclesPerGrain ((int) _config->number ("Harmonizer", "Cycles"));

    switch ((int) _config->number ("Harmonizer", "SemitonesThresh")) {
        case 0:
		   	_harmon->freqThreshold (1);
    	break;
        case 1:
		   	_harmon->freqThreshold (1.0595);
    	break;
        case 2:
		   	_harmon->freqThreshold (1.1225);
    	break;
        case 3:
		   	_harmon->freqThreshold (1.1892);
    	break;
        case 4:
		   	_harmon->freqThreshold (1.2599);
    	break;
        case 5:
		   	_harmon->freqThreshold (1.3348);
    	break;
        case 6:
		   	_harmon->freqThreshold (1.4142);
    	break;
        case 7:
		   	_harmon->freqThreshold (1.4983);
    	break;
        case 8:
		   	_harmon->freqThreshold (1.5874);
    	break;
        case 9:
		   	_harmon->freqThreshold (1.6818);
    	break;
        case 10:
		   	_harmon->freqThreshold (1.7818);
    	break;
        case 11:
		   	_harmon->freqThreshold (1.8897);
    	break;
        case 12:
		   	_harmon->freqThreshold (2);
    	break;
    	case 13:
    	   _harmon->freqThreshold (
        		_config->number ("Harmonizer", "FreqThreshold"));
	    break;
	}
 	_harmon->masterKey ((Pandora::notes) (int)_config->number ("Harmonizer", "MasterKey"));
	_harmon->scale ((Pandora::scales) (int) _config->number ("Harmonizer", "Scale"));

	_harmon->spatialPosition (_config->number ("Harmonizer", "Jitter"));
 	_harmon->antiRumble ((bool) _config->number ("Harmonizer", "AntiRumble"));
	_harmon->bypass ((bool) _config->number ("Harmonizer", "Bypass"));
	_harmon->sourceLevel (_config->number ("Harmonizer", "SourceLevel"));
	_harmon->harmonyLevel (_config->number ("Harmonizer", "HarmonyLevel"));
 	_harmon->freeze (_config->number ("Harmonizer", "Freeze"));

#if defined (REPITCH_ONLY)
	_useMIDIStream = false;
 	_harmon->autoMode (true);

    _harmon->status (0, (bool) true, (bool) false);
    _harmon->status (1, (bool) false, (bool) false);
    _harmon->status (2, (bool) false, (bool) false);
    _harmon->status (3, (bool) false, (bool) false);

    _harmom->ratio (0, 1, 1);
 	_harmon->logic (Pandora::CUSTOM_CHORD);

 	_harmon->vibrato (0, 0, 1000);
 	_harmon->style (Pandora::CUSTOM_VIBRATO);

 	_harmon->randomVibrato (0, /* 0, */ 0);
 	_harmon->randomStyle (Pandora::CUSTOM_RANDOM);

 	_harmon->panning (0, .5);
 	_harmon->disposition (Pandora::CUSTOM_POSITION);

 	_harmon->bypass (false);
 	_harmon->sourceLevel (0);
 	_harmon->harmonyLevel (1);

 	trackPitch = false;
 	_reverbLeft->setEffectMix (0);
 	_reverbRight->setEffectMix (0);
#endif

	// morphizer
	_morphizerLevel = _config->number ("Morphizer", "OutputLevel");

	_morph = new TableGranulator (_info, _table, 0, ALWAYS);
	_morph->density (_config->number ("Morphizer", "Center1"),
		_config->number ("Morphizer", "Random1"));
	if ((int) _config->number ("Morphizer", "Grainlet") == 1) {
		_morph->generation (Pandora::GRAINLET);
	}
	else {
		_morph->generation (Pandora::ASYNC);
	}
	_morph->grainDuration ((double) _config->number ("Morphizer", "Center2"),
		_config->number ("Morphizer", "Random2"),
		_config->number ("Morphizer", "Attack") / 100,
		_config->number ("Morphizer", "Decay") / 100);
    switch ((int) _config->number ("Morphizer", "WindowType")) {
        case 0:
    		_morph->grainEnvelope (Pandora::RECTANGLE);
    	break;
        case 1:
    		_morph->grainEnvelope (Pandora::BARTLETT);
    	break;
        case 2:
    		_morph->grainEnvelope (Pandora::GAUSSIAN);
    	break;
        case 3:
    		_morph->grainEnvelope (Pandora::HAMMING);
    	break;
        case 4:
    		_morph->grainEnvelope (Pandora::HANNING);
    	break;
        case 5:
    		_morph->grainEnvelope (Pandora::BLACKMAN);
    	break;
        case 6:
    		_morph->grainEnvelope (Pandora::BLACKMAN_HARRIS);
    	break;
	}
	double fn = _table->naturalFreq (_info->samplingRate ());
	_morph->frequency (fn * _config->number ("Morphizer", "Center3"),
		_config->number ("Morphizer", "Random3"),
		fn * _config->number ("Morphizer", "Center4"),
		_config->number ("Morphizer", "Random4"));
	_morph->speedFactor (
		_config->number ("Morphizer", "Speed"),
		(long) _config->number ("Morphizer", "PhaseJitter"));
	_morph->grainAmplitude (.99);
	_morph->setSelection (0, 1);
	_morph->spatialPosition (_config->number ("Morphizer", "SpatialJitter"));
	_morph->useJitter (true);
   	if (_config->number ("Morphizer", "ReverseGrain") == 0) {
		_morph->grainDirection (1);
   	}
   	else {
	   	_morph->grainDirection (-1);
   	}

   	if (_live == true) {
   		_morph->speedFactor (1,
   			(long) _config->number ("Morphizer", "PhaseJitter"));
		_morph->grainDirection (1);
	}
}

void MyApp::exit (FXint code) {
	if (_recording == true) {
	    _recording = false;
    	delete _outFile;
	}
  	if (_rose->isRunning ()) {
    	_rose->stop ();
   	}
  	if (_remi->isRunning ()) {
    	_remi->stop ();
	}
    _config->number ("Global", "FirstRun", 0);
 	_config->serialize (reg ());

 	if (1 == code) { // reset and quit...
 		_config->clearAll (reg ());
 	}
	FXApp::exit (code);
}

long MyApp::dummy (FXObject* sender, FXSelector sel, void*) {
	return 0;
}

MyApp::~MyApp () {
#if defined (REPITCH_ONLY)
	_config->number ("Harmonizer", "Waveform", tempWaveform);
#endif
    delete _splashIcon;
    delete _bigIcon;
	delete _smallIcon;
	delete _nerdIcon;
	delete _penguinIcon;
	delete _openIcon;
	delete _quitIcon;
	delete _playIcon;
	delete _stopIcon;
	delete _recIcon;
	delete _stopRecIcon;

	delete _enableIcon;
	delete _settingsIcon;
	delete _audioIcon;
	delete _liveIcon;
	delete _MIDIIcon;
	delete _globalIcon;
	delete _contentsIcon;

	// reVoice specific...
	delete _sineIcon;
	delete _squareIcon;
	delete _harmonIcon;
	delete _pdetIcon;
	delete _panicIcon;
	delete _mixIcon;
	delete _voicesIcon;
	delete _phiIcon;
	delete _morphIcon;

 	delete _harmon;
 	delete _table;
    delete _rose;
    delete _remi;
    delete _left;
    delete _right;
    delete _period;
    delete _autoTune;
    delete _spectral;
    delete _urand;
    delete _rrand;
    delete _urand2;
    delete _rrand2;
    delete _urand3;
    delete _rrand3;
    delete _rrand4;

    delete _morph;

    delete _reverbLeft;
    delete _reverbRight;
}

// EOF

