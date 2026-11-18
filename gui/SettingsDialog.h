// SettingsDialog.h
//

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//#include "proton.h"
#include "pandora.h"
#include "MainWindow.h"
#include "Configure.h"
#include "StringTable.h"
#include <fx.h>

class SettingsDialog : public FXDialogBox {
	FXDECLARE (SettingsDialog)

	Configure* _config;
	StringTable* _stringTable;
public:
   	enum {
   	    ID_ACCEPT = FXMainWindow::ID_LAST,
   	    ID_CANCEL,
   	    ID_BROWSE,
   	    ID_MIDIINPUT,
   	    ID_COMBO1,
   	    ID_COMBO2,
   	    ID_COMBO3,
   	    ID_COMBO4,
   	    ID_RANGEMOD,
   	    ID_LAST
	};
	
	SettingsDialog (MainWindow* owner);
	~SettingsDialog ();
	
	long onCmdCancel (FXObject*, FXSelector, void*);
	long onCmdAccept (FXObject*, FXSelector, void*);
	long onCmdCombo1 (FXObject*, FXSelector, void*);
	long onCmdCombo2 (FXObject*, FXSelector, void*);
	long onCmdCombo3 (FXObject*, FXSelector, void*);
	long onCmdCombo4 (FXObject*, FXSelector, void*);
	long onCmdBrowse (FXObject*, FXSelector, void*);
	long onCmdRange (FXObject*, FXSelector, void*);	
private:
    FXCheckButton* _splash;
    FXDataTarget _CPUTarget;
    int _CPUPerc;
    FXComboBox* _audioInputsCombo;
	FXComboBox* _audioOutputsCombo; 
	FXLabel* _audioInputsInfo;
	FXLabel* _audioOutputsInfo;
    FXComboBox* _MIDIInputsCombo;
	FXComboBox* _MIDIOutputsCombo; 
	FXLabel* _MIDIInputsInfo;
	FXLabel* _MIDIOutputsInfo;
	FXComboBox* _samplingRatesCombo;
	FXSpinner* _frames;
	FXSpinner* _delay;
	FXSpinner* _delayUsed;	
	FXComboBox* _channelsCombo;
	FXTextField* _fileField;
	FXCheckButton* _audioOn;
	FXCheckButton* _thru;	
	FXCheckButton* _active;	
	FXCheckButton* _sysex;	
	FXCheckButton* _clock;	
	FXCheckButton* _MIDIOn;

	SoundInfo* _info;
 	std::vector <int> _audioRec;
 	std::vector <int> _audioPlay;
 	std::vector <int> _MIDIIn;
 	std::vector <int> _MIDIOut;
 	 
	SettingsDialog () {}
};

#endif	// SETTINGSDIALOG_H

// EOF


