// stringsEng.h - String table for the english language
//

#ifndef STRINGSENG_H
#define STRINGSENG_H

// TODO: Customize program title...
#if defined (REPITCH_ONLY)
	_table["TITLE"] = "rePitch, ver. 1.00";
	_table["AUTHOR"] = "Build 0.0.1, november 2004";
#else
	_table["TITLE"] = "reVoice, ver. 1.10";
	_table["AUTHOR"] = "Build 1.0.3, may 2007";
#endif
_table["EDITION"] = "Development edition";
// TODO: Customize program credits...
_table["CREDITS"] = "Many thanks to...";
_table["TECHINFO"] = "\nThis software uses the FOX Toolkit Library\n(www.fox-toolkit.org), the Portaudio Library\n(www.portaudio.com) and is based on the\nPandora Audio Framework, written by\nthe author. This software has been\ndeveloped using only\nopen source tools.";
_table["STATUSTAB"] = "Status\tShow status info\tShow information about system status.";
_table["MIDIMONTAB"] = "MIDI monitor\tShow MIDI input\tShow MIDI input messages.";
_table["INFOTAB"] = "Info\tShow information\tShow other information.";
_table["DEVICES"] = "Devices";
_table["LEVELS"] = "Levels";
_table["CPULOAD"] = "CPU load";
_table["MIDI"] = "MIDI: ";
_table["AUDIO"] = "Audio: ";
_table["RECORDING"] = "Recording...";
_table["LATENCY"] = "Latency (msec): ";
_table["NOT_IMPLEMENTED"] = "Function not implemented for this system";
_table["OPENFILE"] = "Select file to open...";
_table["ENGINEERROR"] = "Audio engine error";
_table["RECORDERROR"] = "Audio recording error";
_table["MIDIERROR"] = "MIDI error";
_table["FILEERROR"] = "I/O file error during reading/writing";
_table["LISTSAVED"] = "Message list saved";
_table["SETTINGSDLG"] = "Settings...";
_table["GLOBALTAB"] = "Global settings";
_table["AUDIOTAB"] = "Audio settings";
_table["AUDIOINPUTTAB"] = "Recording";
_table["AUDIOOUTPUTTAB"] = "Playback";
_table["MIDITAB"] = "MIDI settings";
_table["MIDIINPUTTAB"] = "Input";
_table["MIDIOUTPUTTAB"] = "Output";
_table["PREFERENCES"] = "Preferences";
_table["MAXCPULOAD"] = "Max CPU load";
_table["SAMPLINGRATE"] = "Sampling rate";
_table["ENGINEFRAMES"] = "Engine frames";
_table["INPUTCHANNELS"] = "Input channels";
_table["OUTPUTPCM"] = "Output PCM file";
_table["ENGINEONSTARTUP"] = "Connect engine on startup";
_table["INPUTCHANNELS"] = "Input channels";
_table["MIDIINPUT"] = "Enable MIDI input";
_table["MIDITHRU"] = "Enable MIDI thru";
_table["ACTIVESENSING"] = "Filter active sensing (0xFE)";
_table["SYSEX"] = "Filter system exclusives (0xF0)";
_table["REALTIMEMSG"] = "Filter clock messages (0xF8 only)";
_table["MIDIONSTARTUP"] = "Enable MIDI on startup";
// TODO: Customize the help information...
#if defined (REPITCH_ONLY)
	_table["HELP"] = "Not implemented...";
#else
    _table["HELP"] = 
        "* reVoice ver. 1.00 *" 
        "\n"                  
        "\n"                  
        "OVERVIEW.\n"         
        "\n"                  
        "\n"                  
        "reVoice ver. 1.00 is a realtime environment\n" 
        "to synthesize human voice giving, the possibility\n" 
        "to modify the original pitch without altering\n"     
        "the formant structure of the source.\n"                
        "The program can also produce harmonization effects\n"  
        "and is based on synchronous granular synthesis\n"      
        "and on other complex techniques to detect\n"           
        "the pitch of a signal in realtime\n"                   
        "\n"                                                    
        "\n"                                                    
        "LICENSE INFORMATION.\n"								
        "\n"                                                    
        "\n"                                                    
        "reVoice is a software for digital audio developed\n"    
        "to provide a complex mechanism to produce sound in\n"  
        "realtime. Its only purpouses are internal sound development\n"     
        "and research and so the program is given \"as is\" without any\n"  
        "implicit or explicit warranty by the author Carmine Emanuele\n"    
        "Cella. The author will be held NOT RESPONSIBLE for any damage\n"   
        "or improper use of this program.\n"                                
        "This software is copyrighted by Viscount International spa\n"      
        "and so you CANNOT USE, COPY, DISTRIBUTE OR REVERSE ENGINE\n"       
        "ANY PART OF THIS SOFTWARE WITHOUT THE EXPLICIT PERMISSION OF\n"    
        "VISCOUNT INTERNATIONAL SPA.\n"                                     
        "\n"                                                                
        "\n"                                                                
        "TECHNICAL INFORMATION.\n"                                          
        "\n"                                                                
        "\n"                                                                
        "The program is written in C++ and is based on the the\n"           
        "PortAudio library (www.portaudio.com) for the realtime engine,\n"  
        "on the FOX library (www.fox-toolkit.org) for the GUI and on the\n" 
        "Pandora Audio Framework (www.cryptosound.org) for the generation\n"
        "algorithms. This package is made of different programs intended to work\n" 
        "together; for this reason please KEEP ALL THE PACKAGE TOGETHER,\n"         
        "INCLUDING THE FILE LICENSE.TXT.\n"                                         
        "\n"                                                                        
        "\n"                                                                        
        "CONTACTS.\n"                                                               
        "\n"                                                                        
        "\n"                                                                        
        "For any question or for bugs report please contact:\n"                     
        "\n"                                                                        
        "\n"                                                                        
        "Carmine Emanuele Cella <cecily@tele2.it>\n"                                
        "\n"                                                                       
        "\n"                                                                        
        "#EOF\n";
#endif
_table["HELPDLG"] = "Some help...";
// TODO: Customize web page and mail address...
_table["WEBPAGE"] = "www.cryptosound.org";
#if defined (REPITCH_ONLY)
	_table["MAILCOMMAND"] = "mailto:sarabellissima@libero.it?Subject:Program report";
#else
	_table["MAILCOMMAND"] = "mailto:cecily@tele2.it?Subject:Program report";
#endif
_table["ABOUTDLG"] = "About...";

// menu items...
_table["FILEMENU"] = "&File";
_table["FILEOPENMENU"] = "&Open...\tCtrl-O\tOpen a file.";
_table["FILEDUMPMENU"] = "&Dump current status\tCtrl-D\tDump current configuration.";
_table["FILEQUITMENU"] = "&Quit\t\tClose this program.";
_table["FILERESETQUITMENU"] = "&Reset and quit\t\tClose this program clearing all settings.";
_table["RUNMENU"] = "&Run";
_table["RUNPLAYMENU"] = "&Play/stop audio engine\tF5\tStart or stop the audio generation engine.";
_table["RUNRECORDMENU"] = "&Record/stop record\t\tStart or stop the audio recording.";
_table["RUNENABLEMENU"] = "&Enable/disable MIDI\tF6\tEnable or disable MIDI devices.";
_table["VIEWMENU"] = "&View";
_table["VIEWTOOLBARMENU"] = "&ToolBar\t\tToggle viewing the bar.";
_table["VIEWSTATUSBARMENU"] = "&StatusBar\t\tToggle viewing the bar.";
_table["VIEWCONTROLPANELMENU"] = "&Control panel\t\tToggle viewing the panel.";
_table["TOOLSMENU"] = "&Tools";
_table["TOOLSMIDIMONMENU"] = "MIDI monitor";
_table["TOOLSMIDIMONSAVEMENU"] = "&Save list\t\tSave message list on file.";
_table["TOOLSMIDIMONCLEARMENU"] = "&Clear\tCtrl-M\tClear message list in MIDI monitor.";
#ifdef _WIN32
	_table["TOOLSSNDVOLMENU"] = "&Audio configuration...\t\tChanges the system audio configuration.";
#endif
_table["TOOLSSETTINGSMENU"] = "&Settings...\t\tChange program configuration.";
_table["HELPMENU"] = "&?";
_table["HELPCONTENTSMENU"] = "&Contents...\tF1\tDisplay help contents.";
_table["HELPWEBMENU"] = "&On the web...\t\tConnect to web page of the program.";
_table["HELPMAILMENU"] = "&Bugs/comments report...\t\tSend a mail to report bugs/comments.";
_table["HELPABOUTMENU"] = "&About...\t\tShow information about this program.";

// buttons...
_table["OKBUTTON"] = "OK\tAccept changes\tAccept changes done";
_table["CANCELBUTTON"] = "Cancel\tDiscard changes\tDiscard changes done";
_table["BROWSEBUTTON"] = "Browse\tBrowse files\tBrowse the file system";
_table["FILEOPENBUTTON"] = "\tOpen\tOpen a file";
_table["PLAYSTOPBUTTON"] = "\tPlay/stop\tStart or stop audio generation engine";
_table["RECSTOPBUTTON"] = "\tRec/stop rec\tStart or stop audio recording";
_table["ENABLEDISABLEBUTTON"] = "\tEnable/disable\tEnable or disable MIDI devices";
_table["TOOLSSETTINGSBUTTON"] = "\tSettings\tChange program configuration";
_table["HELPCONTENTSBUTTON"] = "\tContents\tDisplay help contents";
_table["HELPABOUTBUTTON"] = "\tAbout\tShow information about this program";
_table["GLOBALTABBUTTON"] = 
	"Global\tGlobal settings\tChange program configuration";
_table["AUDIOTABBUTTON"] = 
	"Audio\tAudio settings\tChange audio settings";
_table["MIDITABBUTTON"] = 
	"MIDI\tMIDI settings\tChange MIDI settings";
_table["SHOWSPLASHBUTTON"] = "Show splash screen on startup";


// reVoice specific...
_table["HELPPROTONHELPMENU"] = "&Pandora help...\t\tShow Pandora audio framework user guide";

#if defined (REPITCH_ONLY)
    _table["HARMONIZER"] = "Pitch shifter";
    _table["HARMONIZERCR"] = "Shifter";
	_table["PITCHDETECTOR"] = "Pitch detector";
#else
	_table["HARMONIZER"] = "Harmonizer";
	_table["HARMONIZERCR"] = "Harmon";
	_table["PITCHDETECTOR"] = "Pitch detectors";
#endif
_table["PITCHDETECTORCR"] = "P.D.";
_table["MIX"] = "Mixer and effects";
_table["MIXCR"] = "Mix";
_table["VOICES"] = "Voices configuration";
_table["VOICESCR"] = "Voices";
_table["MORPHIZER"] = "Morphizer";
_table["MORPHIZERCR"] = "Morph";
_table["VOICEMENU1"] = "&One\tOne voice\tThe harmonizer uses only one voice.";
_table["VOICEMENU2"] = "&Two\tTwo voicese\tThe harmonizer uses two voices.";
_table["VOICEMENU3"] = "Th&ree\tThree voices\tThe harmonizer uses three voices.";
_table["VOICEMENU4"] = "&Four\tFour voices\tThe harmonizer uses four voices.";
_table["WAVEFORM"] = "Waveform";
_table["SINE"] = "Sine (440 Hz)";
_table["SQUARE"] = "Square (440 Hz)";
_table["FROMFILE"] = "Audio file";
_table["LIVEINPUT"] = "Live input";
_table["INPUTPCM"] = "Input file";
_table["PRESET"] = "Preset";
_table["CUSTOM"] = "Custom";
_table["DOMINANTSEVENTH"] = "Dominant seventh";
_table["MINORSEVENTH"] = "Minor seventh";
_table["MAJORSEVENTH"] = "Major seventh";
_table["DIMINISHEDSEVENTH"] = "Diminished seventh";
_table["AUGMENTEDSEVENTH"] = "Augmented seventh";
_table["MINORWITHSIXTH"] = "Minor with sixth";
_table["QUARTALCHORD"] = "Quartal chord";
_table["SCALIC"] = "Scalic";
_table["INTELLICHORD"] = "IntelliChord";
_table["SINGLECHORD"] = "SingleChord";
_table["MIDISTREAM"] = "MIDI stream (Ch. 1)";
_table["SMALLVIBRATO"] = "Small vibrato";
_table["MEDIUMVIBRATO"] = "Medium vibrato";
_table["LARGEVIBRATO"] = "Large vibrato";
_table["SMALLRANDVIBRATO"] = "Small random vib.";
_table["MEDIUMRANDVIBRATO"] = "Medium random vib.";
_table["LARGERANDVIBRATO"] = "Large random vib.";
_table["TWOTWO"] = "Two vs. two";
_table["ONETHREE"] = "One vs. three";
_table["THREEONE"] = "Three vs. one";
_table["MIXED"] = "Mixed choir";
_table["FEMALE"] = "Female choir";
_table["MALE"] = "Male choir";
_table["FEMALELABEL"] = "Female";
_table["MALELABEL"] = "Male";
_table["FUNDAMENTAL"] = "Fundamental";
_table["FOLLOWMIDI"] = "MIDI note (ch. 0)";
_table["TRANSPOSE"] = "Transpose";
_table["MANUAL"] = "Ratio";
_table["SEMITONES"] = "semitone(s)";
_table["DEFAULTBUTTON"] = "Default\tSet default partials configuration";
_table["RANDOMBUTTON"] = "Random\tRandomize partials configuration";
_table["ENVELOPE"] = "Envelope";
_table["RECTANGLE"] = "Rectangle";
_table["BARTLETT"] = "Bartlett";
_table["GAUSSIAN"] = "Gaussian";
_table["HAMMING"] = "Hamming";
_table["HANNING"] = "Hanning";
_table["BLACKMAN"] = "Blackman";
_table["BLACKMANHARRIS"] = "Blackman-Harris";
_table["ATTACKDECAY"] = "Attack / decay";
_table["AUTOMODE"] = "Automatic cycles";
_table["CYCLES"] = "Cycles per grain";
_table["FREQUENCYTHRESHOLD"] = "Frequency confidence";
_table["STUNISON"] = "Unison";
_table["STMINORSECOND"] = "Minor second";
_table["STMAJORSECOND"] = "Major second";
_table["STMINORTHIRD"] = "Minor third";
_table["STMAJORTHIRD"] = "Major third";
_table["STPERFECTFOURTH"] = "Perfect fourth";
_table["STTRITONE"] = "Tritone";
_table["STPERFECTFIFTH"] = "Perfect fifth";
_table["STMINORSIXTH"] = "Minor sixth";
_table["STMAJORSIXTH"] = "Major sixth";
_table["STMINORSEVENTH"] = "Minor seventh";
_table["STMAJORSEVENTH"] = "Major seventh";
_table["STOCTAVE"] = "Perfect octave";
_table["STSMOOTH"] = "Smooth";
_table["MASTERKEY"] = "Master key";
_table["C"] = "C";
_table["C#"] = "C#";
_table["D"] = "D";
_table["D#"] = "D#";
_table["E"] = "E";
_table["F"] = "F";
_table["F#"] = "F#";
_table["G"] = "G";
_table["G#"] = "G#";
_table["A"] = "A";
_table["A#"] = "A#";
_table["B"] = "B";
_table["SCALE"] = "Scale mode";
_table["MAJOR1"] = "Major 1";
_table["MAJOR2"] = "Major 2";
_table["MINOR1"] = "Minor 1";
_table["MINOR2"] = "Minor 2";
_table["MINOR3"] = "Minor 3";
_table["BLUES1"] = "Blues 1";
_table["CHROMA_MAJOR1"] = "Chromatic maj 1";
_table["OCTOPHONIC1"] = "Octophonic 1";
_table["JITTER"] = "Source position"; // Should have a different name
_table["PDET"] = "Pitch detector";
_table["PERIODESTIMATOR"] = "Period estimator";
_table["AUTOTUNER"] = "Auto-tunable filter";
_table["SPECTRALESTIMATOR"] = "Spectral estimator";
_table["RANGESTART"] = "Range start (Hz)";
_table["QUALITY"] = "Filters quality";
_table["FEEDBACK"] = "Feedback";
_table["SPLIT"] = "Split";
_table["AMPLITUDETHRESHOLD"] = "Amplitude threshold";
_table["ZCCYCLES"] = "Zero-crossing cycles";
_table["CORRECTPITCH"] = "Correct pitch";
_table["FILTERS"] = "Filters";
_table["FILTERSBUTTON"] = "Change\tChange filters";
_table["FILTERLEVELS"] = "Levels (x 12 dB/oct.)";
_table["FFTSIZE"] = "Filters and FFT size";
_table["FREQDELTA"] = "Frequency min / max (Hz)";
_table["RANGESTARTEND"] = "Range start / end (Hz)";
_table["SPEED"] = "Speed";
_table["OVERLAP"] = "Overlap";
_table["FREQUENCY"] = "Detected freq. (Hz)";
_table["NOTENAME"] = "Note name";
_table["SCALETONE"] = "Scale tone";
_table["DATADLG"] = "Filters data";
_table["FFTDATADLG"] = "Filters and FFT";
_table["SIZEINBYTES"] = "FFT samples";
_table["USEBIQUAD"] = "Use biquad filters";
_table["RANGEERROR"] = "Range error; please repeat operation";
_table["PANICBUTTON"] = "\tClear\tClear current table";
_table["PHIRESETBUTTON"] = "\tPhase 0\tClear phase";
_table["SMOOTHING"] = "Smoothing rate (Hz)";
_table["SMOOTHQUAL"] = "Smoothing quality";
_table["GLOBAL"] = "Global controls";
_table["TRACKPITCH"] = "Track pitch";
_table["BYPASS"] = "Bypass";
_table["FREEZE"] = "Freeze pitch";
_table["RUMBLE"] = "Anti-rumble";
_table["PREPROCESSING"] = "Pre-processing";
_table["PARTIALS"] = "Ratios";
_table["VIBRATO"] = "Vibrato";
_table["RANDVIBRATO"] = "Random vib.";
_table["PANNING"] = "Panning";
_table["FORMANTS"] = "Formants";
_table["CONTROLS"] = "Controls";
_table["SETUP"] = "Setup";
_table["ACTIVATION"] = "Activation";
_table["HIGHPASSVOICES"] = "Highpass filtering";
_table["VOICE"] = "Voice";
_table["POSITIONS"] = "Positions";
_table["GENDER"] = "Gender";
_table["SOURCE"] = "Source";
_table["HARMONY"] = "Harmony";
_table["MORPHIZER"] = "Morphizer";
_table["EFFECTS"] = "Reverb";
_table["REVERB"] = "Reverb decay (sec.)";
_table["DELAYLINE"] = "Total delay";
_table["DELAYUSED"] = "Used delay";
_table["DENSITY"] = "Density";
_table["DURATION"] = "Duration";
_table["MORPHFREQUENCY"] = "Frequency (ratio)";
_table["GLISSATO"] = "Glissato (ratio)";
_table["PHASEJITTER"] = "Phase jitter (samples)";
_table["SPATIALWIDTH"] = "Spatial width";
_table["REVGRAIN"] = "Reverse grain";
_table["GRAINLET"] = "Grainlet";
_table["MORPHUSAGE"] = "Morph buffer used: ";
_table["HARMONUSAGE"] = "Harmon buffer used: ";

#ifdef OPTIMIZED
	_table["OPTIMIZED"] = "[Optimized build 1.0.3]";
#else
	_table["DEBUG"] = "[Build 1.0.3]";
#endif

_table["AREYOUSURE"] = "Are you sure?";

#endif	// STRINGSENG_H

// EOF

