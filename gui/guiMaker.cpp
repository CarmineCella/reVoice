// guiMaker.cpp
//

#include "MainWindow.h"

char* params[4] = {
	"DENSITY",
	"DURATION",
	"MORPHFREQUENCY",
	"GLISSATO"
};

extern const int TIMEOUT;
extern FXList* msgList;

void MainWindow::guiMaker (MyApp* app) {
	FXToolBarShell* dragShell1 = new FXToolBarShell (this, FRAME_RAISED);
	FXMenuBar* menubar = new FXMenuBar (this, dragShell1,
		FRAME_RAISED | LAYOUT_SIDE_TOP | LAYOUT_FILL_X);
	menubar->setBorderColor (m_theme.border);
	menubar->setBaseColor (m_theme.base);
	menubar->setBackColor (m_theme.base);
	menubar->setShadowColor (m_shadow);
	menubar->setHiliteColor (m_hilite);
			
	FXToolBarGrip* tbgripmenu = new FXToolBarGrip (menubar, menubar, FXMenuBar::ID_TOOLBARGRIP);
	tbgripmenu->setBorderColor (m_theme.border);
	tbgripmenu->setBaseColor (m_theme.base);
	tbgripmenu->setBackColor (m_theme.base);
	tbgripmenu->setShadowColor (m_shadow);
	tbgripmenu->setHiliteColor (m_hilite);
		
	FXToolBarShell* dragShell2 = new FXToolBarShell (this, FRAME_RAISED);
	_toolBar = new FXToolBar (this, dragShell2, LAYOUT_SIDE_TOP |
		LAYOUT_FILL_X | FRAME_RAISED);
	_toolBar->setBorderColor (m_theme.border);
	_toolBar->setBaseColor (m_theme.base);
	_toolBar->setBackColor (m_theme.base);
	_toolBar->setShadowColor (m_shadow);
	_toolBar->setHiliteColor (m_hilite);
			
	_statusBar = new FXStatusBar (this, LAYOUT_SIDE_BOTTOM |
 		LAYOUT_FILL_X | FRAME_RAISED);
	_statusBar->setBorderColor (m_theme.border);
	_statusBar->setBaseColor (m_theme.base);
	_statusBar->setBackColor (m_theme.base);
	_statusBar->setShadowColor (m_shadow);
	_statusBar->setHiliteColor (m_hilite);
	
	FXStatusLine* sline = _statusBar->getStatusLine ();
	sline->setBorderColor (m_theme.border);
	sline->setBaseColor (m_theme.base);
	sline->setBackColor (m_theme.base);
	sline->setTextColor (m_theme.fore);	
	sline->setTextHighlightColor (m_hilite);
		
 	FXHorizontalFrame* frame = new FXHorizontalFrame (this,
 		LAYOUT_SIDE_TOP | LAYOUT_FILL_X| LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0, 4,4);
	frame->setBorderColor (m_theme.border);
	frame->setBaseColor (m_theme.base);
	frame->setBackColor (m_theme.base);
	frame->setShadowColor (m_shadow);
	frame->setHiliteColor (m_hilite);
	
	FXVerticalFrame* box = new FXVerticalFrame (frame,
 		FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y,
   		0,0,0,0, 0,0,0,0);
	box->setBorderColor (m_theme.border);
	box->setBaseColor (m_theme.base);
	box->setBackColor (m_theme.base);
	box->setShadowColor (m_shadow);
	box->setHiliteColor (m_hilite);
		
	////////////////////////////////////////////////////// PANNELLO LATERALE
    _bookFrame = new FXHorizontalFrame (frame,
    	LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH | LAYOUT_RIGHT | FRAME_SUNKEN
 		| FRAME_THICK,
     	0,0,200,200, 0,0,0,0, 0,0);
	_bookFrame->setBorderColor (m_theme.border);
	_bookFrame->setBaseColor (m_theme.base);
	_bookFrame->setBackColor (m_theme.base);
	_bookFrame->setShadowColor (m_shadow);
	_bookFrame->setHiliteColor (m_hilite);     	
	     	
    FXTabBook* panels = new FXTabBook (_bookFrame, NULL, 0, LAYOUT_FILL_Y
    	| LAYOUT_FILL_X,
    	0,0,0,0, 0,0,0,0);
	panels->setBorderColor (m_theme.border);
	panels->setBaseColor (m_theme.base);
	panels->setBackColor (m_theme.base);
	panels->setShadowColor (m_shadow);
	panels->setHiliteColor (m_hilite);     	
	     	    	
    FXTabItem* statustab = new FXTabItem (panels, _stringTable->get ("STATUSTAB").c_str ());
	statustab->setBorderColor (m_theme.border);
	statustab->setBaseColor (m_theme.base);
	statustab->setBackColor (m_theme.base);
	statustab->setTextColor (m_theme.fore);
	statustab->setShadowColor (m_shadow);
	statustab->setHiliteColor (m_hilite);
	
    _CPULoad = (int) 1; //(100 * app->_rose->loadedCPU ());
    _CPUTarget.connect (_CPULoad);
    _leftLevel = (int) (100 * app->_left->get ());
    _leftTarget.connect (_leftLevel);
	_rightLevel = (int) (100 * app->_right->get ());
	_rightTarget.connect (_rightLevel);

    FXVerticalFrame* status = new FXVerticalFrame (panels,
    	FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y |
     	LAYOUT_TOP | LAYOUT_LEFT,
		0,0,0,0, 4,4,4,4, 10, 10);
	status->setBorderColor (m_theme.border);
	status->setBaseColor (m_theme.base);
	status->setBackColor (m_theme.base);	
	status->setShadowColor (m_shadow);
	status->setHiliteColor (m_hilite);
			
    FXGroupBox* VU = new FXGroupBox (status,
    	_stringTable->get ("LEVELS").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	VU->setBorderColor (m_theme.border);
	VU->setBaseColor (m_theme.base);
	VU->setBackColor (m_theme.base);
	VU->setTextColor (m_theme.fore);	
	VU->setShadowColor (m_shadow);
	VU->setHiliteColor (m_hilite);
	    	
	FXVerticalFrame* VUFrame = new FXVerticalFrame (VU,
     	LAYOUT_FILL_Y | LAYOUT_FILL_X,
     	0,0,0,0, 0,0,0,0, 4, 4);
    VUFrame->setBorderColor (m_theme.border);
	VUFrame->setBaseColor (m_theme.base);
	VUFrame->setBackColor (m_theme.base);	
	VUFrame->setShadowColor (m_shadow);
	VUFrame->setHiliteColor (m_hilite);
	
    FXProgressBar* l = new FXProgressBar (VUFrame, &_leftTarget,
    	FXDataTarget::ID_VALUE,
    	PROGRESSBAR_PERCENTAGE | LAYOUT_FILL_X | FRAME_SUNKEN);
	l->setBarColor (FXRGB (0, 200, 0));
    FXProgressBar* r = new FXProgressBar (VUFrame, &_rightTarget,
    	FXDataTarget::ID_VALUE,
    	PROGRESSBAR_PERCENTAGE | LAYOUT_FILL_X | FRAME_SUNKEN);
	r->setBarColor (FXRGB (0, 200, 0));
    FXHorizontalFrame* multiplierFrame = new FXHorizontalFrame (VUFrame,
	   	LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0);
	multiplierFrame->setBorderColor (m_theme.border);
	multiplierFrame->setBaseColor (m_theme.base);
	multiplierFrame->setBackColor (m_theme.base);	
	multiplierFrame->setShadowColor (m_shadow);
	multiplierFrame->setHiliteColor (m_hilite);   	
	   	
	std::stringstream mult;
	mult << (int) _config->number ("Harmonizer", "OutputMultiplier") << "x";
//	_multiValue = new FXLabel (multiplierFrame, mult.str ().c_str (), 0,
//		LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH, 0,0,20,0);		
//	_multiValue->setBorderColor (m_theme.border);
//	_multiValue->setBaseColor (m_theme.base);
//	_multiValue->setBackColor (m_theme.base);
//	_multiValue->setTextColor (m_theme.fore);	
//	_multiValue->setShadowColor (m_shadow);
//	_multiValue->setHiliteColor (m_hilite);
//			
//	FXRealSlider* outputMult = new FXRealSlider (multiplierFrame, this,
//		ID_MULTIPLIER, LAYOUT_CENTER_Y | SLIDER_HORIZONTAL |
//  		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_X);
//	outputMult->setRange (0, 4.0);
//	outputMult->setTickDelta (1);
//	outputMult->setValue (_config->number ("Harmonizer", "OutputMultiplier"));
//	outputMult->setBorderColor (m_theme.border);
//	outputMult->setBaseColor (m_theme.base);
//	outputMult->setBackColor (m_theme.base);
//	outputMult->setShadowColor (m_shadow);
//	outputMult->setHiliteColor (m_hilite);
		
	FXKnob* vmult = new FXKnob (multiplierFrame, this, 
		ID_MULTIPLIER, KNOB_TICKS | LAYOUT_CENTER_Y | LAYOUT_RIGHT);
	vmult->setRange (0, 400);
	vmult->setValue ((int)(_config->number ("Harmonizer", "OutputMultiplier") * 100));		
	vmult->setBorderColor (m_theme.border);
	vmult->setBaseColor (m_theme.base);
	vmult->setBackColor (m_theme.base);
	vmult->setShadowColor (m_shadow);
	vmult->setHiliteColor (m_hilite);
		
	FX7Segment* _multiValue = new FX7Segment (multiplierFrame,
		FXString::null, SEVENSEGMENT_SHADOW | JUSTIFY_RIGHT | LAYOUT_LEFT 
		| LAYOUT_FILL_ROW | LAYOUT_FILL_X);	
	_multiValue->setBorderColor (m_theme.border);
	_multiValue->setBaseColor (m_theme.base);
	_multiValue->setBackColor (m_theme.base);
	_multiValue->setTextColor (m_theme.fore);	
	_multiValue->setShadowColor (m_shadow);
	_multiValue->setHiliteColor (m_hilite);
		
    FXGroupBox* CPU = new FXGroupBox (status,
    	_stringTable->get ("CPULOAD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	CPU->setBorderColor (m_theme.border);
	CPU->setBaseColor (m_theme.base);
	CPU->setBackColor (m_theme.base);
	CPU->setTextColor (m_theme.fore);
	CPU->setShadowColor (m_shadow);
	CPU->setHiliteColor (m_hilite);    	
	
	FXVerticalFrame* CPUFrame = new FXVerticalFrame (CPU,
     	LAYOUT_FILL_Y | LAYOUT_FILL_X,
     	0,0,0,0, 0,0,0,0, 4, 4);
    FXProgressBar* c = new FXProgressBar (CPUFrame, &_CPUTarget,
    	FXDataTarget::ID_VALUE,
    	LAYOUT_FILL_X | PROGRESSBAR_PERCENTAGE
     	| PROGRESSBAR_DIAL);
	c->setBorderColor (m_theme.border);
	c->setBaseColor (m_theme.base);
	c->setBackColor (m_theme.base);
	c->setShadowColor (m_shadow);
	c->setHiliteColor (m_hilite);   
	c->setTextColor (FXRGB (200, 0, 0));
	
    FXGroupBox* freqBox = new FXGroupBox (status,
    	_stringTable->get ("FREQUENCY").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	freqBox->setBorderColor (m_theme.border);
	freqBox->setBaseColor (m_theme.base);
	freqBox->setBackColor (m_theme.base);
	freqBox->setTextColor (m_theme.fore);
	freqBox->setShadowColor (m_shadow);
	freqBox->setHiliteColor (m_hilite);     
	  	
  	_frequencyLabel = new FXLabel (freqBox, "", 0, LAYOUT_FILL_X |
   		JUSTIFY_CENTER_X | FRAME_SUNKEN);
   	_frequencyLabel->setTextColor (FXRGB (255, 255, 0));
  	_frequencyLabel->setBackColor (FXRGB (0, 0, 0));
    FXGroupBox* noteBox = new FXGroupBox (status,
    	_stringTable->get ("NOTENAME").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	noteBox->setBorderColor (m_theme.border);
	noteBox->setBaseColor (m_theme.base);
	noteBox->setBackColor (m_theme.base);
	noteBox->setTextColor (m_theme.fore);	
	noteBox->setShadowColor (m_shadow);
	noteBox->setHiliteColor (m_hilite);    	
  	_noteLabel = new FXLabel (noteBox, "", 0, LAYOUT_FILL_X |
   		JUSTIFY_CENTER_X | FRAME_SUNKEN);
   	_noteLabel->setTextColor (FXRGB (255, 255, 0));
  	_noteLabel->setBackColor (FXRGB (0, 0, 0));
  	
    FXGroupBox* toneBox = new FXGroupBox (status,
    	_stringTable->get ("SCALETONE").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	toneBox->setBorderColor (m_theme.border);
	toneBox->setBaseColor (m_theme.base);
	toneBox->setBackColor (m_theme.base);
	toneBox->setTextColor (m_theme.fore);
	toneBox->setShadowColor (m_shadow);
	toneBox->setHiliteColor (m_hilite);     
	    	
  	_toneLabel = new FXLabel (toneBox, "", 0, LAYOUT_FILL_X |
   		JUSTIFY_CENTER_X | FRAME_SUNKEN);
   	_toneLabel->setTextColor (FXRGB (255, 255, 0));
  	_toneLabel->setBackColor (FXRGB (0, 0, 0));
   	_recordLabel = new FXLabel (status, "");
	_recordLabel->setBackColor (m_theme.base); 	
   	_recordLabel->setTextColor (FXRGB (255, 0, 0));
	FXTabItem* infotab = new FXTabItem (panels, _stringTable->get ("INFOTAB").c_str ());
	infotab->setBorderColor (m_theme.border);
	infotab->setBaseColor (m_theme.base);
	infotab->setBackColor (m_theme.base);
	infotab->setTextColor (m_theme.fore);
	infotab->setShadowColor (m_shadow);
	infotab->setHiliteColor (m_hilite);  
	
    FXVerticalFrame* info = new FXVerticalFrame (panels,
    	FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y |
     	LAYOUT_TOP | LAYOUT_LEFT,
		0,0,0,0, 4,4,4,4, 10, 10);
	info->setBorderColor (m_theme.border);
	info->setBaseColor (m_theme.base);
	info->setBackColor (m_theme.base);
	info->setShadowColor (m_shadow);
	info->setHiliteColor (m_hilite);  		
	
    FXGroupBox* dev = new FXGroupBox (info,
    	_stringTable->get ("DEVICES").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	dev->setBorderColor (m_theme.border);
	dev->setBaseColor (m_theme.base);
	dev->setBackColor (m_theme.base);
	dev->setTextColor (m_theme.fore);
	dev->setShadowColor (m_shadow);
	dev->setHiliteColor (m_hilite);  
    	
	std::stringstream s0;
	s0 << _stringTable->get ("AUDIO") << "--, --";
    _audioDevLabel = new FXLabel (dev, s0.str ().c_str ());
	_audioDevLabel->setBorderColor (m_theme.border);
	_audioDevLabel->setBaseColor (m_theme.base);
	_audioDevLabel->setBackColor (m_theme.base);
	_audioDevLabel->setTextColor (m_theme.fore);
	_audioDevLabel->setShadowColor (m_shadow);
	_audioDevLabel->setHiliteColor (m_hilite); 
	     
    std::stringstream s1;
	s1 << _stringTable->get ("MIDI") << "--, --";
	_MIDIDevLabel = new FXLabel (dev, s1.str ().c_str ());
	_MIDIDevLabel->setBorderColor (m_theme.border);
	_MIDIDevLabel->setBaseColor (m_theme.base);
	_MIDIDevLabel->setBackColor (m_theme.base);
	_MIDIDevLabel->setTextColor (m_theme.fore);
	_MIDIDevLabel->setShadowColor (m_shadow);
	_MIDIDevLabel->setHiliteColor (m_hilite);
	 	
	std::stringstream s2;
	s2 << _stringTable->get ("LATENCY") << "----";
	_engineLatencyLabel = new FXLabel (dev, s2.str ().c_str ());
	_engineLatencyLabel->setBorderColor (m_theme.border);
	_engineLatencyLabel->setBaseColor (m_theme.base);
	_engineLatencyLabel->setBackColor (m_theme.base);
	_engineLatencyLabel->setTextColor (m_theme.fore);
	_engineLatencyLabel->setShadowColor (m_shadow);
	_engineLatencyLabel->setHiliteColor (m_hilite);
		
    FXGroupBox* olapBox = new FXGroupBox (info,
    	_stringTable->get ("OVERLAP").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
 	olapBox->setBorderColor (m_theme.border);
	olapBox->setBaseColor (m_theme.base);
	olapBox->setBackColor (m_theme.base);
	olapBox->setTextColor (m_theme.fore);
	olapBox->setShadowColor (m_shadow);
	olapBox->setHiliteColor (m_hilite);   	
  	_olapLabel = new FXLabel (olapBox, "", 0, LAYOUT_FILL_X |
   		JUSTIFY_CENTER_X | FRAME_SUNKEN);
   	_olapLabel->setTextColor (FXRGB (255, 255, 0));
  	_olapLabel->setBackColor (FXRGB (0, 0, 0));
  	
//  	_harmonUsageLabel = new FXLabel (info, "");
//   	_harmonUsageLabel->setTextColor (FXRGB (0, 0, 127));
//  	_morphUsageLabel = new FXLabel (info, "");
//   	_morphUsageLabel->setTextColor (FXRGB (0, 0, 127));
	FXTabItem* midimontab = new FXTabItem (panels, _stringTable->get ("MIDIMONTAB").c_str ());
	midimontab->setBorderColor (m_theme.border);
	midimontab->setBaseColor (m_theme.base);
	midimontab->setBackColor (m_theme.base);
	midimontab->setTextColor (m_theme.fore);
	midimontab->setShadowColor (m_shadow);
	midimontab->setHiliteColor (m_hilite);
	
    FXVerticalFrame* MIDIScope = new FXVerticalFrame (panels,
    	FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y |
    	LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT,
		0,0,0,0, 4,4,4,4, 10, 10);
	MIDIScope->setBorderColor (m_theme.border);
	MIDIScope->setBaseColor (m_theme.base);
	MIDIScope->setBackColor (m_theme.base);
	MIDIScope->setShadowColor (m_shadow);
	MIDIScope->setHiliteColor (m_hilite);
			
    FXGroupBox* msgBox = new FXGroupBox (MIDIScope, "",
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP);
 	msgBox->setBorderColor (m_theme.border);
	msgBox->setBaseColor (m_theme.base);
	msgBox->setBackColor (m_theme.base);
	msgBox->setShadowColor (m_shadow);
	msgBox->setHiliteColor (m_hilite);   	
    	
    msgList = new FXList (msgBox, NULL, 0,
    	LIST_SINGLESELECT | LIST_NORMAL | FRAME_THICK |
     	LAYOUT_FILL_X | LAYOUT_FILL_Y);
	msgList->setTextColor (FXRGB (255, 255, 0));
	msgList->setBackColor (FXRGB (0, 0, 0));
	msgList->setSelBackColor (FXRGB (127, 127, 127));

	/// MENU ///
	_fileMenu = new FXMenuPane (this);
	_fileMenu->setBorderColor (m_theme.border);
	_fileMenu->setBaseColor (m_theme.base);
	_fileMenu->setBackColor (m_theme.base);
	_fileMenu->setShadowColor (m_shadow);
	_fileMenu->setHiliteColor (m_hilite);	
	
#if 0
	#if !defined (REPITCH_ONLY)
		new FXMenuCommand (_fileMenu, _stringTable->get ("FILEOPENMENU").c_str (),
			app->_openIcon, this, ID_FILEOPEN);
		new FXMenuSeparator (_fileMenu);
	#endif
#endif
	FXMenuCommand* jollymenu = new FXMenuCommand (_fileMenu, _stringTable->get ("FILEDUMPMENU").c_str (),
		NULL, this, ID_FILEDUMP);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);
			
	FXMenuSeparator* jollyseparator = new FXMenuSeparator (_fileMenu);
	jollyseparator->setBackColor (m_theme.base);
	jollyseparator->setShadowColor (m_shadow);
	jollyseparator->setHiliteColor (m_hilite);	
		
	jollymenu = new FXMenuCommand (_fileMenu, _stringTable->get ("FILERESETQUITMENU").c_str (),
		NULL, this, ID_FILERESETQUIT);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);		
	
	jollyseparator= new FXMenuSeparator (_fileMenu);
	jollyseparator->setBackColor (m_theme.base);
	jollyseparator->setShadowColor (m_shadow);
	jollyseparator->setHiliteColor (m_hilite);
		
	jollymenu = new FXMenuCommand (_fileMenu, _stringTable->get ("FILEQUITMENU").c_str (),
		app->_quitIcon, this, ID_FILEQUIT);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);	
			
	FXMenuTitle* jollytitle = new FXMenuTitle (menubar, _stringTable->get ("FILEMENU").c_str (), 0,
		_fileMenu);		
	jollytitle->setBackColor (m_theme.base);
	jollytitle->setSelBackColor (m_theme.base);
	jollytitle->setShadowColor (m_shadow);
	jollytitle->setHiliteColor (m_hilite);		
	
 	_viewMenu = new FXMenuPane (this);
	_viewMenu->setBackColor (m_theme.base);
	_viewMenu->setShadowColor (m_shadow);
	_viewMenu->setHiliteColor (m_hilite);
	 	
    FXMenuCheck* ch1 = new FXMenuCheck (_viewMenu,
    	_stringTable->get ("VIEWTOOLBARMENU").c_str (),
    	_toolBar, FXWindow::ID_TOGGLESHOWN);
	ch1->setBackColor (m_theme.base);
	ch1->setShadowColor (m_shadow);
	ch1->setHiliteColor (m_hilite);
	    
    FXMenuCheck* ch2 = new FXMenuCheck (_viewMenu,
    	_stringTable->get ("VIEWSTATUSBARMENU").c_str (),
    	_statusBar, FXWindow::ID_TOGGLESHOWN);
	ch2->setBackColor (m_theme.base);
	ch2->setShadowColor (m_shadow);
	ch2->setHiliteColor (m_hilite);    	
	jollyseparator = new FXMenuSeparator (_viewMenu);
	jollyseparator->setBackColor (m_theme.base);
	jollyseparator->setShadowColor (m_shadow);
	jollyseparator->setHiliteColor (m_hilite);
		
	FXMenuCheck* ch5 = new FXMenuCheck (_viewMenu,
		_stringTable->get ("VIEWCONTROLPANELMENU").c_str (),
		_bookFrame, FXWindow::ID_TOGGLESHOWN);
	ch5->setBackColor (m_theme.base);
	ch5->setShadowColor (m_shadow);
	ch5->setHiliteColor (m_hilite);
			
	if ((int) _config->number ("Windows", "ToolBarVisible") == 0) {
		_toolBar->hide ();
		ch1->setCheck (false);
	}
	if ((int) _config->number ("Windows", "StatusBarVisible") == 0) {
		_statusBar->hide ();
		ch2->setCheck (false);
	}
	if ((int) _config->number ("Windows", "ControlPanelVisible") == 0) {
		_bookFrame->hide ();
		ch5->setCheck (false);
	}
	jollytitle = new FXMenuTitle (menubar, _stringTable->get ("VIEWMENU").c_str (), 0,
		_viewMenu);
	jollytitle->setBackColor (m_theme.base);
	jollytitle->setSelBackColor (m_theme.base);
	jollytitle->setShadowColor (m_shadow);
	jollytitle->setHiliteColor (m_hilite);	
	
 	_runMenu = new FXMenuPane (this);
 	_runMenu->setBaseColor (m_theme.base);
	_runMenu->setBackColor (m_theme.base);
	_runMenu->setShadowColor (m_shadow);
	_runMenu->setHiliteColor (m_hilite);
	 	
    _ch3 = new FXMenuCheck (_runMenu,
    	_stringTable->get ("RUNPLAYMENU").c_str (),
    	this, ID_RUNPLAY);
	_ch3->setBackColor (m_theme.base);
	_ch3->setShadowColor (m_shadow);
	_ch3->setHiliteColor (m_hilite);
	    	
    _ch6 = new FXMenuCheck (_runMenu,
        	_stringTable->get ("RUNRECORDMENU").c_str (),
    	this, ID_RUNRECORD);
	_ch6->setBackColor (m_theme.base);
	_ch6->setShadowColor (m_shadow);
	_ch6->setHiliteColor (m_hilite);
	    	
	new FXMenuSeparator (_runMenu);
    _ch4 = new FXMenuCheck (_runMenu,
    	_stringTable->get ("RUNENABLEMENU").c_str (),
    	this, ID_RUNENABLE);
	_ch4->setBackColor (m_theme.base);
	_ch4->setShadowColor (m_shadow);
	_ch4->setHiliteColor (m_hilite);
		
	jollytitle = new FXMenuTitle (menubar, _stringTable->get ("RUNMENU").c_str (), 0,
		_runMenu);
	jollytitle->setBackColor (m_theme.base);
	jollytitle->setSelBackColor (m_theme.base);
	jollytitle->setShadowColor (m_shadow);
	jollytitle->setHiliteColor (m_hilite);	
	
	_toolsMenu = new FXMenuPane (this);
	_toolsMenu->setBackColor (m_theme.base);
	_toolsMenu->setShadowColor (m_shadow);
	_toolsMenu->setHiliteColor (m_hilite);
		
	_MIDIMonSubMenu = new FXMenuPane (this);
	_MIDIMonSubMenu->setBackColor (m_theme.base);
	_MIDIMonSubMenu->setShadowColor (m_shadow);
	_MIDIMonSubMenu->setHiliteColor (m_hilite);
		
	jollymenu = new FXMenuCommand (_MIDIMonSubMenu,
 		_stringTable->get ("TOOLSMIDIMONSAVEMENU").c_str (),
		NULL, this, ID_TOOLSSAVEMON);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);	
			
	jollymenu = new FXMenuCommand (_MIDIMonSubMenu,
 		_stringTable->get ("TOOLSMIDIMONCLEARMENU").c_str (),
		NULL, this, ID_TOOLSCLEARMON);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);	
	
	FXMenuCascade* jollycascade = new FXMenuCascade (_toolsMenu,
 		_stringTable->get ("TOOLSMIDIMONMENU").c_str (),
 		NULL, _MIDIMonSubMenu);
	jollycascade->setBackColor (m_theme.base);
	jollycascade->setShadowColor (m_shadow);
	jollycascade->setHiliteColor (m_hilite);
	 		
	jollyseparator = new FXMenuSeparator (_toolsMenu);
	jollycascade->setBackColor (m_theme.base);
	jollycascade->setShadowColor (m_shadow);
	jollycascade->setHiliteColor (m_hilite);
	
	#ifdef _WIN32
		jollymenu = new FXMenuCommand (_toolsMenu,
  			_stringTable->get ("TOOLSSNDVOLMENU").c_str (),
			0, this, ID_TOOLSSNDVOL);
		jollymenu->setBackColor (m_theme.base);
		jollymenu->setShadowColor (m_shadow);
		jollymenu->setHiliteColor (m_hilite);				
	#endif
	jollymenu = new FXMenuCommand (_toolsMenu,
 		_stringTable->get ("TOOLSSETTINGSMENU").c_str (),
		app->_settingsIcon, this, ID_TOOLSSETTINGS);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);	
			
	jollytitle = new FXMenuTitle (menubar, _stringTable->get ("TOOLSMENU").c_str (), 0,
		_toolsMenu);
	jollytitle->setBackColor (m_theme.base);
	jollytitle->setSelBackColor (m_theme.base);
	jollytitle->setShadowColor (m_shadow);
	jollytitle->setHiliteColor (m_hilite);	
	
	_helpMenu = new FXMenuPane (this);
	_helpMenu->setBackColor (m_theme.base);
	_helpMenu->setShadowColor (m_shadow);
	_helpMenu->setHiliteColor (m_hilite);
		
	jollymenu = new FXMenuCommand (_helpMenu,
		_stringTable->get ("HELPCONTENTSMENU").c_str (), app->_contentsIcon,
		this, ID_HELPCONTENTS);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);
			
//#if defined (REPITCH_ONLY)
//	FXMenuCommand* phelp =
//#endif
//	new FXMenuCommand (_helpMenu,
//		_stringTable->get ("HELPPROTONHELPMENU").c_str (), 0,
//		this, ID_HELPPROTON);
//#if defined (REPITCH_ONLY)
//	phelp->hide ();
//#endif
	jollyseparator = new FXMenuSeparator (_helpMenu);
	jollyseparator->setBackColor (m_theme.base);
	jollyseparator->setShadowColor (m_shadow);
	jollyseparator->setHiliteColor (m_hilite);
	
	jollymenu = new FXMenuCommand (_helpMenu,
		_stringTable->get ("HELPWEBMENU").c_str (), 0,
		this, ID_HELPWEB);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);
			
	jollymenu = new FXMenuCommand (_helpMenu,
		_stringTable->get ("HELPMAILMENU").c_str (), 0,
		this, ID_HELPMAIL);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);
			
	jollyseparator = new FXMenuSeparator (_helpMenu);
	jollyseparator->setBackColor (m_theme.base);
	jollyseparator->setShadowColor (m_shadow);
	jollyseparator->setHiliteColor (m_hilite);
		
	jollymenu = new FXMenuCommand (_helpMenu,
		_stringTable->get ("HELPABOUTMENU").c_str (), 0,
		this, ID_HELPABOUT);
	jollymenu->setBackColor (m_theme.base);
	jollymenu->setShadowColor (m_shadow);
	jollymenu->setHiliteColor (m_hilite);
			
	jollytitle = new FXMenuTitle (menubar, _stringTable->get ("HELPMENU").c_str (), 0,
 		_helpMenu,
		LAYOUT_RIGHT);
	jollytitle->setBackColor (m_theme.base);
	jollytitle->setSelBackColor (m_theme.base);
	jollytitle->setShadowColor (m_shadow);
	jollytitle->setHiliteColor (m_hilite);	
	
	/// TOOLBAR ///
	FXToolBarGrip* tbgrip0 = new FXToolBarGrip (_toolBar, _toolBar, FXToolBar::ID_TOOLBARGRIP,
		TOOLBARGRIP_DOUBLE);
	tbgrip0->setBorderColor (m_theme.border);
	tbgrip0->setBaseColor (m_theme.base);
	tbgrip0->setBackColor (m_theme.base);
	tbgrip0->setShadowColor (m_shadow);
	tbgrip0->setHiliteColor (m_hilite);   		
#if 0
	#if !defined (REPITCH_ONLY)
		new FXButton (_toolBar, _stringTable->get ("FILEOPENBUTTON").c_str (),
			app->_openIcon, this, MainWindow::ID_FILEOPEN, BUTTON_TOOLBAR |
	  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
		new FXToolBarGrip (_toolBar, NULL, 0, TOOLBARGRIP_SEPARATOR);
	#endif
#endif
  	_playButton = new FXToggleButton (_toolBar,
   		_stringTable->get ("PLAYSTOPBUTTON").c_str (),
		(FXString) "", app->_playIcon, app->_stopIcon, this, MainWindow::ID_RUNPLAY,
  		BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
	_playButton->setBorderColor (m_theme.border);
	_playButton->setBaseColor (m_theme.base);
	_playButton->setBackColor (m_theme.base);
	_playButton->setTextColor (m_theme.fore);
	_playButton->setShadowColor (m_shadow);
	_playButton->setHiliteColor (m_hilite);    		
  	_recordButton = new FXToggleButton (_toolBar,
   		_stringTable->get ("RECSTOPBUTTON").c_str (),
		(FXString) "", app->_recIcon, app->_stopRecIcon, this, MainWindow::ID_RUNRECORD,
  		BUTTON_TOOLBAR |
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
	_recordButton->setBorderColor (m_theme.border);
	_recordButton->setBaseColor (m_theme.base);
	_recordButton->setBackColor (m_theme.base);
	_recordButton->setTextColor (m_theme.fore);
	_recordButton->setShadowColor (m_shadow);
	_recordButton->setHiliteColor (m_hilite);  		
	
	FXToolBarGrip* tbgrip1 = new FXToolBarGrip (_toolBar, NULL, 0, TOOLBARGRIP_SINGLE);
	tbgrip1->setBorderColor (m_theme.border);
	tbgrip1->setBaseColor (m_theme.base);
	tbgrip1->setBackColor (m_theme.base);
	tbgrip1->setShadowColor (m_shadow);
	tbgrip1->setHiliteColor (m_hilite);
	  	
  	_enableButton = new FXToggleButton (_toolBar,
   		_stringTable->get ("ENABLEDISABLEBUTTON").c_str (),
		(FXString) "", app->_MIDIIcon, app->_enableIcon, this, MainWindow::ID_RUNENABLE,
  		BUTTON_TOOLBAR |
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
	_enableButton->setBorderColor (m_theme.border);
	_enableButton->setBaseColor (m_theme.base);
	_enableButton->setBackColor (m_theme.base);
	_enableButton->setTextColor (m_theme.fore);
	_enableButton->setShadowColor (m_shadow);
	_enableButton->setHiliteColor (m_hilite);  		
	
	FXButton* tbsettings = new FXButton (_toolBar, _stringTable->get ("TOOLSSETTINGSBUTTON").c_str (),
		app->_settingsIcon, this, MainWindow::ID_TOOLSSETTINGS, BUTTON_TOOLBAR |
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
	tbsettings->setBorderColor (m_theme.border);
	tbsettings->setBaseColor (m_theme.base);
	tbsettings->setBackColor (m_theme.base);
	tbsettings->setTextColor (m_theme.fore);
	tbsettings->setShadowColor (m_shadow);
	tbsettings->setHiliteColor (m_hilite);
	  
	FXToolBarGrip* tbgrip2 = new FXToolBarGrip (_toolBar, NULL, 0, TOOLBARGRIP_SINGLE);
	tbgrip2->setBorderColor (m_theme.border);
	tbgrip2->setBaseColor (m_theme.base);
	tbgrip2->setBackColor (m_theme.base);
	tbgrip2->setShadowColor (m_shadow);
	tbgrip2->setHiliteColor (m_hilite);
		
	FXButton* tbhelp = new FXButton (_toolBar, _stringTable->get ("HELPCONTENTSBUTTON").c_str (),
		app->_contentsIcon, this, MainWindow::ID_HELPCONTENTS, BUTTON_TOOLBAR |
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
	tbhelp->setBorderColor (m_theme.border);
	tbhelp->setBaseColor (m_theme.base);
	tbhelp->setBackColor (m_theme.base);
	tbhelp->setTextColor (m_theme.fore);
	tbhelp->setShadowColor (m_shadow);
	tbhelp->setHiliteColor (m_hilite);
	  		
#ifdef OPTIMIZED
  	FXLabel* buildLabel = new FXLabel (_statusBar,
  		_stringTable->get ("OPTIMIZED").c_str (), 0,
  		LAYOUT_LEFT | LAYOUT_CENTER_Y);
	buildLabel->setBorderColor (m_theme.border);
	buildLabel->setBaseColor (m_theme.base);
	buildLabel->setBackColor (m_theme.base);
	buildLabel->setTextColor (m_theme.fore);
	buildLabel->setShadowColor (m_shadow);
	buildLabel->setHiliteColor (m_hilite);  		
#else
  	FXLabel* buildLabel = new FXLabel (_statusBar,
  		_stringTable->get ("DEBUG").c_str (), 0,
  		LAYOUT_LEFT | LAYOUT_CENTER_Y);
	buildLabel->setBorderColor (m_theme.border);
	buildLabel->setBaseColor (m_theme.base);
	buildLabel->setBackColor (m_theme.base);
	buildLabel->setTextColor (m_theme.fore);
	buildLabel->setShadowColor (m_shadow);
	buildLabel->setHiliteColor (m_hilite);   		
#endif
	FXButton* helpstatusbutt = new FXButton (_statusBar, _stringTable->get ("HELPABOUTBUTTON").c_str (),
		app->_smallIcon, this, MainWindow::ID_HELPABOUT,
  		BUTTON_TOOLBAR | FRAME_RAISED
  		| LAYOUT_TOP | LAYOUT_LEFT);
	helpstatusbutt->setBorderColor (m_theme.border);
	helpstatusbutt->setBaseColor (m_theme.base);
	helpstatusbutt->setBackColor (m_theme.base);
	helpstatusbutt->setTextColor (m_theme.fore);
	helpstatusbutt->setShadowColor (m_shadow);
	helpstatusbutt->setHiliteColor (m_hilite);   
	
	app->addTimeout ((FXObject*) this, ID_TIMER, TIMEOUT, 0);

	//////////////////////////////////////////////// *** reVoice GUI drawing ***
    _titleFont = new FXFont (getApp (), "Verdana", 12, FXFont::ExtraBold,
    	FXFont::Italic);

	_olapLabel->setFont (_titleFont);
	_frequencyLabel->setFont (_titleFont);
	_noteLabel->setFont (_titleFont);
	_toneLabel->setFont (_titleFont);

 	// interior...
    FXHorizontalFrame* switcherFrame = new FXHorizontalFrame (box,
    	LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	switcherFrame->setBorderColor (m_theme.border);
	switcherFrame->setBaseColor (m_theme.base);
	switcherFrame->setBackColor (m_theme.base);
	switcherFrame->setShadowColor (m_shadow);
	switcherFrame->setHiliteColor (m_hilite);   
	   	
    FXVerticalFrame* buttons = new FXVerticalFrame (switcherFrame,
    	LAYOUT_LEFT | LAYOUT_FILL_Y | FRAME_SUNKEN | PACK_UNIFORM_WIDTH |
     	PACK_UNIFORM_HEIGHT, 0,0,0,0, 0,0,0,0, 0,0);
	buttons->setBorderColor (m_theme.border);
	buttons->setBaseColor (m_theme.base);
	buttons->setBackColor (m_theme.base);
	buttons->setShadowColor (m_shadow);
	buttons->setHiliteColor (m_hilite);   
	    	
    FXSwitcher* switcher = new FXSwitcher (switcherFrame,
    	LAYOUT_FILL_X | LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	switcher->setBorderColor (m_theme.border);
	switcher->setBaseColor (m_theme.base);
	switcher->setBackColor (m_theme.base);
	switcher->setShadowColor (m_shadow);
	switcher->setHiliteColor (m_hilite); 
	
	// harmon...
	FXGroupBox* label1 = new FXGroupBox (switcher,
 		_stringTable->get ("HARMONIZER").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	label1->setFont (_titleFont);
	label1->setBorderColor (m_theme.border);
	label1->setBaseColor (m_theme.base);
	label1->setBackColor (m_theme.base);
	label1->setTextColor (m_theme.fore);
	label1->setShadowColor (m_shadow);
	label1->setHiliteColor (m_hilite); 
		
	FXHorizontalFrame* matrix1 = new FXHorizontalFrame (label1,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	matrix1->setBorderColor (m_theme.border);
	matrix1->setBaseColor (m_theme.base);
	matrix1->setBackColor (m_theme.base);
	matrix1->setShadowColor (m_shadow);
	matrix1->setHiliteColor (m_hilite);  		
//	FXGroupBox* voiceBox = new FXGroupBox (matrix1,
// 		_stringTable->get ("VOICES").c_str (),
//		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
//  		0,0,150,0);
//    _voicePop = new FXPopup (this);
//    new FXOption (_voicePop, _stringTable->get ("VOICEMENU1").c_str (), NULL,
//    	this,
//    	ID_VOICEOPTION1, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
//    new FXOption (_voicePop, _stringTable->get ("VOICEMENU2").c_str (), NULL,
//    	this,
//    	ID_VOICEOPTION2, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
//    new FXOption (_voicePop, _stringTable->get ("VOICEMENU3").c_str (), NULL,
//    	this,
//    	ID_VOICEOPTION3, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
//    new FXOption (_voicePop,_stringTable->get ("VOICEMENU4").c_str (), NULL,
//    	this,
//    	ID_VOICEOPTION4, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
//    FXOptionMenu* optionMenu = new FXOptionMenu (voiceBox, _voicePop,
//    	LAYOUT_TOP | FRAME_RAISED | LAYOUT_FILL_X |
//    	FRAME_THICK | JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
//	optionMenu->setCurrentNo ((int)
// 		_config->number ("Harmonizer", "ActiveVoices") - 1,	true);
	FXGroupBox* waveformBox = new FXGroupBox (matrix1,
 		_stringTable->get ("WAVEFORM").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
	waveformBox->setBorderColor (m_theme.border);
	waveformBox->setBaseColor (m_theme.base);
	waveformBox->setBackColor (m_theme.base);
	waveformBox->setTextColor (m_theme.fore);
	waveformBox->setShadowColor (m_shadow);
	waveformBox->setHiliteColor (m_hilite);    		
#if defined (REPITCH_ONLY)
	waveformBox->hide ();
#endif
	FXHorizontalFrame* waveformFrame = new FXHorizontalFrame (waveformBox,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT,
     	0,0,0,0, 0,0,0,0, 4, 4);
	waveformFrame->setBorderColor (m_theme.border);
	waveformFrame->setBaseColor (m_theme.base);
	waveformFrame->setBackColor (m_theme.base);
	waveformFrame->setShadowColor (m_shadow);
	waveformFrame->setHiliteColor (m_hilite);       	
	FXButton* jollybutton = new FXButton (waveformFrame, _stringTable->get ("PANICBUTTON").c_str (),
		app->_panicIcon, this, ID_PANIC,
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT); // | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT, 0,0,16,16);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite);    	
	  		
	jollybutton = new FXButton (waveformFrame, _stringTable->get ("PHIRESETBUTTON").c_str (),
		app->_phiIcon, this, ID_PHIRESET,
  		FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT); // | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT, 0,0,16,16);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite); 
	  		
    FXListBox* waveformCombo = new FXListBox (waveformFrame, this,
    	ID_WAVEFORM, LAYOUT_FILL_X |
    	LISTBOX_NORMAL | FRAME_SUNKEN | FRAME_THICK, 0,0,0,0);
    waveformCombo->setNumVisible (5);
	waveformCombo->appendItem (_stringTable->get ("SINE").c_str (),
 		getApp ()->_sineIcon);
	waveformCombo->appendItem (_stringTable->get ("SQUARE").c_str (),
 		getApp ()->_squareIcon);
	waveformCombo->appendItem (_stringTable->get ("FROMFILE").c_str (),
 		getApp ()->_audioIcon);
	waveformCombo->appendItem (_stringTable->get ("LIVEINPUT").c_str (),
		getApp ()->_liveIcon);
	waveformCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "Waveform"));
	waveformCombo->setBorderColor (m_theme.border);
	waveformCombo->setBaseColor (m_theme.base);
	waveformCombo->setBackColor (FXRGB (255, 255, 255)); //m_theme.base);
	waveformCombo->setTextColor (FXRGB (0, 0, 0)); //m_theme.fore);
	waveformCombo->setShadowColor (m_shadow);
	waveformCombo->setHiliteColor (m_hilite); 
			
    FXGroupBox* fileBox = new FXGroupBox (matrix1,
    	_stringTable->get ("INPUTPCM").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	fileBox->setBorderColor (m_theme.border);
	fileBox->setBaseColor (m_theme.base);
	fileBox->setBackColor (m_theme.base);
	fileBox->setTextColor (m_theme.fore);
	fileBox->setShadowColor (m_shadow);
	fileBox->setHiliteColor (m_hilite);    
	 	
	FXHorizontalFrame* fileFrame = new FXHorizontalFrame (fileBox,
 		LAYOUT_FILL_X,
     	0,0,0,0, 0,0,0,0, 4, 4);
	fileFrame->setBorderColor (m_theme.border);
	fileFrame->setBaseColor (m_theme.base);
	fileFrame->setBackColor (m_theme.base);
	fileFrame->setShadowColor (m_shadow);
	fileFrame->setHiliteColor (m_hilite);  
	    	
    _fileButton = new FXButton (fileFrame,
    	_stringTable->get ("BROWSEBUTTON").c_str (), NULL,
    	this, ID_BROWSE,
    	LAYOUT_LEFT | FRAME_RAISED, 0,0,0,0, 20,20);
	_fileButton->setBorderColor (m_theme.border);
	_fileButton->setBaseColor (m_theme.base);
	_fileButton->setBackColor (m_theme.base);
	_fileButton->setTextColor (m_theme.fore);
	_fileButton->setShadowColor (m_shadow);
	_fileButton->setHiliteColor (m_hilite);
	    	
	_fileField = new FXTextField (fileFrame, 0, NULL, 0,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FILL_X | TEXTFIELD_READONLY,
		0,0,0,0, 4,4,4,4);
	_fileField->setText (_config->literal ("Harmonizer", "InputFile").c_str ());
  	if ((int) _config->number ("Harmonizer", "Waveform") != 2) { // audio file
  		_fileButton->disable ();
    	_fileField->disable ();
   	}
#if defined (REPITCH_ONLY)
  		_fileButton->enable ();
    	_fileField->enable ();
#endif
	FXHorizontalFrame* matrix2 = new FXHorizontalFrame (label1,
 		LAYOUT_FILL_X);
	matrix2->setBorderColor (m_theme.border);
	matrix2->setBaseColor (m_theme.base);
	matrix2->setBackColor (m_theme.base);
	matrix2->setShadowColor (m_shadow);
	matrix2->setHiliteColor (m_hilite); 		
	FXGroupBox* envelopeBox = new FXGroupBox (matrix2,
 		_stringTable->get ("ENVELOPE").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y,
  		0,0,200,0);
	envelopeBox->setBorderColor (m_theme.border);
	envelopeBox->setBaseColor (m_theme.base);
	envelopeBox->setBackColor (m_theme.base);
	envelopeBox->setTextColor (m_theme.fore);
	envelopeBox->setShadowColor (m_shadow);
	envelopeBox->setHiliteColor (m_hilite);
	    	  		
    FXComboBox* envelopeCombo = new FXComboBox (envelopeBox, 0, this,
    	ID_ENVELOPE,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X | LAYOUT_CENTER_Y);
    envelopeCombo->setNumVisible (5);
    envelopeCombo->appendItem (_stringTable->get ("RECTANGLE").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("BARTLETT").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("GAUSSIAN").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("HAMMING").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("HANNING").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("BLACKMAN").c_str ());
    envelopeCombo->appendItem (_stringTable->get ("BLACKMANHARRIS").c_str ());
 	envelopeCombo->setCurrentItem ((int)
 		_config->number ("Harmonizer", "WindowType"));
	envelopeCombo->setBorderColor (m_theme.border);
	envelopeCombo->setBaseColor (m_theme.base);
	envelopeCombo->setBackColor (FXRGB (255, 255, 255)); //m_theme.base);
	envelopeCombo->setTextColor (FXRGB (0, 0, 0)); //m_theme.fore);
	envelopeCombo->setShadowColor (m_shadow);
	envelopeCombo->setHiliteColor (m_hilite);
	 		
	FXGroupBox* adBox = new FXGroupBox (matrix2,
 		_stringTable->get ("ATTACKDECAY").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X,
  		0,0,150,0);
	adBox->setBorderColor (m_theme.border);
	adBox->setBaseColor (m_theme.base);
	adBox->setBackColor (m_theme.base);
	adBox->setTextColor (m_theme.fore);
	adBox->setShadowColor (m_shadow);
	adBox->setHiliteColor (m_hilite);  	
		
	FXVerticalFrame* attackDecayFrame = new FXVerticalFrame (adBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	attackDecayFrame->setBorderColor (m_theme.border);
	attackDecayFrame->setBaseColor (m_theme.base);
	attackDecayFrame->setBackColor (m_theme.base);
	attackDecayFrame->setShadowColor (m_shadow);
	attackDecayFrame->setHiliteColor (m_hilite);  	
			
	FXHorizontalFrame* attackFrame = new FXHorizontalFrame (attackDecayFrame,
		LAYOUT_FILL_X);
	attackFrame->setBorderColor (m_theme.border);
	attackFrame->setBaseColor (m_theme.base);
	attackFrame->setBackColor (m_theme.base);
	attackFrame->setShadowColor (m_shadow);
	attackFrame->setHiliteColor (m_hilite);		
 	_attackProgress = new FXProgressBar (attackFrame, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
    FXSlider* attackSlider = new FXSlider (attackFrame, this, ID_ATTACK,
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 100);
	attackSlider->setRange (0, 50);
	_attackProgress->setProgress ((int) _config->number ("Harmonizer", "Attack"));
	attackSlider->setValue ((int) _config->number ("Harmonizer", "Attack"));
	attackSlider->setBorderColor (m_theme.border);
	attackSlider->setBaseColor (m_theme.base);
	attackSlider->setBackColor (m_theme.base);
	attackSlider->setShadowColor (m_shadow);
	attackSlider->setHiliteColor (m_hilite);  		
	
	FXHorizontalFrame* decayFrame = new FXHorizontalFrame (attackDecayFrame,
		LAYOUT_FILL_X);
	decayFrame->setBorderColor (m_theme.border);
	decayFrame->setBaseColor (m_theme.base);
	decayFrame->setBackColor (m_theme.base);
	decayFrame->setShadowColor (m_shadow);
	decayFrame->setHiliteColor (m_hilite); 
			
 	_decayProgress = new FXProgressBar (decayFrame, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
    FXSlider* decaySlider = new FXSlider (decayFrame, this, ID_DECAY,
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 100);
	decaySlider->setRange (0, 50);
	_decayProgress->setProgress ((int) _config->number ("Harmonizer", "Decay"));
	decaySlider->setValue ((int) _config->number ("Harmonizer", "Decay"));
	decaySlider->setBorderColor (m_theme.border);
	decaySlider->setBaseColor (m_theme.base);
	decaySlider->setBackColor (m_theme.base);
	decaySlider->setShadowColor (m_shadow);
	decaySlider->setHiliteColor (m_hilite);  	
	
	FXHorizontalFrame* matrix3 = new FXHorizontalFrame (label1,
		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	matrix3->setBorderColor (m_theme.border);
	matrix3->setBaseColor (m_theme.base);
	matrix3->setBackColor (m_theme.base);
	matrix3->setShadowColor (m_shadow);
	matrix3->setHiliteColor (m_hilite);  
			
	FXGroupBox* pdetBox = new FXGroupBox (matrix3,
 		_stringTable->get ("PDET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_Y |
  		LAYOUT_FIX_WIDTH,
  		0,0,200,0);
	pdetBox->setBorderColor (m_theme.border);
	pdetBox->setBaseColor (m_theme.base);
	pdetBox->setBackColor (m_theme.base);
	pdetBox->setTextColor (m_theme.fore);
	pdetBox->setShadowColor (m_shadow);
	pdetBox->setHiliteColor (m_hilite);  		
#if defined (REPITCH_ONLY)
	pdetBox->hide ();
#endif
    _pdetPop = new FXPopup (this);
	_pdetPop->setBorderColor (m_theme.border);
	_pdetPop->setBaseColor (m_theme.base);
	_pdetPop->setBackColor (m_theme.base);
	_pdetPop->setShadowColor (m_shadow);
	_pdetPop->setHiliteColor (m_hilite);
	    
    FXOption* jollyoption = new FXOption (_pdetPop, _stringTable->get ("PERIODESTIMATOR").c_str (), NULL,
    	this,
    	ID_PDET1, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
	jollyoption->setBorderColor (m_theme.border);
	jollyoption->setBaseColor (m_theme.base);
	jollyoption->setBackColor (m_theme.base);
	jollyoption->setTextColor (m_theme.fore);
	jollyoption->setShadowColor (m_shadow);
	jollyoption->setHiliteColor (m_hilite);
	    	
    jollyoption = new FXOption (_pdetPop, _stringTable->get ("AUTOTUNER").c_str (), NULL,
    	this,
    	ID_PDET2, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
	jollyoption->setBorderColor (m_theme.border);
	jollyoption->setBaseColor (m_theme.base);
	jollyoption->setBackColor (m_theme.base);
	jollyoption->setTextColor (m_theme.fore);
	jollyoption->setShadowColor (m_shadow);
	jollyoption->setHiliteColor (m_hilite);
	    	
    jollyoption = new FXOption (_pdetPop, _stringTable->get ("SPECTRALESTIMATOR").c_str (), NULL,
    	this,
    	ID_PDET3, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
	jollyoption->setBorderColor (m_theme.border);
	jollyoption->setBaseColor (m_theme.base);
	jollyoption->setBackColor (m_theme.base);
	jollyoption->setTextColor (m_theme.fore);
	jollyoption->setShadowColor (m_shadow);
	jollyoption->setHiliteColor (m_hilite);
	  	
    FXOptionMenu* optionMenu = new FXOptionMenu (pdetBox, _pdetPop,
    	LAYOUT_TOP | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_CENTER_Y |
    	FRAME_THICK | JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
	optionMenu->setCurrentNo ((int)
 		_config->number ("Harmonizer", "PitchDetector"), true);
	optionMenu->setBorderColor (m_theme.border);
	optionMenu->setBaseColor (m_theme.base);
	optionMenu->setBackColor (m_theme.base);
	optionMenu->setTextColor (m_theme.fore);
	optionMenu->setShadowColor (m_shadow);
	optionMenu->setHiliteColor (m_hilite);
	 		
 //    FXComboBox* pdetCombo = new FXComboBox (pdetBox, 0, this,
//    	ID_PDET,
//    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
//     	LAYOUT_FILL_X | LAYOUT_CENTER_Y);
//    pdetCombo->setNumVisible (3);
//    pdetCombo->appendItem (_stringTable->get ("PERIODESTIMATOR").c_str ());
//    pdetCombo->appendItem (_stringTable->get ("AUTOTUNER").c_str ());
// 	pdetCombo->setCurrentItem ((int)
// 		_config->number ("Harmonizer", "PitchDetector"));
    FXGroupBox* freqThreshBox = new FXGroupBox (matrix3,
    	_stringTable->get ("FREQUENCYTHRESHOLD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	freqThreshBox->setBorderColor (m_theme.border);
	freqThreshBox->setBaseColor (m_theme.base);
	freqThreshBox->setBackColor (m_theme.base);
	freqThreshBox->setShadowColor (m_shadow);
	freqThreshBox->setHiliteColor (m_hilite);
	  	    	
	FXHorizontalFrame* freqThreshFrame = new FXHorizontalFrame (freqThreshBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	freqThreshFrame->setBorderColor (m_theme.border);
	freqThreshFrame->setBaseColor (m_theme.base);
	freqThreshFrame->setBackColor (m_theme.base);
	freqThreshFrame->setShadowColor (m_shadow);
	freqThreshFrame->setHiliteColor (m_hilite);
	  			
    FXComboBox* semitonesThreshCombo = new FXComboBox (freqThreshFrame, 0, this,
    	ID_SEMITONESTHRESH,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y, 0,0,150,0);
    semitonesThreshCombo->setNumVisible (7);
    semitonesThreshCombo->appendItem (_stringTable->get ("STUNISON").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMINORSECOND").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMAJORSECOND").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMINORTHIRD").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMAJORTHIRD").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STPERFECTFOURTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STTRITONE").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STPERFECTFIFTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMINORSIXTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMAJORSIXTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMINORSEVENTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STMAJORSEVENTH").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STOCTAVE").c_str ());
    semitonesThreshCombo->appendItem (_stringTable->get ("STSMOOTH").c_str ());
 	semitonesThreshCombo->setCurrentItem ((int)
 		_config->number ("Harmonizer", "SemitonesThresh"));
	semitonesThreshCombo->setBorderColor (m_theme.border);
	semitonesThreshCombo->setBaseColor (m_theme.base);
	semitonesThreshCombo->setBackColor (FXRGB (255, 255, 255)); //m_theme.base);
	semitonesThreshCombo->setTextColor (FXRGB (0, 0, 0)); //m_theme.fore);
	semitonesThreshCombo->setShadowColor (m_shadow);
	semitonesThreshCombo->setHiliteColor (m_hilite);
	 		
	_freqThreshField = new FXTextField (freqThreshFrame, 0, this,
  		ID_FREQTHRESHOLDTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,30,0, 4,4,4,4);
	std::stringstream frth;
	frth << _config->number ("Harmonizer",
		"FreqThreshold");
	_freqThreshField->setText (frth.str ().c_str ());
  	_freqThreshSlider = new FXRealSlider (freqThreshFrame, this,
    	ID_FREQTHRESHOLD, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_freqThreshSlider->setRange (1, 2);
	_freqThreshSlider->setValue (_config->number ("Harmonizer",
		"FreqThreshold"));
	if ((int) _config->number ("Harmonizer", "SemitonesThresh") == 13) {
 		_freqThreshSlider->enable ();
 		_freqThreshField->enable ();
 	}
 	_freqThreshSlider->setBorderColor (m_theme.border);
	_freqThreshSlider->setBaseColor (m_theme.base);
	_freqThreshSlider->setBackColor (m_theme.base);
	_freqThreshSlider->setShadowColor (m_shadow);
	_freqThreshSlider->setHiliteColor (m_hilite);
	
	FXHorizontalFrame* matrix4 = new FXHorizontalFrame (label1,
 		LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_HEIGHT);
	matrix4->setBorderColor (m_theme.border);
	matrix4->setBaseColor (m_theme.base);
	matrix4->setBackColor (m_theme.base);
	matrix4->setShadowColor (m_shadow);
	matrix4->setHiliteColor (m_hilite);
	 		 		
 	FXGroupBox* fundamentalBox = new FXGroupBox (matrix4,
 		_stringTable->get ("FUNDAMENTAL").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FIX_WIDTH,0,0,200,0);
	fundamentalBox->setBorderColor (m_theme.border);
	fundamentalBox->setBaseColor (m_theme.base);
	fundamentalBox->setBackColor (m_theme.base);
	fundamentalBox->setShadowColor (m_shadow);
	fundamentalBox->setHiliteColor (m_hilite);
			
    FXHorizontalFrame* midiFrame = new FXHorizontalFrame (fundamentalBox,
    	LAYOUT_FILL_X);
	midiFrame->setBorderColor (m_theme.border);
	midiFrame->setBaseColor (m_theme.base);
	midiFrame->setBackColor (m_theme.base);
	midiFrame->setShadowColor (m_shadow);
	midiFrame->setHiliteColor (m_hilite);
	    	
    _radioButtons[0] = new FXRadioButton (midiFrame,
    	_stringTable->get ("FOLLOWMIDI").c_str (), this,
    	ID_RADIO1);
	_radioButtons[0] ->setBorderColor (m_theme.border);
	_radioButtons[0] ->setBaseColor (m_theme.base);
	_radioButtons[0] ->setBackColor (m_theme.base);
	_radioButtons[0] ->setTextColor (m_theme.fore);
	_radioButtons[0] ->setShadowColor (m_shadow);
	_radioButtons[0] ->setHiliteColor (m_hilite);
    	
    FXHorizontalFrame* transposeFrame = new FXHorizontalFrame (fundamentalBox,
    	 LAYOUT_FILL_X);
	transposeFrame->setBorderColor (m_theme.border);
	transposeFrame->setBaseColor (m_theme.base);
	transposeFrame->setBackColor (m_theme.base);
	transposeFrame->setShadowColor (m_shadow);
	transposeFrame->setHiliteColor (m_hilite);
	    	 
    _radioButtons[1] = new FXRadioButton (transposeFrame,
    	_stringTable->get ("TRANSPOSE").c_str (), this,
    	ID_RADIO2);
	_radioButtons[1] ->setBorderColor (m_theme.border);
	_radioButtons[1] ->setBaseColor (m_theme.base);
	_radioButtons[1] ->setBackColor (m_theme.base);
	_radioButtons[1] ->setTextColor (m_theme.fore);
	_radioButtons[1] ->setShadowColor (m_shadow);
	_radioButtons[1] ->setHiliteColor (m_hilite);
	    	
    _transposeCombo = new FXComboBox (transposeFrame, 0, this,
    	ID_INTERVAL,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
 	 	LAYOUT_FILL_X);
    _transposeCombo->setNumVisible (10);
 	for (int i = 0; i < 49; i++) {
    	int t = i - 24;
    	std::stringstream s;
     	s << t << " " << _stringTable->get ("SEMITONES");
      	_transposeCombo->appendItem (s.str ().c_str ());
  	}
  	_transposeCombo->setCurrentItem ((int)
  		_config->number ("Harmonizer", "Semitones"));
	_transposeCombo->setBorderColor (m_theme.border);
	_transposeCombo->setBaseColor (m_theme.base);
	_transposeCombo->setBackColor (FXRGB (255, 255, 255)); //m_theme.base);
	_transposeCombo->setTextColor (FXRGB (0, 0, 0)); //m_theme.fore);
	_transposeCombo->setShadowColor (m_shadow);
	_transposeCombo->setHiliteColor (m_hilite);
	  		
    FXHorizontalFrame* manualFrame = new FXHorizontalFrame (fundamentalBox,
		PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X);
	manualFrame->setBorderColor (m_theme.border);
	manualFrame->setBaseColor (m_theme.base);
	manualFrame->setBackColor (m_theme.base);
	manualFrame->setShadowColor (m_shadow);
	manualFrame->setHiliteColor (m_hilite);
			
    _radioButtons[2] = new FXRadioButton (manualFrame,
    	_stringTable->get ("MANUAL").c_str (), this,
    	ID_RADIO3);
	_radioButtons[2]->setBorderColor (m_theme.border);
	_radioButtons[2]->setBaseColor (m_theme.base);
	_radioButtons[2]->setBackColor (m_theme.base);
	_radioButtons[2]->setTextColor (m_theme.fore);
	_radioButtons[2]->setShadowColor (m_shadow);
	_radioButtons[2]->setHiliteColor (m_hilite);
    	
	_ratioField = new FXTextField (manualFrame, 0, this, ID_MANUALTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
  		0,0,30,0);
  	_ratioField->setBorderColor (m_theme.border);
	_ratioField->setBaseColor (m_theme.base);
	_ratioField->setBackColor (m_theme.base);
	_ratioField->setTextColor (m_theme.fore);
	_ratioField->setShadowColor (m_shadow);
	_ratioField->setHiliteColor (m_hilite);		
  		
	_ratioDial = new FXDial (manualFrame, this, ID_MANUAL, LAYOUT_FILL_X |
 		LAYOUT_CENTER_Y | DIAL_HORIZONTAL |
   		DIAL_HAS_NOTCH);
	double fr = _config->number ("Harmonizer", "FundamentalRatio");
	std::stringstream fs;
	fs << fr;
	_ratioDial->setRange (25, 400);
	_ratioDial->setValue ((int) (fr * 100));
	_ratioField->setText (fs.str ().c_str ());
	switch ((int) _config->number ("Harmonizer", "FundamentalType")) {
		case 0:
		    _radioButtons[0]->setCheck (true);
         	_transposeCombo->disable ();
          	_ratioField->disable ();
          	_ratioDial->disable ();
      	break;

        case 1:
		    _radioButtons[1]->setCheck (true);
         	_transposeCombo->enable ();
          	_ratioField->disable ();
          	_ratioDial->disable ();
        break;

        default:
        case 2:
		    _radioButtons[2]->setCheck (true);
         	_transposeCombo->disable ();
          	_ratioField->enable ();
          	_ratioDial->enable ();
        break;
	}
    FXGroupBox* cyclesBox = new FXGroupBox (matrix4,
    	_stringTable->get ("CYCLES").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,150,0);
  	cyclesBox->setBorderColor (m_theme.border);
	cyclesBox->setBaseColor (m_theme.base);
	cyclesBox->setBackColor (m_theme.base);
	cyclesBox->setShadowColor (m_shadow);
	cyclesBox->setHiliteColor (m_hilite);
			    	
#if defined (REPITCH_ONLY)
	cyclesBox->hide ();
#endif
	FXVerticalFrame* cyclesFrame = new FXVerticalFrame (cyclesBox,
		LAYOUT_FILL_X);
  	cyclesFrame->setBorderColor (m_theme.border);
	cyclesFrame->setBaseColor (m_theme.base);
	cyclesFrame->setBackColor (m_theme.base);
	cyclesFrame->setShadowColor (m_shadow);
	cyclesFrame->setHiliteColor (m_hilite);	
		
	FXCheckButton* autoModeCheck = new FXCheckButton (cyclesFrame,
 		_stringTable->get (
 		"AUTOMODE").c_str (), this, ID_AUTOMODE, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	autoModeCheck->setCheck ((bool) _config->number ("Harmonizer", "AutoMode"));
  	autoModeCheck->setBorderColor (m_theme.border);
	autoModeCheck->setBaseColor (m_theme.base);
	autoModeCheck->setBackColor (m_theme.base);
	autoModeCheck->setTextColor (m_theme.fore);
	autoModeCheck->setShadowColor (m_shadow);
	autoModeCheck->setHiliteColor (m_hilite);	
	
    _cyclesSpinner =  new FXSpinner (cyclesFrame, 0, this, ID_CYCLES, SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    _cyclesSpinner->setRange (1, 100);
    _cyclesSpinner->setValue ((int) _config->number ("Harmonizer", "Cycles"));
    if (_config->number ("Harmonizer", "AutoMode") == 0) {
		_cyclesSpinner->enable ();
	}
	else {
    	_cyclesSpinner->disable ();
 	}
    FXVerticalFrame* rightFrame = new FXVerticalFrame (matrix4,
		LAYOUT_FILL_X | LAYOUT_LEFT, 0,0,0,0, 0,0,0,0);
  	rightFrame->setBorderColor (m_theme.border);
	rightFrame->setBaseColor (m_theme.base);
	rightFrame->setBackColor (m_theme.base);
	rightFrame->setShadowColor (m_shadow);
	rightFrame->setHiliteColor (m_hilite);	
			
    FXGroupBox* jitterBox = new FXGroupBox (rightFrame,
    	_stringTable->get ("JITTER").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
  	jitterBox->setBorderColor (m_theme.border);
	jitterBox->setBaseColor (m_theme.base);
	jitterBox->setBackColor (m_theme.base);
	jitterBox->setTextColor (m_theme.fore);
	jitterBox->setShadowColor (m_shadow);
	jitterBox->setHiliteColor (m_hilite);
		    	
	#if defined REPITCH_ONLY
		jitterBox->hide ();
	#endif
 	FXHorizontalFrame* jitterFrame = new FXHorizontalFrame (jitterBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_HEIGHT);
  	jitterFrame->setBorderColor (m_theme.border);
	jitterFrame->setBaseColor (m_theme.base);
	jitterFrame->setBackColor (m_theme.base);
	jitterFrame->setShadowColor (m_shadow);
	jitterFrame->setHiliteColor (m_hilite);		
	
 	_jitterProgress = new FXProgressBar (jitterFrame, 0, 0,
    	LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
 	_jitterSlider = new FXRealSlider (jitterFrame, this, ID_JITTER,
   		LAYOUT_CENTER_Y | SLIDER_HORIZONTAL | SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT |
   		LAYOUT_FILL_X); //, 0,0,150,0);
	_jitterSlider->setRange (0, 1);
	_jitterSlider->setTickDelta (.1);
  	_jitterSlider->setBorderColor (m_theme.border);
	_jitterSlider->setBaseColor (m_theme.base);
	_jitterSlider->setBackColor (m_theme.base);
	_jitterSlider->setShadowColor (m_shadow);
	_jitterSlider->setHiliteColor (m_hilite);		
	
	_jitterProgress->setProgress ((int) (
		_config->number ("Harmonizer", "Jitter") * 100));
	_jitterSlider->setValue (
		_config->number ("Harmonizer", "Jitter"));
	FXGroupBox* rumbleBox = new FXGroupBox (rightFrame,
 		_stringTable->get ("PREPROCESSING").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X |
  		LAYOUT_FILL_Y, 0,0,300,0);
  	rumbleBox->setBorderColor (m_theme.border);
	rumbleBox->setBaseColor (m_theme.base);
	rumbleBox->setBackColor (m_theme.base);
	rumbleBox->setTextColor (m_theme.fore);
	rumbleBox->setShadowColor (m_shadow);
	rumbleBox->setHiliteColor (m_hilite);	 		
	
	FXCheckButton* rumbleCheck = new FXCheckButton (rumbleBox,
 		_stringTable->get (
 		"RUMBLE").c_str (), this, ID_ANTIRUMBLE, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	rumbleCheck->setCheck ((bool) _config->number ("Harmonizer", "AntiRumble"));
  	rumbleCheck->setBorderColor (m_theme.border);
	rumbleCheck->setBaseColor (m_theme.base);
	rumbleCheck->setBackColor (m_theme.base);
	rumbleCheck->setTextColor (m_theme.fore);
	rumbleCheck->setShadowColor (m_shadow);
	rumbleCheck->setHiliteColor (m_hilite);	

    jollybutton = new FXButton (buttons, _stringTable->get ("HARMONIZERCR").c_str (),
    	getApp ()->_harmonIcon, switcher, FXSwitcher::ID_OPEN_FIRST,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite);
	
	// pdet...
    FXGroupBox* label2 = new FXGroupBox (switcher,
    	_stringTable->get ("PITCHDETECTOR").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	label2->setFont (_titleFont);
	FXTabBook* tabBook1 = new FXTabBook (label2, NULL,
 		0, PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X |
   		LAYOUT_FILL_Y | LAYOUT_RIGHT);
    new FXTabItem (tabBook1,
    	_stringTable->get ("PERIODESTIMATOR").c_str ());
    /// --- ///
	FXVerticalFrame* periodEst = new FXVerticalFrame (tabBook1,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
 	FXHorizontalFrame* matrix5 = new FXHorizontalFrame (periodEst,
 		LAYOUT_FILL_X);
    FXGroupBox* feedbackBox = new FXGroupBox (matrix5,
    	_stringTable->get ("FEEDBACK").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* feedbackFrame = new FXHorizontalFrame (feedbackBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_feedbackField = new FXTextField (feedbackFrame, 0, this,
  		ID_FEEDBACKTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream feed;
	feed << _config->number ("Harmonizer",
		"Feedback");
	_feedbackField->setText (feed.str ().c_str ());
  	_feedbackSlider = new FXRealSlider (feedbackFrame, this,
    	ID_FEEDBACK, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_feedbackSlider->setRange (0, 1);
	_feedbackSlider->setValue (_config->number ("Harmonizer",
		"Feedback"));
    FXGroupBox* qualityBox = new FXGroupBox (matrix5,
    	_stringTable->get ("QUALITY").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* qualityFrame = new FXHorizontalFrame (qualityBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_qualityField = new FXTextField (qualityFrame, 0, this,
  		ID_QUALITYTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream qual;
	qual << _config->number ("Harmonizer",
		"FilterQuality");
	_qualityField->setText (qual.str ().c_str ());
  	_qualitySlider = new FXRealSlider (qualityFrame, this,
    	ID_QUALITY, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_qualitySlider->setRange (.25, 10);
	_qualitySlider->setValue (_config->number ("Harmonizer",
		"FilterQuality"));
    FXGroupBox* splitBox = new FXGroupBox (matrix5,
    	_stringTable->get ("SPLIT").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXSpinner* splitSpinner = new FXSpinner (splitBox, 0, this, ID_SPLIT,
    	SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X  | LAYOUT_CENTER_Y);
    splitSpinner->setRange (4, 8192);
    splitSpinner->setValue ((int) _config->number ("Harmonizer", "Split"));
	FXHorizontalFrame* matrix6 = new FXHorizontalFrame (periodEst,
 		LAYOUT_FILL_X);
    FXGroupBox* ampThreshBox = new FXGroupBox (matrix6,
    	_stringTable->get ("AMPLITUDETHRESHOLD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* ampThreshFrame = new FXHorizontalFrame (ampThreshBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_ampThreshField = new FXTextField (ampThreshFrame, 0, this,
  		ID_AMPTHRESHOLDTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream ampth;
	ampth << _config->number ("Harmonizer",
		"AmpThreshold");
	_ampThreshField->setText (ampth.str ().c_str ());
  	_ampThreshSlider = new FXRealSlider (ampThreshFrame, this,
    	ID_AMPTHRESHOLD, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_ampThreshSlider->setRange (0, 1);
	_ampThreshSlider->setValue (_config->number ("Harmonizer",
		"AmpThreshold"));
    FXGroupBox* smoothBox1 = new FXGroupBox (matrix6,
    	_stringTable->get ("SMOOTHQUAL").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* smoothQualFrame = new FXHorizontalFrame (smoothBox1,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_smoothQualField = new FXTextField (smoothQualFrame, 0, this,
  		ID_SMOOTHQUALTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream smqs;
	smqs << _config->number ("Harmonizer",
		"SmoothingQuality");
	_smoothQualField->setText (smqs.str ().c_str ());
  	_smoothQualSlider = new FXRealSlider (smoothQualFrame, this,
    	ID_SMOOTHQUAL, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_smoothQualSlider->setRange (.1, 10);
	_smoothQualSlider->setValue (_config->number ("Harmonizer",
		"SmoothingQuality"));
    FXGroupBox* smoothBox2 = new FXGroupBox (matrix6,
    	_stringTable->get ("SMOOTHING").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXHorizontalFrame* smoothFrame = new FXHorizontalFrame (smoothBox2,
		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	_smoothField = new FXTextField (smoothFrame, 0, this, ID_SMOOTHINGTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
  		0,0,40,0);
	_smoothDial = new FXDial (smoothFrame, this, ID_SMOOTHING, LAYOUT_FILL_X |
 		LAYOUT_CENTER_Y | DIAL_HORIZONTAL |
   		DIAL_HAS_NOTCH);
	double sm = _config->number ("Harmonizer", "SmoothingRate");
	std::stringstream sms;
	sms << sm;
	_smoothDial->setRange (10, 8000);
	_smoothDial->setValue ((int) (sm));
	_smoothField->setText (sms.str ().c_str ());
//    FXGroupBox* zccyclesBox = new FXGroupBox (matrix6,
//    	_stringTable->get ("ZCCYCLES").c_str (),
//    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
//    FXSpinner* zccyclesSpinner = new FXSpinner (zccyclesBox, 0, this,
//    	ID_ZCCYCLES,
//    	SPIN_NORMAL |
//    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
//    zccyclesSpinner->setRange (1, 20);
//    zccyclesSpinner->setValue ((int) _config->number ("Harmonizer", "ZCCycles"));
	FXHorizontalFrame* matrix6a = new FXHorizontalFrame (periodEst,
 		LAYOUT_FILL_X);
    FXGroupBox* filtersBox = new FXGroupBox (matrix6a,
    	_stringTable->get ("FILTERS").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
	FXHorizontalFrame* filtersFrame = new FXHorizontalFrame (filtersBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXButton (filtersFrame,
    	_stringTable->get ("FILTERSBUTTON").c_str (), NULL,
    	this, ID_FILTERS,
    	LAYOUT_LEFT | LAYOUT_CENTER_Y |
     	FRAME_RAISED | FRAME_THICK, 0,0,0,0, 20,20);
	std::stringstream filStat;
	filStat << _stringTable->get ("FILTERLEVELS")
		<< ": " << _config->number ("Harmonizer", "Levels") <<
		" - " << _stringTable->get ("RANGESTARTEND") <<
		": " << _config->number ("Harmonizer", "RangeStart") << ", " <<
			_config->number ("Harmonizer", "RangeEnd");
   	_filterStatus = new FXLabel (filtersFrame, filStat.str ().c_str (), 0 ,
		LAYOUT_CENTER_Y);
	/// ---- ///
#if defined (REPITCH_ONLY)
    FXTabItem* autoTuneSel =
#endif
   new FXTabItem (tabBook1,
    	_stringTable->get ("AUTOTUNER").c_str ());
#if defined (REPITCH_ONLY)
	autoTuneSel->hide ();
#endif
	FXVerticalFrame* autoTune = new FXVerticalFrame (tabBook1,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
 	FXHorizontalFrame* matrix7 = new FXHorizontalFrame (autoTune,
 		LAYOUT_FILL_X);
    FXGroupBox* rangeStartEndBox = new FXGroupBox (matrix7,
    	_stringTable->get ("RANGESTARTEND").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* rangeStartFrame1 = new FXHorizontalFrame (
 		rangeStartEndBox,
		LAYOUT_FILL_X);
	_rangeStartField1 = new FXTextField (rangeStartFrame1, 0, this,
  		ID_RANGESTARTTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream rstart1;
	rstart1 << _config->number ("Harmonizer",
		"RangeStart1");
	_rangeStartField1->setText (rstart1.str ().c_str ());
  	_rangeStartSlider1 = new FXRealSlider (rangeStartFrame1, this,
    	ID_RANGESTART1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_rangeStartSlider1->setRange (20, 2500);
	_rangeStartSlider1->setValue (_config->number ("Harmonizer",
		"RangeStart1"));
	FXHorizontalFrame* rangeEndFrame1 = new FXHorizontalFrame (
 		rangeStartEndBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_rangeEndField1 = new FXTextField (rangeEndFrame1, 0, this,
  		ID_RANGEENDTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream rend1;
	rend1 << _config->number ("Harmonizer",
		"RangeEnd1");
	_rangeEndField1->setText (rend1.str ().c_str ());
  	_rangeEndSlider1 = new FXRealSlider (rangeEndFrame1, this,
    	ID_RANGEEND1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_rangeEndSlider1->setRange (20, 2500);
	_rangeEndSlider1->setValue (_config->number ("Harmonizer",
		"RangeEnd1"));
    FXGroupBox* qualityBox1 = new FXGroupBox (matrix7,
    	_stringTable->get ("QUALITY").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* qualityFrame1 = new FXHorizontalFrame (qualityBox1,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_qualityField1 = new FXTextField (qualityFrame1, 0, this,
  		ID_QUALITYTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream qual1;
	qual1 << _config->number ("Harmonizer",
		"FilterQuality1");
	_qualityField1->setText (qual1.str ().c_str ());
  	_qualitySlider1 = new FXRealSlider (qualityFrame1, this,
    	ID_QUALITY1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_qualitySlider1->setRange (.25, 10);
	_qualitySlider1->setValue (_config->number ("Harmonizer",
		"FilterQuality1"));
	FXHorizontalFrame* matrix8 = new FXHorizontalFrame (autoTune,
 		LAYOUT_FILL_X);
    FXGroupBox* ampThreshBox1 = new FXGroupBox (matrix8,
    	_stringTable->get ("AMPLITUDETHRESHOLD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* ampThreshFrame1 = new FXHorizontalFrame (
		ampThreshBox1,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_ampThreshField1 = new FXTextField (ampThreshFrame1, 0, this,
  		ID_AMPTHRESHOLDTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream ampth1;
	ampth1 << _config->number ("Harmonizer",
		"AmpThreshold1");
	_ampThreshField1->setText (ampth1.str ().c_str ());
  	_ampThreshSlider1 = new FXRealSlider (ampThreshFrame1, this,
    	ID_AMPTHRESHOLD1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_ampThreshSlider1->setRange (0, 1);
	_ampThreshSlider1->setValue (_config->number ("Harmonizer",
		"AmpThreshold1"));
    FXGroupBox* smoothBox1ATune = new FXGroupBox (matrix8,
    	_stringTable->get ("SMOOTHQUAL").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* smoothQualFrameATune = new FXHorizontalFrame (smoothBox1ATune,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_smoothQualFieldATune = new FXTextField (smoothQualFrameATune, 0, this,
  		ID_SMOOTHQUALTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream smqs2;
	smqs2 << _config->number ("Harmonizer",
		"SmoothingQuality1");
	_smoothQualFieldATune->setText (smqs2.str ().c_str ());
  	_smoothQualSliderATune = new FXRealSlider (smoothQualFrameATune, this,
    	ID_SMOOTHQUAL1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_smoothQualSliderATune->setRange (.1, 10);
	_smoothQualSliderATune->setValue (_config->number ("Harmonizer",
		"SmoothingQuality1"));
    FXGroupBox* smoothBox2ATune = new FXGroupBox (matrix8,
    	_stringTable->get ("SMOOTHING").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXHorizontalFrame* smoothFrameATune = new FXHorizontalFrame (smoothBox2ATune,
		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	_smoothFieldATune = new FXTextField (smoothFrameATune, 0, this, ID_SMOOTHINGTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
  		0,0,40,0);
	_smoothDialATune = new FXDial (smoothFrameATune, this, ID_SMOOTHING1, LAYOUT_FILL_X |
 		LAYOUT_CENTER_Y | DIAL_HORIZONTAL |
   		DIAL_HAS_NOTCH);
	double sm2 = _config->number ("Harmonizer", "SmoothingRate1");
	std::stringstream sms2;
	sms2 << sm2;
	_smoothDialATune->setRange (10, 8000);
	_smoothDialATune->setValue ((int) (sm2));
	_smoothFieldATune->setText (sms2.str ().c_str ());
	FXHorizontalFrame* matrix8a = new FXHorizontalFrame (autoTune,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    FXGroupBox* filtersBox1 = new FXGroupBox (matrix8a,
    	_stringTable->get ("FILTERS").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* filtersFrame1 = new FXHorizontalFrame (filtersBox1,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXButton (filtersFrame1,
    	_stringTable->get ("FILTERSBUTTON").c_str (), NULL,
    	this, ID_FILTERS1,
    	LAYOUT_LEFT | LAYOUT_CENTER_Y |
     	FRAME_RAISED | FRAME_THICK, 0,0,0,0, 20,20);
	std::stringstream filStat1;
	filStat1 << _stringTable->get ("FILTERLEVELS")
		<< ": " << _config->number ("Harmonizer", "Levels1");
   	_filterStatus1 = new FXLabel (filtersFrame1, filStat1.str ().c_str (), 0 ,
		LAYOUT_CENTER_Y);
#if 0
    FXGroupBox* speedBox1 = new FXGroupBox (matrix8a,
    	_stringTable->get ("SPEED").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* speedFrame1 = new FXHorizontalFrame (
		speedBox1,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_speedField1 = new FXTextField (speedFrame1, 0, this,
  		ID_SPEEDTEXT1,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream speed1;
	speed1 << _config->number ("Harmonizer",
		"Speed1");
	_speedField1->setText (speed1.str ().c_str ());
  	_speedSlider1 = new FXRealSlider (speedFrame1, this,
    	ID_SPEED1, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_speedSlider1->setRange (1, 50);
	_speedSlider1->setValue (_config->number ("Harmonizer",
		"Speed1"));
#endif
	/// --- ///
#if defined (REPITCH_ONLY)
    FXTabItem* spectralEstimatorSel =
#endif
   new FXTabItem (tabBook1,
    	_stringTable->get ("SPECTRALESTIMATOR").c_str ());
#if defined (REPITCH_ONLY)
	spectralEstimatorSel->hide ();
#endif
	// spectral estimator
	FXVerticalFrame* spectralEstimator = new FXVerticalFrame (tabBook1,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_CENTER_X
   		| LAYOUT_TOP | LAYOUT_LEFT,
     	0,0,0,0,4,4,4,4);
 	FXHorizontalFrame* matrix13 = new FXHorizontalFrame (spectralEstimator,
 		LAYOUT_FILL_X);
    FXGroupBox* rangeStartEndBox2 = new FXGroupBox (matrix13,
    	_stringTable->get ("RANGESTARTEND").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* rangeStartFrame2 = new FXHorizontalFrame (
 		rangeStartEndBox2,
		LAYOUT_FILL_X);
	_rangeStartField2 = new FXTextField (rangeStartFrame2, 0, this,
  		ID_RANGESTARTTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream rstart2;
	rstart2 << _config->number ("Harmonizer",
		"RangeStart2");
	_rangeStartField2->setText (rstart2.str ().c_str ());
  	_rangeStartSlider2 = new FXRealSlider (rangeStartFrame2, this,
    	ID_RANGESTART2, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_rangeStartSlider2->setRange (20, 2500);
	_rangeStartSlider2->setValue (_config->number ("Harmonizer",
		"RangeStart2"));
	FXHorizontalFrame* rangeEndFrame2 = new FXHorizontalFrame (
 		rangeStartEndBox2,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_rangeEndField2 = new FXTextField (rangeEndFrame2, 0, this,
  		ID_RANGEENDTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream rend2;
	rend2 << _config->number ("Harmonizer",
		"RangeEnd2");
	_rangeEndField2->setText (rend2.str ().c_str ());
  	_rangeEndSlider2 = new FXRealSlider (rangeEndFrame2, this,
    	ID_RANGEEND2, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_rangeEndSlider2->setRange (20, 2500);
	_rangeEndSlider2->setValue (_config->number ("Harmonizer",
		"RangeEnd2"));
//	FXHorizontalFrame* matrix14 = new FXHorizontalFrame (spectralEstimator,
// 		LAYOUT_FILL_X);
    FXGroupBox* qualityBox2 = new FXGroupBox (matrix13,
    	_stringTable->get ("QUALITY").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* qualityFrame2 = new FXHorizontalFrame (qualityBox2,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_qualityField2 = new FXTextField (qualityFrame2, 0, this,
  		ID_QUALITYTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream qual2;
	qual2 << _config->number ("Harmonizer",
		"FilterQuality2");
	_qualityField2->setText (qual2.str ().c_str ());
  	_qualitySlider2 = new FXRealSlider (qualityFrame2, this,
    	ID_QUALITY2, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_qualitySlider2->setRange (.25, 10);
	_qualitySlider2->setValue (_config->number ("Harmonizer",
		"FilterQuality2"));
    FXGroupBox* splitBox2 = new FXGroupBox (matrix13,
    	_stringTable->get ("SPLIT").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXSpinner* splitSpinner2 = new FXSpinner (splitBox2, 0, this, ID_SPLIT2,
    	SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X  | LAYOUT_CENTER_Y);
    splitSpinner2->setRange (1, 8192);
    splitSpinner2->setValue ((int) _config->number ("Harmonizer", "Split2"));
	FXHorizontalFrame* matrix13a = new FXHorizontalFrame (spectralEstimator,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    FXGroupBox* ampThreshBox2 = new FXGroupBox (matrix13a,
    	_stringTable->get ("AMPLITUDETHRESHOLD").c_str (),
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* ampThreshFrame2 = new FXHorizontalFrame (
		ampThreshBox2,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_ampThreshField2 = new FXTextField (ampThreshFrame2, 0, this,
  		ID_AMPTHRESHOLDTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream ampth2;
	ampth2 << _config->number ("Harmonizer",
		"AmpThreshold2");
	_ampThreshField2->setText (ampth2.str ().c_str ());
  	_ampThreshSlider2 = new FXRealSlider (ampThreshFrame2, this,
    	ID_AMPTHRESHOLD2, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_ampThreshSlider2->setRange (0, 50);
	_ampThreshSlider2->setValue (_config->number ("Harmonizer",
		"AmpThreshold2"));
    FXGroupBox* smoothBox2Spectral = new FXGroupBox (matrix13a,
    	_stringTable->get ("SMOOTHQUAL").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* smoothQualFrameSpectral = new FXHorizontalFrame (smoothBox2Spectral,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_smoothQualFieldSpectral = new FXTextField (smoothQualFrameSpectral, 0, this,
  		ID_SMOOTHQUALTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream smqs3;
	smqs3 << _config->number ("Harmonizer",
		"SmoothingQuality2");
	_smoothQualFieldSpectral->setText (smqs3.str ().c_str ());
  	_smoothQualSliderSpectral = new FXRealSlider (smoothQualFrameSpectral, this,
    	ID_SMOOTHQUAL2, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_smoothQualSliderSpectral->setRange (.1, 10);
	_smoothQualSliderSpectral->setValue (_config->number ("Harmonizer",
		"SmoothingQuality2"));
    FXGroupBox* smoothBox3Spectral = new FXGroupBox (matrix13a,
    	_stringTable->get ("SMOOTHING").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXHorizontalFrame* smoothFrameSpectral = new FXHorizontalFrame (smoothBox3Spectral,
		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	_smoothFieldSpectral = new FXTextField (smoothFrameSpectral, 0, this, ID_SMOOTHINGTEXT2,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
  		0,0,40,0);
	_smoothDialSpectral = new FXDial (smoothFrameSpectral, this, ID_SMOOTHING2, LAYOUT_FILL_X |
 		LAYOUT_CENTER_Y | DIAL_HORIZONTAL |
   		DIAL_HAS_NOTCH);
	double sm3 = _config->number ("Harmonizer", "SmoothingRate2");
	std::stringstream sms3;
	sms3 << sm3;
	_smoothDialSpectral->setRange (10, 8000);
	_smoothDialSpectral->setValue ((int) (sm3));
	_smoothFieldSpectral->setText (sms3.str ().c_str ());
	FXHorizontalFrame* matrix13b = new FXHorizontalFrame (spectralEstimator,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    FXGroupBox* FFTBox = new FXGroupBox (matrix13b,
    	_stringTable->get ("FFTSIZE").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X, 0,0,0,0, 4,4,4,4);
	FXHorizontalFrame* FFTFrame = new FXHorizontalFrame (FFTBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXButton (FFTFrame,
    	_stringTable->get ("FILTERSBUTTON").c_str (), NULL,
    	this, ID_FFTSIZE,
    	LAYOUT_LEFT | LAYOUT_CENTER_Y |
     	FRAME_RAISED | FRAME_THICK, 0,0,0,0, 20,20);
	std::stringstream fftSize;
	int size = (int) pow (2,
		(int) _config->number ("Harmonizer", "FFTSize") + 7);
	fftSize  << _stringTable->get ("FILTERLEVELS")
		<< ": " << _config->number ("Harmonizer", "Levels2") << " - "
		<< size << " bytes";
   	_FFTStatus = new FXLabel (FFTFrame, fftSize.str ().c_str (), 0 ,
		LAYOUT_CENTER_Y);
	/// --- ///
    jollybutton = new FXButton (buttons, _stringTable->get ("PITCHDETECTORCR").c_str (),
    	getApp ()->_pdetIcon, switcher, FXSwitcher::ID_OPEN_SECOND,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite);
	
	// voices
	FXGroupBox* label3 = new FXGroupBox (switcher,
 		_stringTable->get ("VOICES").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	label3->setFont (_titleFont);
	FXTabBook* tabBook2 = new FXTabBook (label3, NULL,
 		0, LAYOUT_FILL_X |
   		LAYOUT_FILL_Y | LAYOUT_RIGHT);
    new FXTabItem (tabBook2,
    	_stringTable->get ("PARTIALS").c_str ());
	FXVerticalFrame* ratios = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
	FXHorizontalFrame* partialsButtonFrame = new FXHorizontalFrame (ratios,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* logicBox = new FXGroupBox (partialsButtonFrame,
 		_stringTable->get ("PRESET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
    FXComboBox* logicCombo = new FXComboBox (logicBox, 0, this,
    	ID_LOGIC,
    	COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK |
     	LAYOUT_SIDE_TOP);
    logicCombo->setNumVisible (5);
	logicCombo->appendItem (_stringTable->get ("CUSTOM").c_str ());
	logicCombo->appendItem (_stringTable->get ("DOMINANTSEVENTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("MINORSEVENTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("MAJORSEVENTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("DIMINISHEDSEVENTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("AUGMENTEDSEVENTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("QUARTALCHORD").c_str ());
	logicCombo->appendItem (_stringTable->get ("SCALIC").c_str ());
	logicCombo->appendItem (_stringTable->get ("INTELLICHORD").c_str ());
	logicCombo->appendItem (_stringTable->get ("MIDISTREAM").c_str ());
	logicCombo->appendItem (_stringTable->get ("MINORWITHSIXTH").c_str ());
	logicCombo->appendItem (_stringTable->get ("SINGLECHORD").c_str ());
	logicCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "Logic"));
	FXGroupBox* controlsBox = new FXGroupBox (partialsButtonFrame,
 		_stringTable->get ("CONTROLS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE);
	FXHorizontalFrame* matrix9 = new FXHorizontalFrame (controlsBox,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    _defaultButton = new FXButton (matrix9,
    	_stringTable->get ("DEFAULTBUTTON").c_str (), NULL,
    	this, ID_DEFAULT,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
    _randomButton = new FXButton (matrix9,
    	_stringTable->get ("RANDOMBUTTON").c_str (), NULL,
    	this, ID_RANDOM,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
	FXHorizontalFrame* partialsFrame = new FXHorizontalFrame (ratios,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP);
	FXGroupBox* partialsBoxes[PARTIALS];
	FXHorizontalFrame* singlePartialFrames[PARTIALS];
	FXVerticalFrame* partialsColumns[2 * PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
	    std::stringstream v;
	    std::stringstream a;
 	    std::stringstream a1;
 	    std::stringstream f;
   	    std::stringstream f1;
	    v << "V" << i + 1;
    	partialsBoxes[i] = new FXGroupBox (partialsFrame,
 			v.str ().c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X |
  		LAYOUT_FILL_Y);
    	singlePartialFrames[i] = new FXHorizontalFrame (partialsBoxes[i],
     		LAYOUT_FILL_Y | LAYOUT_FILL_X);
  		partialsColumns[i] = new FXVerticalFrame (singlePartialFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
  		partialsColumns[i + 1] = new FXVerticalFrame (singlePartialFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
		_partialsAmpValues[i] = new FXTextField (partialsColumns[i], 0, this,
  			ID_A1TEXT + i,
			FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		a << "A" << i + 1;
		a1 << _config->number ("Harmonizer",
			a.str ());
		_partialsAmpValues[i]->setText (a1.str ().c_str ());
  		_partialsAmpSliders[i] = new FXRealSlider (partialsColumns[i], this,
    		ID_A1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
  		_partialsAmpSliders[i]->setRange (0, 1.0);
 		_partialsAmpSliders[i]->setTickDelta (.1);
 		_partialsAmpSliders[i]->setValue (_config->number ("Harmonizer",
			a.str ()));
  		new FXLabel (partialsColumns[i], "A", 0, LAYOUT_CENTER_X);
		_partialsFreqValues[i] = new FXTextField (partialsColumns[i + 1], 0,
  			this, ID_F1TEXT + i, FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		f << "F" << i + 1;
		f1 << _config->number ("Harmonizer",
			f.str ());
		_partialsFreqValues[i]->setText (f1.str ().c_str ());
  		_partialsFreqSliders[i] = new FXRealSlider (partialsColumns[i + 1], this,
    		ID_F1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
  		_partialsFreqSliders[i]->setRange (.25, 4.0);
 		_partialsFreqSliders[i]->setTickDelta (.25);
 		_partialsFreqSliders[i]->setValue (_config->number ("Harmonizer",
			f.str ()));
  		new FXLabel (partialsColumns[i + 1], "F", 0, LAYOUT_CENTER_X);
	}
	FXHorizontalFrame* matrix3a = new FXHorizontalFrame (ratios,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* masterKeyBox = new FXGroupBox (matrix3a,
 		_stringTable->get ("MASTERKEY").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X,
  		0,0,200,0);
	FXHorizontalFrame* masterKeyFrame = new FXHorizontalFrame (masterKeyBox,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT,
     	0,0,0,0, 0,0,0,0, 4, 4);
	 _masterKeyCombo = new FXListBox (masterKeyFrame, this,
    	ID_MASTERKEY, LAYOUT_FILL_X |
    	LISTBOX_NORMAL | FRAME_SUNKEN | FRAME_THICK, 0,0,0,0);
    _masterKeyCombo->setNumVisible (10);
    _masterKeyCombo->appendItem (_stringTable->get ("C").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("C#").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("D").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("D#").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("E").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("F").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("F#").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("G").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("G#").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("A").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("A#").c_str ());
    _masterKeyCombo->appendItem (_stringTable->get ("B").c_str ());
	_masterKeyCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "MasterKey"));
	FXGroupBox* scaleBox = new FXGroupBox (matrix3a,
 		_stringTable->get ("SCALE").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,230,0);
	FXHorizontalFrame* scaleFrame = new FXHorizontalFrame (scaleBox,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT,
     	0,0,0,0, 0,0,0,0, 4, 4);
	_scaleTypeCombo = new FXListBox (scaleFrame, this,
    	ID_SCALE, LAYOUT_FILL_X |
    	LISTBOX_NORMAL | FRAME_SUNKEN | FRAME_THICK, 0,0,0,0);
    _scaleTypeCombo->setNumVisible (6);
    _scaleTypeCombo->appendItem (_stringTable->get ("MAJOR1").c_str ());
    _scaleTypeCombo->appendItem (_stringTable->get ("MAJOR2").c_str ());
    _scaleTypeCombo->appendItem (_stringTable->get ("MINOR1").c_str ());
    _scaleTypeCombo->appendItem (_stringTable->get ("MINOR2").c_str ());
    _scaleTypeCombo->appendItem (_stringTable->get ("MINOR3").c_str ());
    _scaleTypeCombo->appendItem (_stringTable->get ("BLUES1").c_str ());
//    _scaleTypeCombo->appendItem (_stringTable->get ("CHROMA_MAJOR1").c_str ());
//    _scaleTypeCombo->appendItem (_stringTable->get ("OCTOPHONIC1").c_str ());
	_scaleTypeCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "Scale"));
    _defaultButton->disable ();
    _randomButton->disable ();
    _masterKeyCombo->disable ();
    _scaleTypeCombo->disable ();
    for (int i = 0; i < PARTIALS; i++) {
    	_partialsAmpValues[i]->disable ();
		_partialsAmpSliders[i]->disable ();
		_partialsFreqValues[i]->disable ();
		_partialsFreqSliders[i]->disable ();
 	}
	if ((int) _config->number ("Harmonizer", "Logic") == 0) {
	    _defaultButton->enable ();
	    _randomButton->enable ();
	    FXString temp;
	    for (int i = 0; i < (int) PARTIALS; i++) {
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
        		_partialsAmpValues[i]->enable ();
    			_partialsAmpSliders[i]->enable ();
				_partialsFreqValues[i]->enable ();
    			_partialsFreqSliders[i]->enable ();
  			}
     	}
   	}
   	else if ((int) _config->number ("Harmonizer", "Logic") == 7
   	 || (int) _config->number ("Harmonizer", "Logic") == 8
   	 || (int) _config->number ("Harmonizer", "Logic") == 11) {
   		_masterKeyCombo->enable ();
   		_scaleTypeCombo->enable ();
   	}
    new FXTabItem (tabBook2,
    	_stringTable->get ("VIBRATO").c_str ());
	FXVerticalFrame* vibr = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
//	FXHorizontalFrame* matrix10 = new FXHorizontalFrame (vibr,
// 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXHorizontalFrame* vibratoButtonFrame = new FXHorizontalFrame (vibr,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* vibratoBox = new FXGroupBox (vibratoButtonFrame,
 		_stringTable->get ("PRESET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
    FXComboBox* vibratoCombo = new FXComboBox (vibratoBox, 0, this,
    	ID_VIBRATOLOGIC,
    	COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK |
     	LAYOUT_SIDE_TOP);
    vibratoCombo->setNumVisible (5);
	vibratoCombo->appendItem (_stringTable->get ("CUSTOM").c_str ());
	vibratoCombo->appendItem (_stringTable->get ("SMALLVIBRATO").c_str ());
	vibratoCombo->appendItem (_stringTable->get ("MEDIUMVIBRATO").c_str ());
	vibratoCombo->appendItem (_stringTable->get ("LARGEVIBRATO").c_str ());
	vibratoCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "VibratoLogic"));
	FXGroupBox* controlsBox2 = new FXGroupBox (vibratoButtonFrame,
 		_stringTable->get ("CONTROLS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE);
	FXHorizontalFrame* matrix10 = new FXHorizontalFrame (controlsBox2,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    _defaultButton2 = new FXButton (matrix10,
    	_stringTable->get ("DEFAULTBUTTON").c_str (), NULL,
    	this, ID_DEFAULT2,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
    _randomButton2 = new FXButton (matrix10,
    	_stringTable->get ("RANDOMBUTTON").c_str (), NULL,
    	this, ID_RANDOM2,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
	FXHorizontalFrame* vibratoFrame = new FXHorizontalFrame (vibr,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP);
	FXGroupBox* vibratoBoxes[PARTIALS];
	FXHorizontalFrame* singleVibratoFrames[PARTIALS];
	FXVerticalFrame* vibratoColumns[2 * PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
	    std::stringstream vv;
	    std::stringstream va;
 	    std::stringstream va1;
 	    std::stringstream vf;
   	    std::stringstream vf1;
	    vv << "V" << i + 1;
    	vibratoBoxes[i] = new FXGroupBox (vibratoFrame,
 			vv.str ().c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X |
  		LAYOUT_FILL_Y);
    	singleVibratoFrames[i] = new FXHorizontalFrame (vibratoBoxes[i],
     		LAYOUT_FILL_Y | LAYOUT_FILL_X);
  		vibratoColumns[i] = new FXVerticalFrame (singleVibratoFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
  		vibratoColumns[i + 1] = new FXVerticalFrame (singleVibratoFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
		_vibratoAmpValues[i] = new FXTextField (vibratoColumns[i], 0, this,
  			ID_VA1TEXT + i,
			FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		va << "VA" << i + 1;
		va1 << _config->number ("Harmonizer",
			va.str ());
		_vibratoAmpValues[i]->setText (va1.str ().c_str ());
  		_vibratoAmpSliders[i] = new FXRealSlider (vibratoColumns[i], this,
    		ID_VA1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
  		_vibratoAmpSliders[i]->setRange (0, 20);
 		_vibratoAmpSliders[i]->setTickDelta (2);
 		_vibratoAmpSliders[i]->setValue (_config->number ("Harmonizer",
			va.str ()));
  		new FXLabel (vibratoColumns[i], "A", 0, LAYOUT_CENTER_X);
		_vibratoFreqValues[i] = new FXTextField (vibratoColumns[i + 1], 0,
  			this, ID_VF1TEXT + i, FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		vf << "VF" << i + 1;
		vf1 << _config->number ("Harmonizer",
			vf.str ());
		_vibratoFreqValues[i]->setText (vf1.str ().c_str ());
  		_vibratoFreqSliders[i] = new FXRealSlider (vibratoColumns[i + 1], this,
    		ID_VF1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
  		_vibratoFreqSliders[i]->setRange (0, 10);
 		_vibratoFreqSliders[i]->setTickDelta (1);
 		_vibratoFreqSliders[i]->setValue (_config->number ("Harmonizer",
			vf.str ()));
  		new FXLabel (vibratoColumns[i + 1], "F", 0, LAYOUT_CENTER_X);
	}
    _defaultButton2->disable ();
    _randomButton2->disable ();
    for (int i = 0; i < PARTIALS; i++) {
    	_vibratoAmpValues[i]->disable ();
		_vibratoAmpSliders[i]->disable ();
		_vibratoFreqValues[i]->disable ();
		_vibratoFreqSliders[i]->disable ();
 	}
	if ((int) _config->number ("Harmonizer", "VibratoLogic") == 0) {
	    _defaultButton2->enable ();
	    _randomButton2->enable ();
	    FXString temp;
	    for (int i = 0; i < (int) PARTIALS; i++) {
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
        		_vibratoAmpValues[i]->enable ();
    			_vibratoAmpSliders[i]->enable ();
				_vibratoFreqValues[i]->enable ();
    			_vibratoFreqSliders[i]->enable ();
  			}
     	}
   	}
    new FXTabItem (tabBook2,
    	_stringTable->get ("RANDVIBRATO").c_str ());
	FXVerticalFrame* randVibr = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
//	FXHorizontalFrame* matrix10 = new FXHorizontalFrame (vibr,
// 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXHorizontalFrame* randVibratoButtonFrame = new FXHorizontalFrame (randVibr,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* randVibratoBox = new FXGroupBox (randVibratoButtonFrame,
 		_stringTable->get ("PRESET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
    FXComboBox* randVibratoCombo = new FXComboBox (randVibratoBox, 0, this,
    	ID_RANDVIBRATOLOGIC,
    	COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK |
     	LAYOUT_SIDE_TOP);
    randVibratoCombo->setNumVisible (5);
	randVibratoCombo->appendItem (_stringTable->get ("CUSTOM").c_str ());
	randVibratoCombo->appendItem (_stringTable->get ("SMALLRANDVIBRATO").c_str ());
	randVibratoCombo->appendItem (_stringTable->get ("MEDIUMRANDVIBRATO").c_str ());
	randVibratoCombo->appendItem (_stringTable->get ("LARGERANDVIBRATO").c_str ());
	randVibratoCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "RandomVibratoLogic"));
	FXGroupBox* controlsBox2a = new FXGroupBox (randVibratoButtonFrame,
 		_stringTable->get ("CONTROLS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE);
	FXHorizontalFrame* matrix10a = new FXHorizontalFrame (controlsBox2a,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    _defaultButton2a = new FXButton (matrix10a,
    	_stringTable->get ("DEFAULTBUTTON").c_str (), NULL,
    	this, ID_DEFAULT2A,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
    _randomButton2a = new FXButton (matrix10a,
    	_stringTable->get ("RANDOMBUTTON").c_str (), NULL,
    	this, ID_RANDOM2A,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
	FXHorizontalFrame* randVibratoFrame = new FXHorizontalFrame (randVibr,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP);
	FXGroupBox* randVibratoBoxes[PARTIALS];
	FXHorizontalFrame* singleRandVibratoFrames[PARTIALS];
	FXVerticalFrame* randVibratoColumns[2 * PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
	    std::stringstream vv;
	    std::stringstream va;
 	    std::stringstream va1;
 	    std::stringstream vf;
   	    std::stringstream vf1;
	    vv << "V" << i + 1;
    	randVibratoBoxes[i] = new FXGroupBox (randVibratoFrame,
 			vv.str ().c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X |
  		LAYOUT_FILL_Y);
    	singleRandVibratoFrames[i] = new FXHorizontalFrame (randVibratoBoxes[i],
     		LAYOUT_FILL_Y | LAYOUT_FILL_X);
//  		randVibratoColumns[i] = new FXVerticalFrame (singleRandVibratoFrames[i],
//    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
  		randVibratoColumns[i + 1] = new FXVerticalFrame (singleRandVibratoFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
//		_randVibratoAmpValues[i] = new FXTextField (randVibratoColumns[i], 0, this,
//  			ID_VARAND1TEXT + i,
//			FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
//  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
//			0,0,30,0, 4,4,4,4);
//		va << "VARAND" << i + 1;
//		va1 << _config->number ("Harmonizer",
//			va.str ());
//		_randVibratoAmpValues[i]->setText (va1.str ().c_str ());
//  		_randVibratoAmpSliders[i] = new FXRealSlider (randVibratoColumns[i], this,
//    		ID_VARAND1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
//      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
//        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
//  		_randVibratoAmpSliders[i]->setRange (0, 1);
// 		_randVibratoAmpSliders[i]->setTickDelta (.1);
// 		_randVibratoAmpSliders[i]->setValue (_config->number ("Harmonizer",
//			va.str ()));
//  		new FXLabel (randVibratoColumns[i], "A", 0, LAYOUT_CENTER_X);
		_randVibratoFreqValues[i] = new FXTextField (randVibratoColumns[i + 1], 0,
  			this, ID_VFRAND1TEXT + i, FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		vf << "VFRAND" << i + 1;
		vf1 << _config->number ("Harmonizer",
			vf.str ());
		_randVibratoFreqValues[i]->setText (vf1.str ().c_str ());
  		_randVibratoFreqSliders[i] = new FXRealSlider (randVibratoColumns[i + 1], this,
    		ID_VFRAND1 + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_INSIDE_BAR | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,12,0);
  		_randVibratoFreqSliders[i]->setRange (0, 25);
 		_randVibratoFreqSliders[i]->setTickDelta (.1);
 		_randVibratoFreqSliders[i]->setValue (_config->number ("Harmonizer",
			vf.str ()));
  		new FXLabel (randVibratoColumns[i + 1], "F", 0, LAYOUT_CENTER_X);
	}
    _defaultButton2a->disable ();
    _randomButton2a->disable ();
    for (int i = 0; i < PARTIALS; i++) {
//    	_randVibratoAmpValues[i]->disable ();
//		_randVibratoAmpSliders[i]->disable ();
		_randVibratoFreqValues[i]->disable ();
		_randVibratoFreqSliders[i]->disable ();
 	}
	if ((int) _config->number ("Harmonizer", "RandomVibratoLogic") == 0) {
	    _defaultButton2a->enable ();
	    _randomButton2a->enable ();
	    FXString temp;
	    for (int i = 0; i < (int) PARTIALS; i++) {
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
//        		_randVibratoAmpValues[i]->enable ();
//    			_randVibratoAmpSliders[i]->enable ();
				_randVibratoFreqValues[i]->enable ();
    			_randVibratoFreqSliders[i]->enable ();
  			}
     	}
   	}
    new FXTabItem (tabBook2,
        	_stringTable->get ("PANNING").c_str ());
	FXVerticalFrame* panningFrame = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
	FXHorizontalFrame* panningButtonFrame = new FXHorizontalFrame (panningFrame,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* panLogicBox = new FXGroupBox (panningButtonFrame,
 		_stringTable->get ("PRESET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
    FXComboBox* panCombo = new FXComboBox (panLogicBox, 0, this,
    	ID_PANNINGLOGIC,
    	COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK |
     	LAYOUT_SIDE_TOP);
    panCombo->setNumVisible (5);
	panCombo->appendItem (_stringTable->get ("CUSTOM").c_str ());
	panCombo->appendItem (_stringTable->get ("TWOTWO").c_str ());
	panCombo->appendItem (_stringTable->get ("ONETHREE").c_str ());
	panCombo->appendItem (_stringTable->get ("THREEONE").c_str ());
	panCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "PanningLogic"));
	FXGroupBox* controlsBox3 = new FXGroupBox (panningButtonFrame,
 		_stringTable->get ("CONTROLS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE);
	FXHorizontalFrame* matrix11 = new FXHorizontalFrame (controlsBox3,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    _defaultButton3 = new FXButton (matrix11,
    	_stringTable->get ("DEFAULTBUTTON").c_str (), NULL,
    	this, ID_DEFAULT3,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
    _randomButton3 = new FXButton (matrix11,
    	_stringTable->get ("RANDOMBUTTON").c_str (), NULL,
    	this, ID_RANDOM3,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
	FXGroupBox* panningBox = new FXGroupBox (panningFrame,
 		_stringTable->get ("POSITIONS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXVerticalFrame* positionFrame = new FXVerticalFrame (panningBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* panningSingleFrame[PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
       	panningSingleFrame[i] = new FXHorizontalFrame (positionFrame,
    		LAYOUT_FILL_X);
     	_panProgress[i] = new FXProgressBar (panningSingleFrame[i], 0, 0,
        	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE
         	| LAYOUT_CENTER_Y,
         	0,0,50);
     	FXString s;
		s.format ("P%d", i + 1);
    	_panProgress[i]->setProgress ((int)
     		(_config->number ("Harmonizer", s.text ()) * 100));
  		_panSliders[i] = new FXRealSlider (panningSingleFrame[i], this,
    		ID_PANNING1 + i, LAYOUT_FILL_X | LAYOUT_CENTER_Y |
      		SLIDER_INSIDE_BAR | SLIDER_HORIZONTAL);
  		_panSliders[i]->setRange (0, 1.0);
 		_panSliders[i]->setValue (_config->number ("Harmonizer",
			s.text ()));
		s.format ("On%d", i + 1);
		if ((bool) _config->number ("Harmonizer",
			s.text ()) == false) {
       		_panSliders[i]->disable ();
       	}
	}
    _defaultButton3->disable ();
    _randomButton3->disable ();
    for (int i = 0; i < PARTIALS; i++) {
    	_panSliders[i]->disable ();
		_panProgress[i]->disable ();
 	}
	if ((int) _config->number ("Harmonizer", "PanningLogic") == 0) {
	    _defaultButton3->enable ();
	    _randomButton3->enable ();
	    FXString temp;
	    for (int i = 0; i < (int) PARTIALS; i++) {
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
            	_panSliders[i]->enable ();
        		_panProgress[i]->enable ();
  			}
     	}
   	}
   	/////////******************///////////////
    new FXTabItem (tabBook2,
        	_stringTable->get ("FORMANTS").c_str ());
	FXVerticalFrame* formantsFrame = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
	FXHorizontalFrame* formantsButtonFrame = new FXHorizontalFrame (formantsFrame,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	FXGroupBox* formantLogicBox = new FXGroupBox (formantsButtonFrame,
 		_stringTable->get ("PRESET").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH,
  		0,0,200,0);
    FXComboBox* formantCombo = new FXComboBox (formantLogicBox, 0, this,
    	ID_FORMANTSLOGIC,
    	COMBOBOX_STATIC | LAYOUT_FILL_X | FRAME_SUNKEN | FRAME_THICK |
     	LAYOUT_SIDE_TOP);
    formantCombo->setNumVisible (5);
	formantCombo->appendItem (_stringTable->get ("CUSTOM").c_str ());
	formantCombo->appendItem (_stringTable->get ("MIXED").c_str ());
	formantCombo->appendItem (_stringTable->get ("FEMALE").c_str ());
	formantCombo->appendItem (_stringTable->get ("MALE").c_str ());
	formantCombo->setCurrentItem ((int)
		_config->number ("Harmonizer", "FormantsLogic"));
	FXGroupBox* controlsBox4 = new FXGroupBox (formantsButtonFrame,
 		_stringTable->get ("CONTROLS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE);
	FXHorizontalFrame* matrix11a = new FXHorizontalFrame (controlsBox4,
 		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
    _defaultButton4 = new FXButton (matrix11a,
    	_stringTable->get ("DEFAULTBUTTON").c_str (), NULL,
    	this, ID_DEFAULT4,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
    _randomButton4 = new FXButton (matrix11a,
    	_stringTable->get ("RANDOMBUTTON").c_str (), NULL,
    	this, ID_RANDOM4,
    	LAYOUT_CENTER_Y | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH, 0,0,80,0, 20,20);
	FXGroupBox* formantsBox = new FXGroupBox (formantsFrame,
 		_stringTable->get ("GENDER").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXVerticalFrame* genderFrame = new FXVerticalFrame (formantsBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* formantsSingleFrame[PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
       	formantsSingleFrame[i] = new FXHorizontalFrame (genderFrame,
    		LAYOUT_FILL_X);
//     	_formantProgress[i] = new FXProgressBar (panningSingleFrame[i], 0, 0,
//        	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE
//         	| LAYOUT_CENTER_Y,
//         	0,0,50);
     	FXString s;
		s.format ("FORM%d", i + 1);
//    	_formantProgress[i]->setProgress ((int)
//     		(_config->number ("Harmonizer", s.text ()) * 100));
		new FXLabel (formantsSingleFrame[i], _stringTable->get ("MALELABEL").c_str (), 0,
			LAYOUT_CENTER_Y, 0,0,20,0);
  		_formantsSliders[i] = new FXRealSlider (formantsSingleFrame[i], this,
    		ID_FORMANT1 + i, LAYOUT_FILL_X | LAYOUT_CENTER_Y |
      		SLIDER_INSIDE_BAR | SLIDER_HORIZONTAL | SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT);
  		_formantsSliders[i]->setRange (.5, 2.0);
  		_formantsSliders[i]->setTickDelta (.5);
 		_formantsSliders[i]->setValue (_config->number ("Harmonizer",
			s.text ()));
		s.format ("On%d", i + 1);
		if ((bool) _config->number ("Harmonizer",
			s.text ()) == false) {
       		_formantsSliders[i]->disable ();
       	}
		new FXLabel (formantsSingleFrame[i], _stringTable->get ("FEMALELABEL").c_str (), 0,
			LAYOUT_CENTER_Y, 0,0,20,0);
	}
    _defaultButton4->disable ();
    _randomButton4->disable ();
    for (int i = 0; i < PARTIALS; i++) {
    	_formantsSliders[i]->disable ();
//		_panProgress[i]->disable ();
 	}
	if ((int) _config->number ("Harmonizer", "FormantsLogic") == 0) {
	    _defaultButton4->enable ();
	    _randomButton4->enable ();
	    FXString temp;
	    for (int i = 0; i < (int) PARTIALS; i++) {
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
            	_formantsSliders[i]->enable ();
//        		_panProgress[i]->enable ();
  			}
     	}
   	}
   	//////////////////*************///////////////////
    new FXTabItem (tabBook2,
    	_stringTable->get ("SETUP").c_str ());
	FXVerticalFrame* setupFrame = new FXVerticalFrame (tabBook2,
 		FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y | LAYOUT_FILL_X
   		| LAYOUT_TOP,
     	0,0,0,0,4,4,4,4);
	FXGroupBox* activationBox = new FXGroupBox (setupFrame,
 		_stringTable->get ("ACTIVATION").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X);
  	FXCheckButton* voiceChecks[PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
	    FXString s;
	    s.format ("%s %d", _stringTable->get (
 			"VOICE").c_str (), 1 + i);
		voiceChecks[i] = new FXCheckButton (activationBox,
 			s.text (), this, ID_VOICECHECK1 + i, ICON_BEFORE_TEXT | LAYOUT_TOP
   			| LAYOUT_LEFT);
		s.format ("On%d", i + 1);
		voiceChecks[i]->setCheck ((bool) _config->number ("Harmonizer", s.text ()));
	}
	FXGroupBox* hpfilterBox = new FXGroupBox (setupFrame,
 		_stringTable->get ("HIGHPASSVOICES").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X);
	for (int i = 0; i < PARTIALS; i++) {
	    FXString s;
	    s.format ("%s %d", _stringTable->get (
 			"VOICE").c_str (), 1 + i);
		_voiceFilters[i] = new FXCheckButton (hpfilterBox,
 			s.text (), this, ID_VOICEFILTER1 + i, ICON_BEFORE_TEXT | LAYOUT_TOP
   			| LAYOUT_LEFT);
		s.format ("HP%d", i + 1);
		_voiceFilters[i]->setCheck ((bool) _config->number ("Harmonizer", s.text ()));
	}

	for (int i = 0; i < PARTIALS; i++) {
        FXString temp;
        temp.format ("On%d", i + 1);
        if (_config->number ("Harmonizer", temp.text ()) == true) {
       		_voiceFilters[i]->enable ();
		}
		else {
         	_voiceFilters[i]->disable ();
		}
 	}
#if defined (REPITCH_ONLY)
    FXButton* voicesSel =
#endif
    jollybutton = new FXButton (buttons, _stringTable->get ("VOICESCR").c_str (),
    	getApp ()->_voicesIcon, switcher, FXSwitcher::ID_OPEN_THIRD,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite);
	     	
#if defined (REPITCH_ONLY)
	voicesSel->hide ();
#endif

	// morph
	FXGroupBox* labelMorph = new FXGroupBox (switcher,
		_stringTable->get ("MORPHIZER").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	labelMorph->setFont (_titleFont);
	FXHorizontalFrame* matrix1Morph = new FXHorizontalFrame (labelMorph,
 		LAYOUT_FILL_X);
	FXGroupBox* morphEnvelopeBox = new FXGroupBox (matrix1Morph,
 		_stringTable->get ("ENVELOPE").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y,
  		0,0,200,0);
    FXComboBox* morphEnvelopeCombo = new FXComboBox (morphEnvelopeBox, 0, this,
    	ID_MORPHENVELOPE,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X | LAYOUT_CENTER_Y);
    morphEnvelopeCombo->setNumVisible (5);
    morphEnvelopeCombo->appendItem (_stringTable->get ("RECTANGLE").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("BARTLETT").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("GAUSSIAN").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("HAMMING").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("HANNING").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("BLACKMAN").c_str ());
    morphEnvelopeCombo->appendItem (_stringTable->get ("BLACKMANHARRIS").c_str ());
 	morphEnvelopeCombo->setCurrentItem ((int)
 		_config->number ("Morphizer", "WindowType"));
	FXGroupBox* morphAdBox = new FXGroupBox (matrix1Morph,
 		_stringTable->get ("ATTACKDECAY").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X,
  		0,0,150,0);
	FXVerticalFrame* morphAttackDecayFrame = new FXVerticalFrame (morphAdBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXHorizontalFrame* morphAttackFrame = new FXHorizontalFrame (morphAttackDecayFrame,
		LAYOUT_FILL_X);
 	_morphAttackProgress = new FXProgressBar (morphAttackFrame, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
    FXSlider* morphAttackSlider = new FXSlider (morphAttackFrame, this, ID_MORPHATTACK,
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 100);
	morphAttackSlider->setRange (0, 50);
	_morphAttackProgress->setProgress ((int) _config->number ("Morphizer", "Attack"));
	morphAttackSlider->setValue ((int) _config->number ("Morphizer", "Attack"));
	FXHorizontalFrame* morphDecayFrame = new FXHorizontalFrame (morphAttackDecayFrame,
		LAYOUT_FILL_X);
 	_morphDecayProgress = new FXProgressBar (morphDecayFrame, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
    FXSlider* morphDecaySlider = new FXSlider (morphDecayFrame, this, ID_MORPHDECAY,
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X | LAYOUT_CENTER_Y, 0, 0, 100);
	morphDecaySlider->setRange (0, 50);
	_morphDecayProgress->setProgress ((int) _config->number ("Morphizer", "Decay"));
	morphDecaySlider->setValue ((int) _config->number ("Morphizer", "Decay"));
	FXHorizontalFrame* matrix2Morph = new FXHorizontalFrame (labelMorph,
 		LAYOUT_FILL_X);
    FXGroupBox* phJitterBox = new FXGroupBox (matrix2Morph,
    	_stringTable->get ("PHASEJITTER").c_str (),
    	FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0,0,200,0);
    FXHorizontalFrame* phJitterFrame = new FXHorizontalFrame (phJitterBox,
		LAYOUT_FILL_X | PACK_UNIFORM_HEIGHT);
	_phJitterField = new FXTextField (phJitterFrame, 0, this, ID_PHJITTERTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
  		0,0,40,0);
	_phJitterDial = new FXDial (phJitterFrame, this, ID_PHJITTER, LAYOUT_FILL_X |
 		LAYOUT_CENTER_Y | DIAL_HORIZONTAL |
   		DIAL_HAS_NOTCH);
	double jit = _config->number ("Morphizer", "PhaseJitter");
	std::stringstream sjit;
	sjit << jit;
	_phJitterDial->setRange (0, 40000);
	_phJitterDial->setValue ((int) (jit));
	_phJitterField->setText (sjit.str ().c_str ());
	 FXGroupBox* widthBox = new FXGroupBox (matrix2Morph,
    	_stringTable->get ("SPATIALWIDTH").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
 	FXHorizontalFrame* widthFrame = new FXHorizontalFrame (widthBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_HEIGHT);
 	_widthProgress = new FXProgressBar (widthFrame, 0, 0,
    	LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE,
     	0,0,50);
 	FXRealSlider* widthSlider = new FXRealSlider (widthFrame, this, ID_WIDTH,
   		LAYOUT_CENTER_Y | SLIDER_HORIZONTAL | SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT |
   		LAYOUT_FILL_X); //, 0,0,150,0);
	widthSlider->setRange (0, 1);
	widthSlider->setTickDelta (.1);
	_widthProgress->setProgress ((int) (
		_config->number ("Morphizer", "SpatialJitter") * 100));
	widthSlider->setValue (
		_config->number ("Morphizer", "SpatialJitter"));
	FXHorizontalFrame* revAndSpeedFrame = new FXHorizontalFrame (labelMorph,
 		LAYOUT_FILL_X | LAYOUT_TOP);
	FXGroupBox* revGrainBox = new FXGroupBox (revAndSpeedFrame,
 		_stringTable->get ("GLOBAL").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FIX_WIDTH |
  		LAYOUT_FILL_Y, 0,0,200,0);
	FXHorizontalFrame* globSetFrame = new FXHorizontalFrame (revGrainBox,
 		LAYOUT_FILL_X | LAYOUT_TOP);
	_revGrainCheck = new FXCheckButton (globSetFrame,
 		_stringTable->get (
 		"REVGRAIN").c_str (), this, ID_REVGRAIN, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	_revGrainCheck->setCheck ((bool) _config->number ("Morphizer", "ReverseGrain"));
	if (getApp ()->_live == true) {
		_revGrainCheck->disable ();
	}

	_grainletCheck = new FXCheckButton (globSetFrame,
 		_stringTable->get (
 		"GRAINLET").c_str (), this, ID_GRAINLET, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	_grainletCheck->setCheck ((bool) _config->number ("Morphizer", "Grainlet"));
	_grainletCheck->hide ();
    FXGroupBox* speedFactorBox = new FXGroupBox (revAndSpeedFrame,
    	_stringTable->get ("SPEED").c_str (), // uses filter's speed label
    	FRAME_THICK | LAYOUT_FILL_Y | LAYOUT_FILL_X, 0,0,300,0, 4,4,4,4);
	FXHorizontalFrame* speedHFrame = new FXHorizontalFrame (speedFactorBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	_speedFactorField = new FXTextField (speedHFrame, 0, this,
  		ID_SPEEDFACTORTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,40,0, 4,4,4,4);
	std::stringstream speedfactor;
	speedfactor << _config->number ("Morphizer",
		"Speed");
	_speedFactorField->setText (speedfactor.str ().c_str ());
  	_speedFactorSlider = new FXRealSlider (speedHFrame, this,
    	ID_SPEEDFACTOR, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_speedFactorSlider->setRange (0, 2);
	_speedFactorSlider->setValue (_config->number ("Morphizer",
		"Speed"));
	if (getApp ()->_live == true) {
		_speedFactorField->disable ();
		_speedFactorSlider->disable ();
	}
	FXHorizontalFrame* morphParamFrame = new FXHorizontalFrame (labelMorph,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP);
	FXGroupBox* morphParamBoxes[PARTIALS];
	FXHorizontalFrame* singleMorphParamFrames[PARTIALS];
	FXVerticalFrame* morphParamColumns[2 * PARTIALS];
	for (int i = 0; i < PARTIALS; i++) {
	    std::stringstream v;
	    std::stringstream a;
 	    std::stringstream a1;
 	    std::stringstream f;
   	    std::stringstream f1;
	    v << _stringTable->get (params[i]);
    	morphParamBoxes[i] = new FXGroupBox (morphParamFrame,
 			v.str ().c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X |
  		LAYOUT_FILL_Y);
    	singleMorphParamFrames[i] = new FXHorizontalFrame (morphParamBoxes[i],
     		LAYOUT_FILL_Y | LAYOUT_FILL_X);
  		morphParamColumns[i] = new FXVerticalFrame (singleMorphParamFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
  		morphParamColumns[i + 1] = new FXVerticalFrame (singleMorphParamFrames[i],
    		LAYOUT_FILL_Y | LAYOUT_CENTER_X);
		_morphDetParamValues[i] = new FXTextField (morphParamColumns[i], 0, this,
  			ID_MORPH1DETTEXT + i,
			FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		a << "Center" << i + 1;
		a1 << _config->number ("Morphizer",
			a.str ());
		_morphDetParamValues[i]->setText (a1.str ().c_str ());
  		_morphDetParamSliders[i] = new FXRealSlider (morphParamColumns[i], this,
    		ID_MORPH1DET + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
        switch (i) {
        	case 0: // density
	 	 		_morphDetParamSliders[i]->setRange (1, 512);
 				_morphDetParamSliders[i]->setTickDelta (50);
 			break;
        	case 1: // duration
	 	 		_morphDetParamSliders[i]->setRange (0, 5);
 				_morphDetParamSliders[i]->setTickDelta (.5);
 			break;
        	case 2: // frequency
	 	 		_morphDetParamSliders[i]->setRange (.5, 2);
 				_morphDetParamSliders[i]->setTickDelta (.1);
 			break;
        	case 3: // glissato
	 	 		_morphDetParamSliders[i]->setRange (.5, 2);
 				_morphDetParamSliders[i]->setTickDelta (.1);
 			break;

        }
 		_morphDetParamSliders[i]->setValue (_config->number ("Morphizer",
			a.str ()));
  		new FXLabel (morphParamColumns[i], "Center", 0, LAYOUT_CENTER_X);
		_morphRandParamValues[i] = new FXTextField (morphParamColumns[i + 1], 0,
  			this, ID_MORPH1RANDTEXT + i, FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  			| LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X,
			0,0,30,0, 4,4,4,4);
		f << "Random" << i + 1;
		f1 << _config->number ("Morphizer",
			f.str ());
		_morphRandParamValues[i]->setText (f1.str ().c_str ());
  		_morphRandParamSliders[i] = new FXRealSlider (morphParamColumns[i + 1], this,
    		ID_MORPH1RAND + i, LAYOUT_CENTER_Y | SLIDER_VERTICAL |
      		SLIDER_TICKS_RIGHT | SLIDER_TICKS_LEFT | LAYOUT_FILL_Y |
        	LAYOUT_FIX_WIDTH | LAYOUT_CENTER_X, 0,0,20,0);
  		_morphRandParamSliders[i]->setRange (0, 1);
 		_morphRandParamSliders[i]->setTickDelta (.1);
 		_morphRandParamSliders[i]->setValue (_config->number ("Morphizer",
			f.str ()));
  		new FXLabel (morphParamColumns[i + 1], "Random", 0, LAYOUT_CENTER_X);
	}

	if ((int) _config->number ("Morphizer", "Grainlet") == 1) { // duration on/off
		_morphDetParamValues[1]->disable ();
		_morphDetParamSliders[1]->disable ();
		_morphRandParamValues[1]->disable ();
		_morphRandParamSliders[1]->disable ();
	}
    jollybutton = new FXButton (buttons, _stringTable->get ("MORPHIZERCR").c_str (),
		getApp ()->_morphIcon, switcher, FXSwitcher::ID_OPEN_FOURTH,
 		FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite); 		

	// mix
	FXGroupBox* label4 = new FXGroupBox (switcher,
 		_stringTable->get ("MIX").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FILL_Y);
	label4->setFont (_titleFont);
	FXVerticalFrame* matrix12 = new FXVerticalFrame (label4,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXGroupBox* globalBox = new FXGroupBox (matrix12,
    	_stringTable->get ("GLOBAL").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
	FXVerticalFrame* globalFrame = new FXVerticalFrame (globalBox,
		LAYOUT_FILL_X);
	FXCheckButton* bypassCheck = new FXCheckButton (globalFrame,
 		_stringTable->get (
 		"BYPASS").c_str (), this, ID_BYPASS, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	bypassCheck->setCheck ((bool) _config->number ("Harmonizer", "Bypass"));
	FXCheckButton* freezeCheck = new FXCheckButton (globalFrame,
 		_stringTable->get (
 		"FREEZE").c_str (), this, ID_FREEZE, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	freezeCheck->setCheck ((bool) _config->number ("Harmonizer", "Freeze"));
	FXCheckButton* trackPitchCheck = new FXCheckButton (globalFrame,
 		_stringTable->get (
 		"TRACKPITCH").c_str (), this, ID_TRACKPITCH, ICON_BEFORE_TEXT | LAYOUT_TOP
   		| LAYOUT_LEFT);
	trackPitchCheck->setCheck ((bool) _config->number ("Harmonizer", "TrackPitch"));
    FXGroupBox* reverbBox = new FXGroupBox (matrix12,
    	_stringTable->get ("REVERB").c_str (),
    	FRAME_THICK | LAYOUT_FILL_X);
	FXHorizontalFrame* reverbFrame = new FXHorizontalFrame (reverbBox,
		LAYOUT_FILL_X);
	_reverbField = new FXTextField (reverbFrame, 0, this,
  		ID_REVERBTEXT,
		FRAME_SUNKEN | JUSTIFY_RIGHT | TEXTFIELD_REAL
  		| LAYOUT_FIX_WIDTH | LAYOUT_LEFT | LAYOUT_CENTER_Y,
		0,0,30,0, 4,4,4,4);
	std::stringstream revs;
	revs << _config->number ("Harmonizer",
		"T60");
	_reverbField->setText (revs.str ().c_str ());
  	_reverbSlider = new FXRealSlider (reverbFrame, this,
    	ID_REVERB, LAYOUT_CENTER_Y | LAYOUT_LEFT |
    	SLIDER_INSIDE_BAR | LAYOUT_FILL_X);
	_reverbSlider->setRange (0, 20);
	_reverbSlider->setValue (_config->number ("Harmonizer",
		"T60"));
	FXHorizontalFrame* levelsFrame = new FXHorizontalFrame (matrix12,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXSpring* spring1 = new FXSpring (levelsFrame,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y, 1,0, 0,0,0,0, 0,0,0,0);
	FXGroupBox* sourceBox = new FXGroupBox (spring1,
 		_stringTable->get ("SOURCE").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X);
	FXVerticalFrame* vFrame1 = new FXVerticalFrame (sourceBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXDial* sourceDial = new FXDial (vFrame1, this, ID_LEVEL1, LAYOUT_FIX_WIDTH |
 		LAYOUT_FIX_HEIGHT | LAYOUT_CENTER_Y | LAYOUT_CENTER_X | DIAL_VERTICAL |
   		DIAL_HAS_NOTCH, 0,0,40,200);
	sourceDial->setRange (0, 100);
 	sourceDial->setValue ((int)
     		(_config->number ("Harmonizer", "SourceLevel") * 100));
 	_sourceProgress = new FXProgressBar (vFrame1, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE | LAYOUT_CENTER_X,
     	0,0,50);
	_sourceProgress->setProgress ((int)
     		(_config->number ("Harmonizer", "SourceLevel") * 100));
	FXSpring* spring2 = new FXSpring (levelsFrame,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y, 1,0, 0,0,0,0, 0,0,0,0);
	FXGroupBox* harmonyBox = new FXGroupBox (spring2,
 		_stringTable->get ("HARMONY").c_str (),
		LAYOUT_SIDE_TOP | FRAME_GROOVE | LAYOUT_FILL_X);
	FXVerticalFrame* vFrame2 = new FXVerticalFrame (harmonyBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXDial* harmonyDial = new FXDial (vFrame2, this, ID_LEVEL2, LAYOUT_FIX_WIDTH |
 		LAYOUT_FIX_HEIGHT | LAYOUT_CENTER_Y | LAYOUT_CENTER_X | DIAL_VERTICAL |
   		DIAL_HAS_NOTCH, 0,0,40,200);
	harmonyDial->setRange (0, 100);
 	harmonyDial->setValue ((int)
     		(_config->number ("Harmonizer", "HarmonyLevel") * 100));
 	_harmonyProgress = new FXProgressBar (vFrame2, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE | LAYOUT_CENTER_X,
     	0,0,50);
	_harmonyProgress->setProgress ((int)
     		(_config->number ("Harmonizer", "HarmonyLevel") * 100));
	FXSpring* spring3 = new FXSpring (levelsFrame,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y, 1,0, 0,0,0,0, 0,0,0,0);
	FXGroupBox* morphBox = new FXGroupBox (spring3,
 		_stringTable->get ("MORPHIZER").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X);
	FXVerticalFrame* vFrame3 = new FXVerticalFrame (morphBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXDial* morphDial = new FXDial (vFrame3, this, ID_LEVEL3, LAYOUT_FIX_WIDTH |
 		LAYOUT_FIX_HEIGHT | LAYOUT_CENTER_Y | LAYOUT_CENTER_X | DIAL_VERTICAL |
   		DIAL_HAS_NOTCH, 0,0,40,200);
	morphDial->setRange (0, 100);
 	morphDial->setValue ((int)
     		(_config->number ("Morphizer", "OutputLevel") * 100));
 	_morphProgress = new FXProgressBar (vFrame3, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE | LAYOUT_CENTER_X,
     	0,0,50);
	_morphProgress->setProgress ((int)
     		(_config->number ("Morphizer", "OutputLevel") * 100));
	FXSpring* spring4 = new FXSpring (levelsFrame,
 		LAYOUT_FILL_X | LAYOUT_FILL_Y, 1,0, 0,0,0,0, 0,0,0,0);
	FXGroupBox* effectsBox = new FXGroupBox (spring4,
 		_stringTable->get ("EFFECTS").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_RIGHT | FRAME_GROOVE | LAYOUT_FILL_X);
	FXVerticalFrame* vFrame4 = new FXVerticalFrame (effectsBox,
		LAYOUT_FILL_X | LAYOUT_FILL_Y);
	FXDial* effectsDial = new FXDial (vFrame4, this, ID_LEVEL4, LAYOUT_FIX_WIDTH |
 		LAYOUT_FIX_HEIGHT | LAYOUT_CENTER_Y | LAYOUT_CENTER_X | DIAL_VERTICAL |
   		DIAL_HAS_NOTCH, 0,0,40,200);
	effectsDial->setRange (0, 100);
 	effectsDial->setValue ((int)
     		(_config->number ("Harmonizer", "EffectsLevel") * 100));
 	_effectsProgress = new FXProgressBar (vFrame4, 0, 0,
    	LAYOUT_FIX_WIDTH | PROGRESSBAR_PERCENTAGE | FRAME_GROOVE | LAYOUT_CENTER_X,
     	0,0,50);
	_effectsProgress->setProgress ((int)
     		(_config->number ("Harmonizer", "EffectsLevel") * 100));
#if defined (REPITCH_ONLY)
    FXButton* mixSel =
#endif
    jollybutton = new FXButton (buttons, _stringTable->get ("MIXCR").c_str (),
    	getApp ()->_mixIcon, switcher, FXSwitcher::ID_OPEN_FIFTH,
     	FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y);
	jollybutton->setBorderColor (m_theme.border);
	jollybutton->setBaseColor (m_theme.base);
	jollybutton->setBackColor (m_theme.base);
	jollybutton->setTextColor (m_theme.fore);
	jollybutton->setShadowColor (m_shadow);
	jollybutton->setHiliteColor (m_hilite);
	     	
#if defined (REPITCH_ONLY)
	mixSel->hide ();
#endif
}

void MainWindow::setupColors (const ColorTheme& t) {
	m_theme.base = t.base;
	m_theme.border = t.border;
	m_theme.back = t.back;
	m_theme.fore = t.fore;
	m_theme.selfore = t.selfore;
	m_theme.selback = t.selback;
	m_theme.menufore = t.menufore;
	m_theme.menuback = t.menuback;
	m_theme.tipfore = t.tipfore;
	m_theme.tipback = t.tipback;
   
	m_shadow = makeShadowColor (m_theme.base);
	m_hilite = makeHiliteColor (m_theme.base);
}

// EOF

