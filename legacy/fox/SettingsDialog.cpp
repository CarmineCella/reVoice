// SettingsDialog.cpp
//

#include <string>
#include <sstream>
#include "SettingsDialog.h"

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

extern double standardSampleRates[];
extern double maxCPULoad;

FXDEFMAP (SettingsDialog) SettingsDialogMap[] = {
    FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_CANCEL, SettingsDialog::onCmdCancel),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_ACCEPT, SettingsDialog::onCmdAccept),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_COMBO1, SettingsDialog::onCmdCombo1),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_COMBO2, SettingsDialog::onCmdCombo2),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_COMBO3, SettingsDialog::onCmdCombo3),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_COMBO4, SettingsDialog::onCmdCombo4),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_BROWSE, SettingsDialog::onCmdBrowse),
	FXMAPFUNC (SEL_COMMAND, SettingsDialog::ID_RANGEMOD, SettingsDialog::onCmdRange),
};

FXIMPLEMENT (SettingsDialog, FXDialogBox, SettingsDialogMap,
	ARRAYNUMBER (SettingsDialogMap))

SettingsDialog::SettingsDialog (MainWindow* owner) : FXDialogBox (owner, "",
	DECOR_TITLE | DECOR_BORDER) {
	_config = Configure::instance ();
	_stringTable = StringTable::instance ();
  	setTitle (_stringTable->get ("SETTINGSDLG").c_str ());

   	_info = SoundInfo::instance ();

	// Interior
    FXVerticalFrame* vertical = new FXVerticalFrame (this,
    	LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y); // | LAYOUT_FIX_WIDTH
     	//| LAYOUT_FIX_HEIGHT, 0,0,300,300);
    FXHorizontalFrame* horizontal = new FXHorizontalFrame (vertical,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXVerticalFrame* buttons = new FXVerticalFrame (horizontal,
    	LAYOUT_LEFT | LAYOUT_FILL_Y | FRAME_SUNKEN | PACK_UNIFORM_WIDTH |
     	PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0, 0,0);
    FXSwitcher* switcher = new FXSwitcher (horizontal,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

    _CPUPerc = (int) ((double) 100 * _config->number ("Global", "MAXCPULoad"));
    _CPUTarget.connect (_CPUPerc);
    // Global
    FXVerticalFrame* globalPane = new FXVerticalFrame (switcher,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 0,0);
    new FXLabel (globalPane, _stringTable->get ("GLOBALTAB").c_str (),
    	0, LAYOUT_LEFT);
    new FXHorizontalSeparator (globalPane, SEPARATOR_LINE | LAYOUT_FILL_X);
    FXVerticalFrame* globalPrefs = new FXVerticalFrame (globalPane,
    	LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0, 4,4,4,4, 4, 4);
    _splash = new FXCheckButton (globalPrefs, _stringTable->get ("SHOWSPLASHBUTTON").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
	new FXHorizontalSeparator (globalPrefs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    FXGroupBox* CPU = new FXGroupBox (globalPrefs, _stringTable->get ("MAXCPULOAD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	FXVerticalFrame* CPUFrame = new FXVerticalFrame (CPU,
     	LAYOUT_FILL_X,
     	0,0,0,0, 0,0,0,0, 4, 4);
    new FXProgressBar (CPUFrame, &_CPUTarget, FXDataTarget::ID_VALUE,
    	LAYOUT_FILL_X | PROGRESSBAR_PERCENTAGE | PROGRESSBAR_DIAL);
    new FXSlider (CPUFrame, &_CPUTarget, FXDataTarget::ID_VALUE,
    	LAYOUT_FILL_X, 0, 0, 100);

    new FXButton (buttons, _stringTable->get ("GLOBALTABBUTTON").c_str (),
    	owner->getApp ()->_globalIcon, switcher, FXSwitcher::ID_OPEN_FIRST,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);

 	// Audio
    FXVerticalFrame* audioPane = new FXVerticalFrame (switcher,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 0,0);
    new FXLabel (audioPane, _stringTable->get ("AUDIOTAB").c_str (),
    	0, LAYOUT_LEFT);
    new FXHorizontalSeparator (audioPane, SEPARATOR_LINE | LAYOUT_FILL_X);
	FXTabBook* tabBook1 = new FXTabBook (audioPane, NULL,
 		0, PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X |
   		LAYOUT_FILL_Y | LAYOUT_RIGHT);
    new FXTabItem (tabBook1,
    	_stringTable->get ("AUDIOINPUTTAB").c_str ());
	FXVerticalFrame* audioInputs = new FXVerticalFrame (tabBook1,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
    _audioInputsCombo = new FXComboBox (audioInputs, 0, this,
    	SettingsDialog::ID_COMBO1,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X);
	_audioInputsCombo->setNumVisible (10);
	new FXHorizontalSeparator (audioInputs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	_audioInputsInfo = new FXLabel (audioInputs, (FXString) "", 0,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_SIDE_BOTTOM | JUSTIFY_RIGHT |
   		JUSTIFY_TOP | FRAME_GROOVE);
  	new FXTabItem (tabBook1,
    	_stringTable->get ("AUDIOOUTPUTTAB").c_str ());
	FXVerticalFrame* audioOutputs = new FXVerticalFrame (tabBook1,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
    _audioOutputsCombo = new FXComboBox (audioOutputs, 0, this,
    	SettingsDialog::ID_COMBO2,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X);
 	_audioOutputsCombo->setNumVisible (10);
	new FXHorizontalSeparator (audioOutputs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	_audioOutputsInfo = new FXLabel (audioOutputs, (FXString) "", 0,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_SIDE_BOTTOM | JUSTIFY_RIGHT |
   		JUSTIFY_TOP | FRAME_GROOVE);
  	new FXTabItem (tabBook1,
    	_stringTable->get ("PREFERENCES").c_str ());
    FXVerticalFrame* audioPrefs = new FXVerticalFrame (tabBook1,
    	FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0, 4,4,4,4, 4, 4);
    FXGroupBox* sr = new FXGroupBox (audioPrefs, _stringTable->get ("SAMPLINGRATE").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
    _samplingRatesCombo = new FXComboBox (sr, 0, NULL,
    	0,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
	_samplingRatesCombo->setNumVisible (5);
	_samplingRatesCombo->disable ();
	FXHorizontalFrame* buffFrame = new FXHorizontalFrame (audioPrefs,
		LAYOUT_FILL_X, 0,0,0,0,0,0,0,0);
    FXGroupBox* fr = new FXGroupBox (buffFrame, _stringTable->get ("ENGINEFRAMES").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
    _frames =  new FXSpinner (fr, 0, NULL, 0, SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    _frames->setRange (1, 11025);
    FXGroupBox* del = new FXGroupBox (buffFrame, _stringTable->get ("DELAYLINE").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
#if defined (REPITCH_ONLY)
  		del->hide ();
#endif
    _delay =  new FXSpinner (del, 0, this, ID_RANGEMOD, SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    _delay->setRange (1, 524288);
    FXGroupBox* delUsed = new FXGroupBox (buffFrame, _stringTable->get ("DELAYUSED").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
#if defined (REPITCH_ONLY)
  		delUsed->hide ();
#endif
    _delayUsed =  new FXSpinner (delUsed, 0, NULL, 0, SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    _delayUsed->setRange (0,
		(long) _config->number ("Harmonizer", "DelayLine") - 1);
	new FXHorizontalSeparator (audioPrefs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    FXGroupBox* ch = new FXGroupBox (audioPrefs, _stringTable->get ("INPUTCHANNELS").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
    _channelsCombo = new FXComboBox (ch, 0, NULL,
    	0,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
	_channelsCombo->setNumVisible (3);
    FXGroupBox* file = new FXGroupBox (audioPrefs, _stringTable->get ("OUTPUTPCM").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	FXHorizontalFrame* fileFrame = new FXHorizontalFrame (file, LAYOUT_FILL_X,
     	0,0,0,0, 0,0,0,0, 4, 4);
    new FXButton (fileFrame, _stringTable->get ("BROWSEBUTTON").c_str (), NULL,
    	this, SettingsDialog::ID_BROWSE,
    	LAYOUT_LEFT | FRAME_RAISED | FRAME_THICK, 0,0,0,0, 20,20);
	_fileField = new FXTextField (fileFrame, 0, NULL, 0,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FILL_X,
		0,0,0,0, 4,4,4,4);
	new FXHorizontalSeparator (audioPrefs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    _audioOn = new FXCheckButton (audioPrefs, _stringTable->get ("ENGINEONSTARTUP").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
    new FXButton (buttons, _stringTable->get ("AUDIOTABBUTTON").c_str (),
    	owner->getApp ()->_audioIcon, switcher, FXSwitcher::ID_OPEN_SECOND,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);

 	// MIDI
    FXVerticalFrame* MIDIPane = new FXVerticalFrame (switcher,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 0,0);
    new FXLabel (MIDIPane, _stringTable->get ("MIDITAB").c_str (),
    	0, LAYOUT_LEFT);
    new FXHorizontalSeparator (MIDIPane, SEPARATOR_LINE | LAYOUT_FILL_X);
	FXTabBook* tabBook2 = new FXTabBook (MIDIPane, NULL,
 		0, PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X |
   		LAYOUT_FILL_Y | LAYOUT_RIGHT);
    new FXTabItem (tabBook2,
    	_stringTable->get ("MIDIINPUTTAB").c_str ());
	FXVerticalFrame* MIDIInputs = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
//    _input = new FXCheckButton (MIDIInputs, _stringTable->get ("MIDIINPUT").c_str (),
//    	this,
//    	SettingsDialog::ID_MIDIINPUT, JUSTIFY_RIGHT | LAYOUT_FILL_Y | LAYOUT_FILL_ROW);
    _MIDIInputsCombo = new FXComboBox (MIDIInputs, 0, this,
    	SettingsDialog::ID_COMBO3,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X);
    _MIDIInputsCombo->setNumVisible (10);
	new FXHorizontalSeparator (MIDIInputs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	_MIDIInputsInfo = new FXLabel (MIDIInputs, (FXString) "", 0,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_SIDE_BOTTOM | JUSTIFY_RIGHT |
   		JUSTIFY_TOP | FRAME_GROOVE);
  	new FXTabItem (tabBook2,
    	_stringTable->get ("MIDIOUTPUTTAB").c_str ());
	FXVerticalFrame* MIDIOutputs = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
    _MIDIOutputsCombo = new FXComboBox (MIDIOutputs, 0, this,
    	SettingsDialog::ID_COMBO4,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X);
    _MIDIOutputsCombo->setNumVisible (10);
	new FXHorizontalSeparator (MIDIOutputs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	_MIDIOutputsInfo = new FXLabel (MIDIOutputs, (FXString) "", 0,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_SIDE_BOTTOM | JUSTIFY_RIGHT |
		JUSTIFY_TOP | FRAME_GROOVE);
  	new FXTabItem (tabBook2,
    	_stringTable->get ("PREFERENCES").c_str ());
    FXVerticalFrame* MIDIPrefs = new FXVerticalFrame (tabBook2,
    	FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0, 4,4,4,4, 4, 4);
    _thru = new FXCheckButton (MIDIPrefs, _stringTable->get ("MIDITHRU").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
	new FXHorizontalSeparator (MIDIPrefs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    _active = new FXCheckButton (MIDIPrefs, _stringTable->get ("ACTIVESENSING").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
    _sysex = new FXCheckButton (MIDIPrefs, _stringTable->get ("SYSEX").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
    _clock = new FXCheckButton (MIDIPrefs, _stringTable->get ("REALTIMEMSG").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);
	new FXHorizontalSeparator (MIDIPrefs, SEPARATOR_GROOVE | LAYOUT_FILL_X);
    _MIDIOn = new FXCheckButton (MIDIPrefs, _stringTable->get ("MIDIONSTARTUP").c_str (),
    	0, JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_FILL_X
     	| LAYOUT_FILL_ROW);

    new FXButton (buttons, _stringTable->get ("MIDITABBUTTON").c_str (),
    	owner->getApp ()->_MIDIIcon, switcher, FXSwitcher::ID_OPEN_THIRD,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);

    // Bottom part
    new FXHorizontalSeparator (vertical, SEPARATOR_RIDGE | LAYOUT_FILL_X);
    FXHorizontalFrame* closebox = new FXHorizontalFrame (vertical,
    	LAYOUT_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH);
    new FXButton (closebox, _stringTable->get ("OKBUTTON").c_str (), NULL,
    	this, SettingsDialog::ID_ACCEPT,
    	BUTTON_INITIAL | BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED
     	| FRAME_THICK, 0,0,0,0, 20,20);
    new FXButton (closebox, _stringTable->get ("CANCELBUTTON").c_str (), NULL,
    	this, SettingsDialog::ID_CANCEL,
    	BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED | FRAME_THICK,
     	0,0,0,0, 20,20);

 	_splash->setCheck ((int) _config->number ("Windows", "ShowSplashScreen"));

	_audioRec = owner->getApp ()->_rose->recordDevices ();
 	_audioPlay = owner->getApp ()->_rose->playbackDevices ();
 	_MIDIIn = owner->getApp ()->_remi->inputDevices ();
 	_MIDIOut = owner->getApp ()->_remi->outputDevices ();

 	if (_audioRec.size () == 0) {
		_audioInputsCombo->disable ();
  	}
  	else {
        for (unsigned int i = 0; i < _audioRec.size (); i++) {
            _audioInputsCombo->appendItem (
            	owner->getApp ()->_rose->deviceName (_audioRec[i]).c_str ());
        }
        unsigned int current = (unsigned int) _config->number ("Devices", "AudioRecording");
        if (current >= _audioRec.size ())
        	current = 0;
        _audioInputsCombo->setCurrentItem (current);
    	_audioInputsInfo->setText (
     		owner->getApp ()->_rose->deviceInfo (_audioRec[current]).c_str ());
   	}
  	if (_audioPlay.size () == 0) {
    	_audioOutputsCombo->disable ();
   	}
  	else {
        for (unsigned int i = 0; i < _audioPlay.size (); i++) {
            _audioOutputsCombo->appendItem (
            	owner->getApp ()->_rose->deviceName (_audioPlay[i]).c_str ());
        }
        unsigned int current = (unsigned int) _config->number ("Devices", "AudioPlayback");
        if (current >= _audioPlay.size ())
        	current = 0;
        _audioOutputsCombo->setCurrentItem (current);
    	_audioOutputsInfo->setText (
     		owner->getApp ()->_rose->deviceInfo (_audioPlay[current]).c_str ());
   	}
  	if (_MIDIIn.size () == 0) {
  	    _MIDIInputsCombo->disable ();
  	    _thru->disable ();
  	    _active->disable ();
  	    _sysex->disable ();
  	    _clock->disable ();
   	}
  	else {
        for (unsigned int i = 0; i < _MIDIIn.size (); i++) {
            _MIDIInputsCombo->appendItem (
            	owner->getApp ()->_remi->deviceName (_MIDIIn[i]).c_str ());
        }
        unsigned int current = (unsigned int) _config->number ("Devices", "MIDIInput");
        if (current >= _MIDIIn.size ())
        	current = 0;
        _MIDIInputsCombo->setCurrentItem (current);
    	_MIDIInputsInfo->setText (
     		owner->getApp ()->_remi->deviceInfo (_MIDIIn[current]).c_str ());
   	}
  	if (_MIDIOut.size () == 0) {
  	    _MIDIOutputsCombo->disable ();
   	}
  	else {
        for (unsigned int i = 0; i < _MIDIOut.size (); i++) {
            _MIDIOutputsCombo->appendItem (
            	owner->getApp ()->_remi->deviceName (_MIDIOut[i]).c_str ());
        }
        unsigned int current = (unsigned int) _config->number ("Devices", "MIDIOutput");
        if (current >= _MIDIOut.size ())
        	current = 0;
        _MIDIOutputsCombo->setCurrentItem (current);
    	_MIDIOutputsInfo->setText (
     		owner->getApp ()->_remi->deviceInfo (_MIDIOut[current]).c_str ());
	}

	_channelsCombo->appendItem ("0");
	_channelsCombo->appendItem ("1");
	_channelsCombo->appendItem ("2");
	_channelsCombo->disable (); // reVoice specific...

	for (int i = 0; standardSampleRates[i] > 0; i++) {
	    std::stringstream s;
	    s << standardSampleRates[i];
		_samplingRatesCombo->appendItem (s.str ().c_str ());
	}

    _samplingRatesCombo->setCurrentItem ((int) _config->number ("Devices", "SamplingRate"));
    _frames->setValue ((int)_config->number ("Devices", "EngineFrames"));
    _delay->setValue ((int)_config->number ("Harmonizer", "DelayLine"));
    _delayUsed->setValue ((int)_config->number ("Harmonizer", "UsedDelay"));
    _channelsCombo->setCurrentItem ((int) _config->number ("Devices", "InputChannels"));
    _fileField->setText (_config->literal ("Devices", "OutputPCMFile").c_str ());
    _audioOn->setCheck ((int) _config->number ("Devices", "ConnectEngineOnStartup"));

 	_thru->setCheck ((int) _config->number ("Devices", "EnableMIDIThru"));
 	_active->setCheck ((int) _config->number ("Devices", "FilterActiveSensing"));
 	_sysex->setCheck ((int) _config->number ("Devices", "FilterSysEx"));
 	_clock->setCheck ((int) _config->number ("Devices", "FilterClock"));
 	_MIDIOn->setCheck ((int) _config->number ("Devices", "EnableMIDIOnStartup"));
}

SettingsDialog::~SettingsDialog () {
}

long SettingsDialog::onCmdCancel (FXObject* sender, FXSelector, void*) {
    FXDialogBox::onCmdCancel (this, 0, 0);
	return 1;
}

long SettingsDialog::onCmdRange (FXObject* sender, FXSelector, void*) {
	_delayUsed->setRange (0, _delay->getValue () - 1);
	return 1;
}

long SettingsDialog::onCmdAccept (FXObject* sender, FXSelector, void*) {
	_config->number ("Global", "MAXCPULoad", (double) _CPUPerc / 100);
	maxCPULoad = (double) _CPUPerc / 100;
    _config->number ("Windows", "ShowSplashScreen", _splash->getCheck ());
    _config->number ("Devices", "AudioRecording", _audioInputsCombo->getCurrentItem ());
    _config->number ("Devices", "AudioPlayback", _audioOutputsCombo->getCurrentItem ());
    _config->number ("Devices", "MIDIInput", _MIDIInputsCombo->getCurrentItem ());
    _config->number ("Devices", "MIDIOutput", _MIDIOutputsCombo->getCurrentItem ());

    _config->number ("Devices", "SamplingRate", _samplingRatesCombo->getCurrentItem ());
    _config->number ("Devices", "EngineFrames", _frames->getValue ());
    _config->number ("Harmonizer", "DelayLine", _delay->getValue ());
    _config->number ("Harmonizer", "UsedDelay", _delayUsed->getValue ());
    _config->number ("Devices", "InputChannels", _channelsCombo->getCurrentItem ());
    std::string s;
    s = _fileField->getText ().text ();
    _config->literal ("Devices", "OutputPCMFile", s);
    _config->number ("Devices", "ConnectEngineOnStartup", _audioOn->getCheck ());

	_config->number ("Devices", "EnableMIDIThru", _thru->getCheck ());
    _config->number ("Devices", "FilterActiveSensing", _active->getCheck ());
    _config->number ("Devices", "FilterSysEx", _sysex->getCheck ());
    _config->number ("Devices", "FilterClock", _clock->getCheck ());
    _config->number ("Devices", "EnableMIDIOnStartup", _MIDIOn->getCheck ());
    handle (this, FXSEL (SEL_CLOSE, NULL), 0);
    FXDialogBox::onCmdAccept (this, 0, 0);
	return 1;
}

long SettingsDialog::onCmdCombo1 (FXObject* sender, FXSelector, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	int current = c->getCurrentItem ();
	MyApp* app = (MyApp*) getApp ();
    _audioInputsInfo->setText (
    	app->_rose->deviceInfo (_audioRec[current]).c_str ());
	return 1;
}

long SettingsDialog::onCmdCombo2 (FXObject* sender, FXSelector, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	int current = c->getCurrentItem ();
	MyApp* app = (MyApp*) getApp ();
    _audioOutputsInfo->setText (
    	app->_rose->deviceInfo (_audioPlay[current]).c_str ());
	return 1;
}

long SettingsDialog::onCmdCombo3 (FXObject* sender, FXSelector, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	int current = c->getCurrentItem ();
	MyApp* app = (MyApp*) getApp ();
    _MIDIInputsInfo->setText (
    	app->_remi->deviceInfo (_MIDIIn[current]).c_str ());
	return 1;
}

long SettingsDialog::onCmdCombo4 (FXObject* sender, FXSelector, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	int current = c->getCurrentItem ();
	MyApp* app = (MyApp*) getApp ();
	_MIDIOutputsInfo->setText (
		app->_remi->deviceInfo (_MIDIOut[current]).c_str ());
	return 1;
}

long SettingsDialog::onCmdBrowse (FXObject* sender, FXSelector, void*) {
    FXFileDialog openDialog (this,_stringTable->get ("OPENFILE").c_str (),
    	DECOR_NONE);
    //openDialog.setSelectMode (SELECTFILE_EXISTING);
    openDialog.setDirectory (_config->literal ("Global", "LastDirectory").c_str ());
    openDialog.setPatternList ("Wave PCM (*.wav)");
    if (openDialog.execute ()){
        _fileField->setText (openDialog.getFilename ());
        _config->literal ("Global", "LastDirectory",
        	openDialog.getDirectory ().text ());
    }
	return 1;
}

// EOF

