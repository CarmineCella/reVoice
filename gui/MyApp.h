// MyApp.h
//

#ifndef MYAPP_H
#define MYAPP_H

#include "NRev.h"
#include "Rose.h"
#include "Remi.h"
//#include "proton.h"
#include "pandora.h"
#include "Configure.h"
#include "StringTable.h"
#include "graphics.h"
#include <fx.h>

#define PARTIALS 4

class MyApp : public FXApp {
    FXDECLARE (MyApp)
public:
    FXBMPIcon* _splashIcon;
	FXBMPIcon* _smallIcon;	// resources are public
	FXBMPIcon* _bigIcon;
	FXBMPIcon* _nerdIcon;
	FXBMPIcon* _penguinIcon;
	FXBMPIcon* _openIcon;
	FXBMPIcon* _quitIcon;
	FXBMPIcon* _playIcon;
	FXBMPIcon* _stopIcon;
	FXBMPIcon* _recIcon;
	FXBMPIcon* _stopRecIcon;
	FXBMPIcon* _enableIcon;
	FXBMPIcon* _settingsIcon;
	FXBMPIcon* _audioIcon;
	FXBMPIcon* _liveIcon;
 	FXBMPIcon* _MIDIIcon;
    FXBMPIcon* _globalIcon;
	FXBMPIcon* _contentsIcon;
	FXBMPIcon* _panicIcon;
	FXBMPIcon* _phiIcon;
	FXBMPIcon* _mixIcon;
	FXBMPIcon* _voicesIcon;
	FXBMPIcon* _morphIcon;

 	Configure* _config;

  	// reVoice specific...
  	SoundInfo* _info;
	Rose* _rose;
	Remi* _remi;
 	EnvelopeFollower* _left;
	EnvelopeFollower* _right;
 	WavOutFile* _outFile;
	bool _recording;
	bool _useMIDIFreq;
	bool _useMIDIStream;
	bool _live;
 	double _outputMultiplier;

 	// Morphizer
 	TableGranulator* _morph;
 	double _morphizerLevel;

    Harmonizer* _harmon;
	SignalTable* _table;
    PeriodEstimator* _period;
    AdaptiveTuner* _autoTune;
    SpectralEstimator* _spectral;
    AbstractPitchDetector* _pdet;
    RangedRandomNumberGenerator* _urand;
    RangedRandomNumberGenerator* _rrand;
    RangedRandomNumberGenerator* _urand2;
    RangedRandomNumberGenerator* _rrand2;
    RangedRandomNumberGenerator* _urand3;
	RangedRandomNumberGenerator* _rrand3;
    RangedRandomNumberGenerator* _rrand4;

    NRev* _reverbLeft;
 	NRev* _reverbRight;

	FXBMPIcon* _sineIcon;
	FXBMPIcon* _squareIcon;
    FXBMPIcon* _harmonIcon;
	FXBMPIcon* _pdetIcon;
private:
    StringTable* _stringTable;

    MyApp () {}
    MyApp (const MyApp&);
    MyApp& operator= (const MyApp&);
public:
  	enum {
    	ID_LAST = FXApp::ID_LAST
	};
public:
	MyApp (const FXString& name, const FXString& vendor);
 	virtual ~MyApp ();
	virtual void init (int& argc, char** argv, FXbool connect = TRUE);
   	virtual void exit (FXint code = 0);
   	virtual void configInit ();
   	long dummy (FXObject* sender, FXSelector sel, void*);
};

#endif	// MYAPP_H

// EOF

