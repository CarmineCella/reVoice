// MainWindow.h
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "MyApp.h"
#include "Configure.h"
#include "StringTable.h"
#include <fx.h>
#ifdef _WIN32
	#include <windows.h>
#endif


struct ColorTheme {
  const FXchar* name;
  FXColor base;
  FXColor border;
  FXColor back;
  FXColor fore;
  FXColor selback;
  FXColor selfore;
  FXColor tipback;
  FXColor tipfore;
  FXColor menuback;
  FXColor menufore;
};

class MainWindow : public FXMainWindow {
	FXDECLARE (MainWindow)
	
	Configure* _config;
	StringTable* _stringTable;

	FXMenuPane* _fileMenu;
	FXMenuPane* _viewMenu;
	FXMenuPane* _runMenu;
  	FXMenuPane* _toolsMenu;
  	FXMenuPane* _MIDIMonSubMenu;
	FXMenuPane* _helpMenu;
	
	FXToolBar* _toolBar;
	FXStatusBar* _statusBar;
    FXTimer* _timer;
	FXHorizontalFrame* _bookFrame;
	FXLabel* _audioDevLabel;
	FXLabel* _MIDIDevLabel;
 	FXLabel* _engineLatencyLabel;
 	FXLabel* _recordLabel;
  	FXDataTarget _CPUTarget;	
  	int _CPULoad;
    FXDataTarget _leftTarget;
    int _leftLevel;
    FXDataTarget _rightTarget;
    int _rightLevel;
 	
	FXMenuCheck* _ch3;
	FXToggleButton* _playButton;
	FXMenuCheck* _ch4;
	FXToggleButton* _enableButton;
	FXMenuCheck* _ch6;
	FXToggleButton* _recordButton;
	
	// reVoice specific...
	FXLabel* _olapLabel;	
	FXLabel* _frequencyLabel;
	FXLabel* _noteLabel;
	FXLabel* _toneLabel;
	//FXLabel* _multiValue;
	FX7Segment* _multiValue;
	
	FXFont* _titleFont;
	FXPopup* _pdetPop;
	FXButton* _fileButton;
	FXTextField* _fileField;
	FXButton* _defaultButton;
	FXButton* _randomButton;
	FXButton* _defaultButton2;
	FXButton* _randomButton2;
	FXButton* _defaultButton2a;
	FXButton* _randomButton2a;
	FXButton* _defaultButton3;
	FXButton* _randomButton3;	
	FXButton* _defaultButton4;
	FXButton* _randomButton4;	
	FXListBox* _masterKeyCombo;
	FXListBox* _scaleTypeCombo;	
	FXTextField* _partialsAmpValues[PARTIALS];
	FXTextField* _partialsFreqValues[PARTIALS];
	FXRealSlider* _partialsAmpSliders[PARTIALS];
	FXRealSlider* _partialsFreqSliders[PARTIALS];
	FXTextField* _vibratoAmpValues[PARTIALS];
	FXTextField* _vibratoFreqValues[PARTIALS];
	FXRealSlider* _vibratoAmpSliders[PARTIALS];
	FXRealSlider* _vibratoFreqSliders[PARTIALS];
//	FXTextField* _randVibratoAmpValues[PARTIALS];
	FXTextField* _randVibratoFreqValues[PARTIALS];
//	FXRealSlider* _randVibratoAmpSliders[PARTIALS];
	FXRealSlider* _randVibratoFreqSliders[PARTIALS];
	FXRealSlider* _panSliders[PARTIALS];
	FXProgressBar* _panProgress[PARTIALS];
	FXRealSlider* _formantsSliders[PARTIALS];
    FXRadioButton* _radioButtons[3];
	FXTextField* _ratioField;
	FXDial* _ratioDial;
	FXComboBox* _transposeCombo;
	FXProgressBar* _attackProgress;
	FXProgressBar* _decayProgress;
	FXProgressBar* _morphAttackProgress;
	FXProgressBar* _morphDecayProgress;	
	FXSpinner* _cyclesSpinner;
	FXTextField* _freqThreshField;
	FXRealSlider* _freqThreshSlider;
	FXProgressBar* _jitterProgress;
	FXRealSlider* _jitterSlider;
	FXCheckButton* _voiceFilters[PARTIALS];
	// pdet here..
 	FXTextField* _feedbackField;
	FXRealSlider* _feedbackSlider;
 	FXTextField* _qualityField;
	FXRealSlider* _qualitySlider;
	FXTextField* _ampThreshField;
	FXRealSlider* _ampThreshSlider;
	FXLabel* _filterStatus;
	FXTextField* _smoothField;
	FXDial* _smoothDial;
	FXTextField* _smoothQualField;
	FXRealSlider* _smoothQualSlider;
	
 	FXTextField* _rangeStartField1;
 	FXTextField* _rangeEndField1;
	FXRealSlider* _rangeStartSlider1;
	FXRealSlider* _rangeEndSlider1;	
 	FXTextField* _qualityField1;
	FXRealSlider* _qualitySlider1;
	FXTextField* _ampThreshField1;
	FXRealSlider* _ampThreshSlider1;
	//FXTextField* _speedField1;
	//FXRealSlider* _speedSlider1;
	FXLabel* _filterStatus1;
	FXTextField* _smoothFieldATune;
	FXDial* _smoothDialATune;
	FXTextField* _smoothQualFieldATune;
	FXRealSlider* _smoothQualSliderATune;
	
	FXTextField* _rangeStartField2;
 	FXTextField* _rangeEndField2;
	FXRealSlider* _rangeStartSlider2;
	FXRealSlider* _rangeEndSlider2;	
 	FXTextField* _qualityField2;
	FXRealSlider* _qualitySlider2;	
	FXTextField* _ampThreshField2;
	FXRealSlider* _ampThreshSlider2;
	FXLabel* _FFTStatus;	
	FXTextField* _smoothFieldSpectral;
	FXDial* _smoothDialSpectral;
	FXTextField* _smoothQualFieldSpectral;
	FXRealSlider* _smoothQualSliderSpectral;	
	
	// morph
	FXTextField* _morphDetParamValues[PARTIALS];
	FXTextField* _morphRandParamValues[PARTIALS];
	FXRealSlider* _morphDetParamSliders[PARTIALS];
	FXRealSlider* _morphRandParamSliders[PARTIALS];
	FXTextField* _phJitterField;
	FXDial* _phJitterDial;
	FXProgressBar* _widthProgress;
 	FXTextField* _speedFactorField;
	FXRealSlider* _speedFactorSlider;	
	FXCheckButton* _grainletCheck;
	FXCheckButton* _revGrainCheck;	
	
	FXLabel* _harmonUsageLabel;
	FXLabel* _morphUsageLabel;
	
	// mix	
	FXProgressBar* _sourceProgress;
	FXProgressBar* _harmonyProgress;
	FXProgressBar* _morphProgress;
	FXProgressBar* _effectsProgress;
	FXTextField* _reverbField;
	FXRealSlider* _reverbSlider;
	
	bool _ready;
	
	FXColor m_hilite;
  	FXColor m_shadow;
  	ColorTheme m_theme;
protected:
	MainWindow () {}

public:
	MainWindow (MyApp* app);
	~MainWindow ();

  	virtual void create ();
	MyApp* getApp () const;

   	enum {
   	    ID_FILEOPEN = FXMainWindow::ID_LAST,
   	    ID_FILEDUMP,
   	    ID_FILEQUIT,
   	    ID_FILERESETQUIT,
   	    ID_RUNPLAY,
   	    ID_RUNRECORD,
   	    ID_RUNENABLE,
   	    #ifdef _WIN32
   	    	ID_TOOLSSNDVOL,
   	    #endif
		ID_TOOLSSETTINGS,
		ID_TOOLSSAVEMON,
		ID_TOOLSCLEARMON,
		ID_HELPCONTENTS,
		ID_HELPWEB,
		ID_HELPMAIL,
   	    ID_HELPABOUT,
   	    ID_SPLASHSCREEN,
        ID_TIMER,
        
        // reVoice specific... 
        ID_MULTIPLIER,
        ID_HELPPROTON,
  		ID_PANIC,
  		ID_PHIRESET,  		
        ID_WAVEFORM,
     	ID_BROWSE,
     	ID_FILEFIELD,
     	ID_LOGIC,
     	ID_VIBRATOLOGIC,
     	ID_RANDVIBRATOLOGIC,
     	ID_PANNINGLOGIC,
     	ID_FORMANTSLOGIC,
     	ID_DEFAULT,
     	ID_RANDOM,
     	ID_DEFAULT2,
     	ID_RANDOM2,
     	ID_DEFAULT2A,
     	ID_RANDOM2A,
     	ID_DEFAULT3,
     	ID_RANDOM3,
     	ID_DEFAULT4,
     	ID_RANDOM4,     	
     	ID_F1,
     	ID_F2,
		ID_F3,
		ID_F4,
     	ID_F1TEXT,
     	ID_F2TEXT,
		ID_F3TEXT,
		ID_F4TEXT,		
     	ID_A1,
     	ID_A2,
     	ID_A3,
     	ID_A4,     	
      	ID_A1TEXT,
     	ID_A2TEXT,
     	ID_A3TEXT,
     	ID_A4TEXT,
     	ID_VF1,
     	ID_VF2,
		ID_VF3,
		ID_VF4,
     	ID_VF1TEXT,
     	ID_VF2TEXT,
		ID_VF3TEXT,
		ID_VF4TEXT,		
     	ID_VA1,
     	ID_VA2,
     	ID_VA3,
     	ID_VA4,     	
      	ID_VA1TEXT,
     	ID_VA2TEXT,
     	ID_VA3TEXT,
     	ID_VA4TEXT,
     	ID_VFRAND1,
     	ID_VFRAND2,
		ID_VFRAND3,
		ID_VFRAND4,
     	ID_VFRAND1TEXT,
     	ID_VFRAND2TEXT,
		ID_VFRAND3TEXT,
		ID_VFRAND4TEXT,		
//     	ID_VARAND1,
//     	ID_VARAND2,
//     	ID_VARAND3,
//     	ID_VARAND4,     	
//      	ID_VARAND1TEXT,
//     	ID_VARAND2TEXT,
//     	ID_VARAND3TEXT,
//     	ID_VARAND4TEXT,
     	ID_VOICECHECK1,
     	ID_VOICECHECK2,
     	ID_VOICECHECK3,
     	ID_VOICECHECK4,
     	ID_VOICEFILTER1,
     	ID_VOICEFILTER2,
     	ID_VOICEFILTER3,
     	ID_VOICEFILTER4,
        ID_PANNING1,
		ID_PANNING2,
  		ID_PANNING3,
  		ID_PANNING4, 
        ID_FORMANT1,
		ID_FORMANT2,
  		ID_FORMANT3,
  		ID_FORMANT4,   		
     	ID_RADIO1,
     	ID_RADIO2,
     	ID_RADIO3,
     	ID_INTERVAL,
     	ID_MANUAL,
       	ID_MANUALTEXT,
     	ID_ENVELOPE,
     	ID_ATTACK,
     	ID_DECAY,
     	ID_AUTOMODE,
     	ID_CYCLES,
     	ID_FREQTHRESHOLD,
     	ID_FREQTHRESHOLDTEXT,
     	ID_SEMITONESTHRESH,
     	ID_MASTERKEY,
     	ID_SCALE,
     	ID_JITTER,
     	ID_PDET1,
     	ID_PDET2,
     	ID_PDET3,
     	ID_TRACKPITCH,
     	ID_BYPASS,
     	ID_FREEZE,
     	ID_ANTIRUMBLE,
     	// pdet here...
     	ID_FEEDBACK,
     	ID_FEEDBACKTEXT,
     	ID_QUALITY,
     	ID_QUALITYTEXT,
     	ID_SPLIT,
     	ID_AMPTHRESHOLD,
     	ID_AMPTHRESHOLDTEXT,
     	ID_ZCCYCLES,
     	ID_FILTERS,
		ID_SMOOTHQUAL,
     	ID_SMOOTHQUALTEXT,
     	ID_SMOOTHING,
     	ID_SMOOTHINGTEXT,
     	ID_RANGESTART1,
     	ID_RANGESTARTTEXT1,
     	ID_RANGEEND1,
     	ID_RANGEENDTEXT1,
     	ID_QUALITY1,
     	ID_QUALITYTEXT1,
     	ID_AMPTHRESHOLD1,
     	ID_AMPTHRESHOLDTEXT1,
     	//ID_SPEED1,
     	//ID_SPEEDTEXT1,
     	ID_FILTERS1,
		ID_SMOOTHQUAL1,
     	ID_SMOOTHQUALTEXT1,
     	ID_SMOOTHING1,
     	ID_SMOOTHINGTEXT1,
     	ID_RANGESTART2,
     	ID_RANGESTARTTEXT2,
     	ID_RANGEEND2,
     	ID_RANGEENDTEXT2,
     	ID_QUALITY2,
     	ID_QUALITYTEXT2,     	
     	ID_AMPTHRESHOLD2,
     	ID_AMPTHRESHOLDTEXT2, 
     	ID_SPLIT2,     	  
     	ID_FFTSIZE,
		ID_SMOOTHQUAL2,
     	ID_SMOOTHQUALTEXT2,
     	ID_SMOOTHING2,
     	ID_SMOOTHINGTEXT2,     	  	 	
     	// morph
     	ID_MORPHENVELOPE,
     	ID_MORPHATTACK,
     	ID_MORPHDECAY,
     	ID_MORPH1DET,
     	ID_MORPH2DET,
		ID_MORPH3DET,
		ID_MORPH4DET,
     	ID_MORPH1DETTEXT,
     	ID_MORPH2DETTEXT,
		ID_MORPH3DETTEXT,
		ID_MORPH4DETTEXT,		
     	ID_MORPH1RAND,
     	ID_MORPH2RAND,
		ID_MORPH3RAND,
		ID_MORPH4RAND,
     	ID_MORPH1RANDTEXT,
     	ID_MORPH2RANDTEXT,
		ID_MORPH3RANDTEXT,
		ID_MORPH4RANDTEXT,	  
		ID_PHJITTER,
		ID_PHJITTERTEXT, 	
		ID_WIDTH,
		ID_SPEEDFACTOR,
		ID_SPEEDFACTORTEXT,
		ID_REVGRAIN,
		ID_GRAINLET,
     	// mix
     	ID_LEVEL1,
     	ID_LEVEL2,
     	ID_LEVEL3,
     	ID_LEVEL4,
     	ID_REVERB,
     	ID_REVERBTEXT,
   	    ID_LAST
	};

 	long onCmdFileOpen (FXObject*, FXSelector, void*);
 	long onCmdFileDump (FXObject*, FXSelector, void*); 	
	long onCmdFileQuit (FXObject*, FXSelector, void*);
	long onCmdFileResetAndQuit (FXObject*, FXSelector, void*);
	long onCmdRunPlay (FXObject*, FXSelector, void*);	
	long onCmdRunRecord (FXObject*, FXSelector, void*);
	long onCmdRunEnable (FXObject*, FXSelector, void*);
	long onCmdToolsSaveMon (FXObject*, FXSelector, void*);	
	long onCmdToolsClearMon (FXObject*, FXSelector, void*);
	#ifdef _WIN32
		long onCmdToolsSndVol (FXObject*, FXSelector, void*);	
	#endif
	long onCmdToolsSettings (FXObject*, FXSelector, void*);	
	long onCmdHelpContents (FXObject*, FXSelector, void*);
	long onCmdHelpWeb (FXObject*, FXSelector, void*);
	long onCmdHelpMail (FXObject*, FXSelector, void*);
	long onCmdHelpAbout (FXObject*, FXSelector, void*);
	long onCmdSplashScreen (FXObject*, FXSelector, void*);
	long onTimer (FXObject*, FXSelector, void*);
	
	// reVoice specific...
	long onCmdHelpProton (FXObject*, FXSelector, void*);
	
//	long onCmdVoiceOptions (FXObject*, FXSelector, void*);
	long onCmdPanic (FXObject*, FXSelector, void*);
	long onCmdPhiReset (FXObject*, FXSelector, void*);
	long onCmdMultiplier (FXObject*, FXSelector, void*);	
	long onCmdWaveform (FXObject*, FXSelector, void*);
	long onCmdFileBrowse (FXObject*, FXSelector, void*);
	long onCmdLogic (FXObject*, FXSelector, void*);
	long onCmdDefault (FXObject*, FXSelector, void*);
	long onCmdRandom (FXObject*, FXSelector, void*);
	long onCmdSlider (FXObject*, FXSelector, void*);
	long onCmdSliderText (FXObject*, FXSelector, void*);
	long onCmdVibratoLogic (FXObject*, FXSelector, void*);
	long onCmdDefault2 (FXObject*, FXSelector, void*);
	long onCmdRandom2 (FXObject*, FXSelector, void*);
	long onCmdVibratoSlider (FXObject*, FXSelector, void*);
	long onCmdVibratoSliderText (FXObject*, FXSelector, void*);
	long onCmdRandVibratoLogic (FXObject*, FXSelector, void*);
	long onCmdDefault2a (FXObject*, FXSelector, void*);
	long onCmdRandom2a (FXObject*, FXSelector, void*);
	long onCmdRandVibratoSlider (FXObject*, FXSelector, void*);
	long onCmdRandVibratoSliderText (FXObject*, FXSelector, void*);
	long onCmdVoiceCheck (FXObject*, FXSelector, void*);
	long onCmdVoiceFilter (FXObject*, FXSelector, void*);
	long onCmdPanningLogic (FXObject*, FXSelector, void*);
	long onCmdDefault3 (FXObject*, FXSelector, void*);
	long onCmdRandom3 (FXObject*, FXSelector, void*);
	long onCmdPanSlider (FXObject*, FXSelector, void*);
	long onCmdFormantsLogic (FXObject*, FXSelector, void*);	
	long onCmdDefault4 (FXObject*, FXSelector, void*);
	long onCmdRandom4 (FXObject*, FXSelector, void*);
	long onCmdFormantSlider (FXObject*, FXSelector, void*);	
	long onCmdRadio (FXObject*, FXSelector, void*);
	long onCmdSemitones (FXObject*, FXSelector, void*);
	long onCmdManual (FXObject*, FXSelector, void*);
	long onCmdManualText (FXObject*, FXSelector, void*);
	long onCmdEnvelope (FXObject*, FXSelector, void*);
	long onCmdAttackDecay (FXObject*, FXSelector, void*);
	long onCmdMorphEnvelope (FXObject*, FXSelector, void*);
	long onCmdMorphAttackDecay (FXObject*, FXSelector, void*);	
	long onCmdAutoMode (FXObject*, FXSelector, void*);
	long onCmdCycles (FXObject*, FXSelector, void*);	
	long onCmdFreqThreshold (FXObject*, FXSelector, void*);
	long onCmdFreqThresholdText (FXObject*, FXSelector, void*);
	long onCmdSemitonesThresh (FXObject*, FXSelector, void*);
	long onCmdMasterKey (FXObject*, FXSelector, void*);
	long onCmdScale (FXObject*, FXSelector, void*);
	long onCmdJitter (FXObject*, FXSelector, void*);
	long onCmdPitchDetector (FXObject*, FXSelector, void*);
	long onCmdTrackPitch (FXObject*, FXSelector, void*);
	long onCmdBypass (FXObject*, FXSelector, void*);
	long onCmdFreeze (FXObject*, FXSelector, void*);
	long onCmdAntiRumble (FXObject*, FXSelector, void*);
	long onCmdFeedback (FXObject*, FXSelector, void*);
	long onCmdFeedbackText (FXObject*, FXSelector, void*);
	long onCmdQuality (FXObject*, FXSelector, void*);
	long onCmdQualityText (FXObject*, FXSelector, void*);
	long onCmdSplit (FXObject*, FXSelector, void*);
	long onCmdAmpThreshold (FXObject*, FXSelector, void*);
	long onCmdAmpThresholdText (FXObject*, FXSelector, void*);
	long onCmdZCCycles (FXObject*, FXSelector, void*);
	long onCmdFilters (FXObject*, FXSelector, void*);
	long onCmdSmoothingQual (FXObject*, FXSelector, void*);
	long onCmdSmoothingQualText (FXObject*, FXSelector, void*);
	long onCmdSmoothing (FXObject*, FXSelector, void*);
	long onCmdSmoothingText (FXObject*, FXSelector, void*);
	long onCmdRangeStartEnd1 (FXObject*, FXSelector, void*);
	long onCmdRangeStartEndText1 (FXObject*, FXSelector, void*);
	long onCmdQuality1 (FXObject*, FXSelector, void*);
	long onCmdQualityText1 (FXObject*, FXSelector, void*);
	long onCmdAmpThreshold1 (FXObject*, FXSelector, void*);
	long onCmdAmpThresholdText1 (FXObject*, FXSelector, void*);
//	long onCmdSpeed1 (FXObject*, FXSelector, void*);
//	long onCmdSpeedText1 (FXObject*, FXSelector, void*);
	long onCmdSmoothingQual1 (FXObject*, FXSelector, void*);
	long onCmdSmoothingQualText1 (FXObject*, FXSelector, void*);
	long onCmdSmoothing1 (FXObject*, FXSelector, void*);
	long onCmdSmoothingText1 (FXObject*, FXSelector, void*);	
	long onCmdFilters1 (FXObject*, FXSelector, void*);	
	long onCmdRangeStartEnd2 (FXObject*, FXSelector, void*);
	long onCmdRangeStartEndText2 (FXObject*, FXSelector, void*);	
	long onCmdQuality2 (FXObject*, FXSelector, void*);
	long onCmdQualityText2 (FXObject*, FXSelector, void*);	
	long onCmdAmpThreshold2 (FXObject*, FXSelector, void*);
	long onCmdAmpThresholdText2 (FXObject*, FXSelector, void*);	
	long onCmdSplit2 (FXObject*, FXSelector, void*);	
	long onCmdFFTSize (FXObject*, FXSelector, void*);		
	long onCmdSmoothingQual2 (FXObject*, FXSelector, void*);
	long onCmdSmoothingQualText2 (FXObject*, FXSelector, void*);
	long onCmdSmoothing2 (FXObject*, FXSelector, void*);
	long onCmdSmoothingText2 (FXObject*, FXSelector, void*);		
	long onCmdMixer (FXObject*, FXSelector, void*);
	long onCmdReverb (FXObject*, FXSelector, void*);
	long onCmdReverbText (FXObject*, FXSelector, void*);
	// morph
	long onCmdMorphSlider (FXObject*, FXSelector, void*);
	long onCmdMorphSliderText (FXObject*, FXSelector, void*);		
	long onCmdPhJitter (FXObject*, FXSelector, void*);
	long onCmdPhJitterText (FXObject*, FXSelector, void*);	
	long onCmdWidth (FXObject*, FXSelector, void*);	
	long onCmdSpeedFactorText (FXObject*, FXSelector, void*);	
	long onCmdSpeedFactor (FXObject*, FXSelector, void*);	
	long onCmdReverseGrain (FXObject*, FXSelector, void*);
	long onCmdGrainlet (FXObject*, FXSelector, void*);
	
	void guiMaker (MyApp* app);
	void setupColors (const ColorTheme& t);
};

#endif	// MAINWINDOW_H

// EOF

