// MainWindow.cpp
//

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "callbacks.h"
#include "MainWindow.h"
#include "SettingsDialog.h"
#include "SplashScreen.h"

#include "FXJPGIcon.h"
#include "gui.h"

#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//	#define TRACE_ON
//	#include "CodeTrace.h"
#endif

#include <cmath>

extern double standardSampleRates[];
extern bool trackPitch;

FXList* msgList; // global for better use...

extern const int TIMEOUT = 10;
void* userData = 0;

#define BUG { \
	FXMessageBox a (this, "Is this a bug...", \
		"...or not??", 0, MBOX_OK); \
	a.execute (); \
}

#ifdef ITALIAN_LANG
	char* noteNames[12] = {
		"do",
		"do#",
		"re",
		"re#",
		"mi",
		"fa",
		"fa#",
		"sol",
		"sol#",
		"la",
		"la#",
		"si"
	};
#else
	char* noteNames[12] = {
		"c",
		"c#",
		"d",
		"d#",
		"e",
		"f",
		"f#",
		"g",
		"g#",
		"a",
		"a#",
		"b"
	};
#endif

const ColorTheme ColorThemes[] = {
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"FOX"               ,FXRGB(212,208,200),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 36,106),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 36,106),FXRGB(255,255,255)},
  {"Gnome"             ,FXRGB(214,215,214),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"Atlas Green"       ,FXRGB(175,180,159),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(111,122, 99),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(111,122, 99),FXRGB(255,255,255)},
  {"BeOS"              ,FXRGB(217,217,217),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(168,168,168),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(168,168,168),FXRGB(  0,  0,  0)},
  {"Blue Slate"        ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"CDE"	       ,FXRGB(156,153,156),FXRGB(  0,  0,  0),FXRGB(131,129,131),FXRGB(255,255,255),FXRGB( 49, 97,131),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 49, 97,131),FXRGB(255,255,255)},
  {"Digital CDE"       ,FXRGB( 74,121,131),FXRGB(  0,  0,  0),FXRGB( 55, 77, 78),FXRGB(255,255,255),FXRGB( 82,102,115),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 82,102,115),FXRGB(255,255,255)},
  {"Dark Blue"         ,FXRGB( 66,103,148),FXRGB(  0,  0,  0),FXRGB(  0, 42, 78),FXRGB(255,255,255),FXRGB( 92,179,255),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 92,179,255),FXRGB(  0,  0,  0)},
  {"Desert FOX"        ,FXRGB(214,205,187),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(128,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(128,  0,  0),FXRGB(255,255,255)},
  {"EveX"              ,FXRGB(230,222,220),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Galaxy"            ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"iMac"              ,FXRGB(205,206,205),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"KDE 1"             ,FXRGB(192,192,192),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
  {"KDE 2"             ,FXRGB(220,220,220),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 10, 95,137),FXRGB(255,255,255)},
  {"KDE 3"             ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(255,221,118),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(255,221,118),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Keramik"           ,FXRGB(234,233,232),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(169,209,255),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(169,209,255),FXRGB(  0,  0,  0)},
  {"Keramik Emerald"   ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(134,204,134),FXRGB(  0,  0,  0),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(134,204,134),FXRGB(  0,  0,  0)},
  {"Keramik White"     ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"Mandrake"          ,FXRGB(230,231,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 33, 68,156),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 33, 68,156),FXRGB(255,255,255)},
  {"Media Peach"       ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Next"              ,FXRGB(168,168,168),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255)},
  {"Pale Gray"         ,FXRGB(214,214,214),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,  0),FXRGB(255,255,255)},
  {"Plastik"           ,FXRGB(239,239,239),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(103,141,178),FXRGB(255,255,255)},
  {"Pumpkin"           ,FXRGB(238,216,174),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(205,133, 63),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(205,133, 63),FXRGB(255,255,255)},
  {"Redmond 95"        ,FXRGB(195,195,195),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0,  0,128),FXRGB(255,255,255)},
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
//|        Name        |        Base      |       Border     |       Back       |      Fore        |      Selback     |      Selfore     |      Tipback     |     Tipfore      |      Menuback    |      Menufore    |
//|--------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------+------------------|
  {"Redmond 2000"      ,FXRGB(212,208,200),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(  0, 36,104),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(  0, 36,104),FXRGB(255,255,255)},
  {"Redmond XP"        ,FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255)},
  {"Solaris"           ,FXRGB(174,178,195),FXRGB(  0,  0,  0),FXRGB(147,151,165),FXRGB(  0,  0,  0),FXRGB(113,139,165),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(113,139,165),FXRGB(255,255,255)},
  {"Storm"             ,FXRGB(192,192,192),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB(139,  0,139),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB(139,  0,139),FXRGB(255,255,255)},
  {"Sea Sky"           ,FXRGB(165,178,198),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 49,101,156),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 49,101,156),FXRGB(255,255,255)},
  };

const FXint numThemes = ARRAYNUMBER (ColorThemes);

FXDEFMAP (MainWindow) MainWindowMap[] = {
	FXMAPFUNC (SEL_SIGNAL,    MainWindow::ID_FILEQUIT,       	MainWindow::onCmdFileQuit),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILEOPEN,       	MainWindow::onCmdFileOpen),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILEDUMP,       	MainWindow::onCmdFileDump),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILEQUIT,       	MainWindow::onCmdFileQuit),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILERESETQUIT,   	MainWindow::onCmdFileResetAndQuit),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RUNPLAY,        	MainWindow::onCmdRunPlay),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RUNRECORD,      	MainWindow::onCmdRunRecord),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RUNENABLE,      	MainWindow::onCmdRunEnable),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_TOOLSSAVEMON,   	MainWindow::onCmdToolsSaveMon),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_TOOLSCLEARMON,  	MainWindow::onCmdToolsClearMon),
	#ifdef _WIN32
 		FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_TOOLSSNDVOL,  	MainWindow::onCmdToolsSndVol),
	#endif
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_TOOLSSETTINGS,  	MainWindow::onCmdToolsSettings),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_HELPCONTENTS, 	 	MainWindow::onCmdHelpContents),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_HELPWEB,    	 	MainWindow::onCmdHelpWeb),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_HELPMAIL,    	 	MainWindow::onCmdHelpMail),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_HELPABOUT,    	 	MainWindow::onCmdHelpAbout),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_SPLASHSCREEN, 	 	MainWindow::onCmdSplashScreen),
 	FXMAPFUNC (SEL_KEYPRESS,  0,			     	         	MainWindow::onKeyPress),
    FXMAPFUNC (SEL_TIMEOUT,   MainWindow::ID_TIMER,          	MainWindow::onTimer),

    // reVoice specific...
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_HELPPROTON,		MainWindow::onCmdHelpProton),

//	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEOPTION1,  	MainWindow::onCmdVoiceOptions),
//	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEOPTION2,  	MainWindow::onCmdVoiceOptions),
//	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEOPTION3,  	MainWindow::onCmdVoiceOptions),
//	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEOPTION4,  	MainWindow::onCmdVoiceOptions),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PANIC,     		MainWindow::onCmdPanic),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PHIRESET,     		MainWindow::onCmdPhiReset),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MULTIPLIER,   		MainWindow::onCmdMultiplier),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_WAVEFORM,      	MainWindow::onCmdWaveform),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_BROWSE,        	MainWindow::onCmdFileBrowse),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_LOGIC,         	MainWindow::onCmdLogic),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VIBRATOLOGIC,      MainWindow::onCmdVibratoLogic),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDVIBRATOLOGIC,  MainWindow::onCmdRandVibratoLogic),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PANNINGLOGIC,      MainWindow::onCmdPanningLogic),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FORMANTSLOGIC,     MainWindow::onCmdFormantsLogic),
 	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_DEFAULT,       	MainWindow::onCmdDefault),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDOM,        	MainWindow::onCmdRandom),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_DEFAULT2,       	MainWindow::onCmdDefault2),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDOM2,        	MainWindow::onCmdRandom2),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_DEFAULT2A,       	MainWindow::onCmdDefault2a),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDOM2A,        	MainWindow::onCmdRandom2a),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_DEFAULT3,       	MainWindow::onCmdDefault3),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDOM3,        	MainWindow::onCmdRandom3),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_DEFAULT4,       	MainWindow::onCmdDefault4),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RANDOM4,        	MainWindow::onCmdRandom4),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A1,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A2,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A3,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A4,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F1,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F2,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F3,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F4,            	MainWindow::onCmdSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A1TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A2TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A3TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_A4TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F1TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F2TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F3TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_F4TEXT,        	MainWindow::onCmdSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA1,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA2,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA3,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA4,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF1,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF2,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF3,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF4,            	MainWindow::onCmdVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA1TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA2TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA3TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VA4TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF1TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF2TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF3TEXT,        	MainWindow::onCmdVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VF4TEXT,        	MainWindow::onCmdVibratoSliderText),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND1,          	MainWindow::onCmdRandVibratoSlider),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND2,          	MainWindow::onCmdRandVibratoSlider),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND3,          	MainWindow::onCmdRandVibratoSlider),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND4,          	MainWindow::onCmdRandVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND1,          	MainWindow::onCmdRandVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND2,          	MainWindow::onCmdRandVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND3,          	MainWindow::onCmdRandVibratoSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND4,          	MainWindow::onCmdRandVibratoSlider),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND1TEXT,      	MainWindow::onCmdRandVibratoSliderText),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND2TEXT,      	MainWindow::onCmdRandVibratoSliderText),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND3TEXT,      	MainWindow::onCmdRandVibratoSliderText),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VARAND4TEXT,      	MainWindow::onCmdRandVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND1TEXT,      	MainWindow::onCmdRandVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND2TEXT,      	MainWindow::onCmdRandVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND3TEXT,      	MainWindow::onCmdRandVibratoSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_VFRAND4TEXT,      	MainWindow::onCmdRandVibratoSliderText),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICECHECK1,      	MainWindow::onCmdVoiceCheck),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICECHECK2,      	MainWindow::onCmdVoiceCheck),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICECHECK3,      	MainWindow::onCmdVoiceCheck),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICECHECK4,      	MainWindow::onCmdVoiceCheck),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEFILTER1,      MainWindow::onCmdVoiceFilter),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEFILTER2,     	MainWindow::onCmdVoiceFilter),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEFILTER3,     	MainWindow::onCmdVoiceFilter),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_VOICEFILTER4,     	MainWindow::onCmdVoiceFilter),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PANNING1,         	MainWindow::onCmdPanSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PANNING2,         	MainWindow::onCmdPanSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PANNING3,         	MainWindow::onCmdPanSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PANNING4,         	MainWindow::onCmdPanSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FORMANT1,         	MainWindow::onCmdFormantSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FORMANT2,         	MainWindow::onCmdFormantSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FORMANT3,         	MainWindow::onCmdFormantSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FORMANT4,         	MainWindow::onCmdFormantSlider),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RADIO1,        	MainWindow::onCmdRadio),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RADIO2,        	MainWindow::onCmdRadio),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_RADIO3,        	MainWindow::onCmdRadio),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_INTERVAL,      	MainWindow::onCmdSemitones),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MANUAL,        	MainWindow::onCmdManual),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MANUALTEXT,    	MainWindow::onCmdManualText),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_ENVELOPE,      	MainWindow::onCmdEnvelope),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_ATTACK,        	MainWindow::onCmdAttackDecay),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_DECAY,         	MainWindow::onCmdAttackDecay),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_MORPHENVELOPE,     MainWindow::onCmdMorphEnvelope),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPHATTACK,       MainWindow::onCmdMorphAttackDecay),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPHDECAY,        MainWindow::onCmdMorphAttackDecay),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_AUTOMODE,      	MainWindow::onCmdAutoMode),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_CYCLES,   	        MainWindow::onCmdCycles),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FREQTHRESHOLD, 	MainWindow::onCmdFreqThreshold),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FREQTHRESHOLDTEXT, MainWindow::onCmdFreqThresholdText),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_SEMITONESTHRESH,   MainWindow::onCmdSemitonesThresh),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_MASTERKEY,   		MainWindow::onCmdMasterKey),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_SCALE,   			MainWindow::onCmdScale),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_JITTER,        	MainWindow::onCmdJitter),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PDET1,        	    MainWindow::onCmdPitchDetector),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PDET2,        	    MainWindow::onCmdPitchDetector),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_PDET3,        	    MainWindow::onCmdPitchDetector),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_TRACKPITCH,      	MainWindow::onCmdTrackPitch),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_BYPASS,      		MainWindow::onCmdBypass),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FREEZE,      		MainWindow::onCmdFreeze),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_ANTIRUMBLE,   		MainWindow::onCmdAntiRumble),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FEEDBACK, 		    MainWindow::onCmdFeedback),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_FEEDBACKTEXT, 	    MainWindow::onCmdFeedbackText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITY, 			MainWindow::onCmdQuality),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITYTEXT, 		MainWindow::onCmdQualityText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPLIT, 			MainWindow::onCmdSplit),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_SPLIT, 			MainWindow::onCmdSplit),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLD, 	    MainWindow::onCmdAmpThreshold),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLDTEXT,  MainWindow::onCmdAmpThresholdText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_ZCCYCLES, 			MainWindow::onCmdZCCycles),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILTERS,     		MainWindow::onCmdFilters),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUAL,   		MainWindow::onCmdSmoothingQual),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUALTEXT,	MainWindow::onCmdSmoothingQualText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHING,        	MainWindow::onCmdSmoothing),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHINGTEXT,    	MainWindow::onCmdSmoothingText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGESTART1, 		MainWindow::onCmdRangeStartEnd1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGESTARTTEXT1, 	MainWindow::onCmdRangeStartEndText1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGEEND1, 		MainWindow::onCmdRangeStartEnd1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGEENDTEXT1, 	MainWindow::onCmdRangeStartEndText1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITY1, 			MainWindow::onCmdQuality1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITYTEXT1, 		MainWindow::onCmdQualityText1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLD1,	    MainWindow::onCmdAmpThreshold1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLDTEXT1, MainWindow::onCmdAmpThresholdText1),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPEED1, 			MainWindow::onCmdSpeed1),
//	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPEEDTEXT1, 		MainWindow::onCmdSpeedText1),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FILTERS1,     		MainWindow::onCmdFilters1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUAL1,  		MainWindow::onCmdSmoothingQual1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUALTEXT1,	MainWindow::onCmdSmoothingQualText1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHING1,       	MainWindow::onCmdSmoothing1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHINGTEXT1,   	MainWindow::onCmdSmoothingText1),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGESTART2, 		MainWindow::onCmdRangeStartEnd2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGESTARTTEXT2, 	MainWindow::onCmdRangeStartEndText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGEEND2, 		MainWindow::onCmdRangeStartEnd2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_RANGEENDTEXT2, 	MainWindow::onCmdRangeStartEndText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITY2, 			MainWindow::onCmdQuality2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_QUALITYTEXT2, 		MainWindow::onCmdQualityText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLD2,	    MainWindow::onCmdAmpThreshold2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_AMPTHRESHOLDTEXT2, MainWindow::onCmdAmpThresholdText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPLIT2, 			MainWindow::onCmdSplit2),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_SPLIT2, 			MainWindow::onCmdSplit2),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_FFTSIZE,     		MainWindow::onCmdFFTSize),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUAL2,  		MainWindow::onCmdSmoothingQual2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHQUALTEXT2,	MainWindow::onCmdSmoothingQualText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHING2,       	MainWindow::onCmdSmoothing2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SMOOTHINGTEXT2,   	MainWindow::onCmdSmoothingText2),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_LEVEL1,     		MainWindow::onCmdMixer),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_LEVEL2,     		MainWindow::onCmdMixer),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_LEVEL3,     		MainWindow::onCmdMixer),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_LEVEL4,     		MainWindow::onCmdMixer),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_REVERB,            MainWindow::onCmdReverb),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_REVERBTEXT,        MainWindow::onCmdReverbText),
	// morph
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH1DET,         MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH2DET,         MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH3DET,         MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH4DET,         MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH1RAND,        MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH2RAND,        MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH3RAND,        MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH4RAND,       	MainWindow::onCmdMorphSlider),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH1DETTEXT,    	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH2DETTEXT,    	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH3DETTEXT,    	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH4DETTEXT,    	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH1RANDTEXT,    MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH2RANDTEXT,   	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH3RANDTEXT,   	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_MORPH4RANDTEXT,   	MainWindow::onCmdMorphSliderText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PHJITTER,        	MainWindow::onCmdPhJitter),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_PHJITTERTEXT,    	MainWindow::onCmdPhJitterText),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_WIDTH,        		MainWindow::onCmdWidth),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPEEDFACTOR, 		MainWindow::onCmdSpeedFactor),
	FXMAPFUNC (SEL_CHANGED,   MainWindow::ID_SPEEDFACTORTEXT, 	MainWindow::onCmdSpeedFactorText),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_REVGRAIN,   		MainWindow::onCmdReverseGrain),
	FXMAPFUNC (SEL_COMMAND,   MainWindow::ID_GRAINLET,   		MainWindow::onCmdGrainlet),
};


FXIMPLEMENT (MainWindow, FXMainWindow, MainWindowMap,
	ARRAYNUMBER (MainWindowMap))

MainWindow::MainWindow (MyApp* app)
    : FXMainWindow (app, "", 0, 0, DECOR_ALL,
	0, 0, 0, 0) {
	_config = Configure::instance ();
	position ((int) _config->number ("Windows", "MainWindowXPos"),
 		(int) _config->number ("Windows", "MainWindowYPos"),
   		(int) _config->number ("Windows", "MainWindowWidth"),
  		(int) _config->number ("Windows", "MainWindowHeight"));

	_stringTable = StringTable::instance ();
	setTitle (_stringTable->get ("TITLE").c_str ());

	setIcon (app->_bigIcon);
 	setMiniIcon (app->_smallIcon);

	setupColors (ColorThemes[5]);
	guiMaker (app);
	setBackColor (m_theme.base);

    _ready = true;
}

MainWindow::~MainWindow () {
	delete _fileMenu;
	delete _viewMenu;
	delete _runMenu;
	delete _toolsMenu;
	delete _MIDIMonSubMenu;
	delete _helpMenu;
	delete _titleFont;

	getApp ()->removeTimeout ((FXObject*) _timer, ID_TIMER);

	this->handle (this, FXSEL (SEL_COMMAND, ID_FILEQUIT), NULL);
}

void MainWindow::create () {
  	FXMainWindow::create ();
}

MyApp* MainWindow::getApp () const {
    return (MyApp*) FXMainWindow::getApp ();
}

long MainWindow::onCmdFileOpen (FXObject*, FXSelector, void*) {
    FXFileDialog openDialog (this,_stringTable->get ("OPENFILE").c_str (),
    	DECOR_NONE);
    openDialog.setSelectMode (SELECTFILE_EXISTING);
    //openDialog.setPatternList ("Text files (*.txt)");
    openDialog.setDirectory (_config->literal ("Global",
    	"LastDirectory").c_str ());
    if (openDialog.execute ()){
        _config->literal ("Global", "LastDirectory",
        	openDialog.getDirectory ().text ());
        // FXString file = openDialog.getFilename ();
    }

    return 1;
}

long MainWindow::onCmdFileDump (FXObject*, FXSelector, void*) {
    time_t now;
    time (&now);
   	tm* nowtm = localtime (&now);

	std::stringstream file;
	file << _config->vendor ();
	file << ".";
	file << _config->program ();
	file << ".dump.";
	file << nowtm->tm_mday << "-";
	int a = nowtm->tm_mon + 1;
	file << a << "-";
	a = nowtm->tm_year + 1900;
	file << a;
	file << "@";
	file << nowtm->tm_hour;
	file << ".";
	file << nowtm->tm_min;
	file << ".txt";
	std::ofstream out (file.str ().c_str ());
    _config->dump (out);
    out.close ();

    std::string s = "File: ";
    s += file.str ();
    FX::FXMessageBox::information (this, MBOX_OK,
    	_stringTable->get ("TITLE").c_str (),
    	s.c_str ());
    return 1;
}

long MainWindow::onCmdFileQuit (FXObject*, FXSelector, void*) {
    _config->number ("Windows", "ToolBarVisible", (double)
    	_toolBar->shown ());
    _config->number ("Windows", "StatusBarVisible", (double)
    	_statusBar->shown ());
    _config->number ("Windows", "ControlPanelVisible", (double)
    	_bookFrame->shown ());
    _config->number ("Windows", "MainWindowXPos", (double) getX ());
	_config->number ("Windows", "MainWindowYPos", (double) getY ());
    _config->number ("Windows", "MainWindowHeight", (double) getHeight ());
	_config->number ("Windows", "MainWindowWidth", (double) getWidth ());

	getApp ()->exit ();
 	return 1;
}

long MainWindow::onCmdFileResetAndQuit (FXObject*, FXSelector, void*) {
	if (FX::FXMessageBox::question (this, FX::MBOX_OK_CANCEL,
		_stringTable->get ("TITLE").c_str (),
		_stringTable->get ("AREYOUSURE").c_str ()) == MBOX_CLICKED_OK) {
		getApp ()->exit (1);
	}
 	return 1;
}

long MainWindow::onCmdRunPlay (FXObject*, FXSelector, void *) {
	if (getApp ()->_rose->isRunning () == 1) {
    	int err = getApp ()->_rose->stop ();
    	if (err) {
		    std::string s;
		    s = _stringTable->get ("ENGINEERROR").c_str ();
      		s += " (stop): ";
        	s += getApp ()->_rose->decodeError (err);
			FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
				s.c_str (), 0, MBOX_OK);
			a.execute ();
			_ch3->setCheck (true);
			_playButton->setState (true);
			return 1;
      	}
		else {
			_ch3->setCheck (false);
			_playButton->setState (false);

			getApp ()->_left->reset ();
			getApp ()->_right->reset ();
        	return 1;
      	}
	}
	else {
    	int recId = getApp ()->_rose->translateRecordNumber (
     		(int) _config->number ("Devices",
      			"AudioRecording"));
		if (recId < 0) {
     		recId = 0;
			_config->number ("Devices", "AudioRecording", 0);
  		}
    	int playId = getApp ()->_rose->translatePlaybackNumber (
     		(int) _config->number ("Devices",
      			"AudioPlayback"));
		if (playId < 0) {
     		playId = 0;
			_config->number ("Devices", "AudioPlayback", 0);
  		}

	    getApp ()->_rose->frames ((int) _config->number ("Devices",
			"EngineFrames"));
	    getApp ()->_rose->recordDevice (recId);
		getApp ()->_rose->playbackDevice (playId);

		int err = -1;
		switch ((int) _config->number ("Devices", "InputChannels")) {
		    default:
		    case 0:
				err = getApp ()->_rose->start (audioCallback32NoInput, 0,
    				(void*) getApp ());
			break;
			case 1:
			    err = getApp ()->_rose->start (audioCallback32Mono, 1,
       				(void*) getApp ());
   			break;
   			case 2:
   			    err = getApp ()->_rose->start (audioCallback32Stereo, 2,
    				(void*) getApp ());
		    break;
 		}

		if (err) {
		    std::string s;
		    s = _stringTable->get ("ENGINEERROR").c_str ();
      		s += " (start): ";
        	s += getApp ()->_rose->decodeError (err);
			FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
				s.c_str (), 0, MBOX_OK);
			a.execute ();
			_ch3->setCheck (false);
			_playButton->setState (false);
			return 1;
      	}
		else {
			_ch3->setCheck (true);
			_playButton->setState (true);
        	return 1;
      	}
 	}
}

long MainWindow::onCmdRunRecord (FXObject*, FXSelector, void *) {
	if (getApp ()->_recording == false) {
	    try {
	    	getApp ()->_outFile = new WavOutFile (_config->literal ("Devices",
    	 		"OutputPCMFile").c_str (),
       			(int) getApp ()->_info->samplingRate (), 16, 2);
     	}
      	catch (std::runtime_error& e) {
      	    std::string s;
      	    s = _stringTable->get ("RECORDERROR");
      	    s += ": ";
      	    s += e.what ();
			FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
				s.c_str (), 0, MBOX_OK);
			a.execute ();
			_ch6->setCheck (false);
			_recordButton->setState (false);
			return 1;
		}

		_ch6->setCheck (true);
		_recordButton->setState (true);
		getApp ()->_recording = true;
		return 1;
	}
	else {
	    getApp ()->_recording = false;
	    delete getApp ()->_outFile;
		_ch6->setCheck (false);
   		_recordButton->setState (false);
   		return 1;
	}
}

long MainWindow::onCmdRunEnable (FXObject*, FXSelector, void *) {
    getApp ()->_remi->enableThru ((int) _config->number ("Devices",
    	"EnableMIDIThru"));
	if (getApp ()->_remi->inputDevices ().size () < 1) {
  	    _config->number ("Devices", "EnableMIDIInput", 0);
 	}
 	else {
   	    _config->number ("Devices", "EnableMIDIInput", 1);
  	}

	if (getApp ()->_remi->isRunning () == 1) {
    	int err = getApp ()->_remi->stop ();
    	if (err) {
		    std::string s;
		    s = _stringTable->get ("MIDIERROR").c_str ();
      		s += " (stop): ";
        	s += getApp ()->_remi->decodeError (err);
			FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
				s.c_str (), 0, MBOX_OK);
			a.execute ();
			_ch4->setCheck (true);
			_enableButton->setState (true);
			return 1;
      	}
		else {
			_ch4->setCheck (false);
			_enableButton->setState (false);
        	return 1;
      	}
	}
	else {
    	int inputId = getApp ()->_remi->translateInputNumber (
     		(int) _config->number ("Devices",
      			"MIDIInput"));
		if (inputId < 0) {
     		inputId = 0;
			_config->number ("Devices", "MIDIInput", 0);
  		}
    	int outputId = getApp ()->_remi->translateOutputNumber (
     		(int) _config->number ("Devices",
      			"MIDIOutput"));
		if (outputId < 0) {
     		outputId = 0;
			_config->number ("Devices", "MIDIOutput", 0);
  		}

	    getApp ()->_remi->inputDevice (inputId);
		getApp ()->_remi->outputDevice (outputId);

		int err = -1;
  		if ((int) _config->number ("Devices", "EnableMIDIInput") == 1) {
  		    err = getApp ()->_remi->start (MIDICallback32, (void*)
        		getApp ());
        }
        else {
        	err = getApp ()->_remi->start ();
    	}

		if (err) {
		    std::string s;
		    s = _stringTable->get ("MIDIERROR").c_str ();
      		s += " (start): ";
        	s += getApp ()->_remi->decodeError (err);
			FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
				s.c_str (), 0, MBOX_OK);
			a.execute ();
			_ch4->setCheck (false);
			_enableButton->setState (false);
			return 1;
      	}
		else {
            int a = (int)
            	(_config->number ("Devices", "FilterActiveSensing") * 1) |
            	(int) (_config->number ("Devices", "FilterSysEx") * 2) |
                (int) (_config->number ("Devices", "FilterClock") * 4);
            getApp ()->_remi->setFilters (a);

			_ch4->setCheck (true);
			_enableButton->setState (true);
        	return 1;
      	}
 	}
}

long MainWindow::onCmdToolsSaveMon (FXObject*, FXSelector, void *) {
    std::ofstream out ("MIDIMon.txt");
    if (!out.good ()) {
		FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
			_stringTable->get ("FILEERROR").c_str (), 0, MBOX_OK);
		a.execute ();
    }
    else {
    	for (int i = 0; i < msgList->getNumItems (); i++) {
        	out << msgList->getItem (i)->getText ().text () << std::endl;
     	}

     	out.close ();
		FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
			_stringTable->get ("LISTSAVED").c_str (), 0, MBOX_OK);
		a.execute ();
	}
	return 1;
}

long MainWindow::onCmdToolsClearMon (FXObject*, FXSelector, void *) {
	msgList->clearItems ();
	return 1;
}

#ifdef _WIN32
    long MainWindow::onCmdToolsSndVol (FXObject*, FXSelector, void *) {
    	SHELLEXECUTEINFO	seInfo;

    	ZeroMemory (&seInfo, sizeof (seInfo));
    	seInfo.cbSize = sizeof (seInfo);
    	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    	seInfo.lpVerb =  "open";
    	seInfo.lpFile = "sndvol32.exe";
    	seInfo.lpParameters = "";
    	seInfo.nShow = SW_SHOWNORMAL;
    	seInfo.hwnd = (HWND) this->id ();
    	ShellExecuteEx (&seInfo);
    	return 1;
    }
#endif

long MainWindow::onCmdToolsSettings (FXObject*, FXSelector, void *) {
	SettingsDialog settings (this);
	if (settings.execute (PLACEMENT_OWNER)) {
		// FIXME: SR CHANGE IN PROTON!!
   		getApp ()->_info->samplingRate (
    		standardSampleRates[(int) _config->number ("Devices",
  			"SamplingRate")]);
    	if (getApp ()->_rose->isRunning () == 1) {
     	    handle (this, FXSEL (SEL_COMMAND, ID_RUNPLAY), NULL); // twice...
       	    handle (this, FXSEL (SEL_COMMAND, ID_RUNPLAY), NULL);
  		}
    	if (getApp ()->_remi->isRunning () == 1) {
    	    handle (this, FXSEL (SEL_COMMAND, ID_RUNENABLE), NULL); // twice...
       	    handle (this, FXSEL (SEL_COMMAND, ID_RUNENABLE), NULL);
  		}

    	if (_config->number ("Harmonizer", "Waveform") == 3) {
    		getApp ()->_harmon->mute (true);
    		getApp ()->_morph->mute (true);
    		delete getApp ()->_table;
    		getApp ()->_table = new SignalTable (
      			(long) _config->number ("Harmonizer", "DelayLine"));
    		double r = _config->number ("Harmonizer",
    			"UsedDelay") / _config->number ("Harmonizer",
    			"DelayLine");
      		getApp ()->_table->delay (r);
            getApp ()->_harmon->table (getApp ()->_table);
			getApp ()->_morph->table (getApp ()->_table);
			getApp ()->_morph->speedFactor (1,
				(long) _config->number ("Morphizer", "PhaseJitter"));
			getApp ()->_morph->grainDirection (1);
			double fn = getApp ()->_table->naturalFreq (getApp ()->_info->samplingRate ());
			getApp ()->_morph->frequency (fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
			getApp ()->_morph->grainDuration ((double) _config->number ("Morphizer", "Center2"),
				_config->number ("Morphizer", "Random2"),
				_config->number ("Morphizer", "Attack") / 100,
				_config->number ("Morphizer", "Decay") / 100);
    		getApp ()->_live = true;
        	getApp()->_pdet->reset ();
    		getApp ()->_harmon->mute (false);
    		getApp ()->_morph->mute (false);
		}
	}
	return 1;
}

long MainWindow::onCmdHelpWeb (FXObject*, FXSelector, void *) {
#ifdef _WIN32
	SHELLEXECUTEINFO	seInfo;

	ZeroMemory (&seInfo, sizeof (seInfo));
	seInfo.cbSize = sizeof (seInfo);
	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	seInfo.lpVerb = "open";
	seInfo.lpFile = _stringTable->get ("WEBPAGE").c_str ();
	seInfo.lpParameters = "";
	seInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteEx (&seInfo);
#else
	FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
		_stringTable->get ("NOT_IMPLEMENTED").c_str (), 0, MBOX_OK);
	a.execute ();
#endif
	return 1;
}

long MainWindow::onCmdHelpMail (FXObject*, FXSelector, void *) {
#ifdef _WIN32
	SHELLEXECUTEINFO	seInfo;

	ZeroMemory (&seInfo, sizeof (seInfo));
	seInfo.cbSize = sizeof (seInfo);

	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	seInfo.lpVerb = "open";
	seInfo.lpFile = _stringTable->get ("MAILCOMMAND").c_str ();
	seInfo.lpParameters = "";
	seInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteEx (&seInfo);
#else
	FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
		_stringTable->get ("NOT_IMPLEMENTED").c_str (), 0, MBOX_OK);
	a.execute ();
#endif
	return 1;
}

long MainWindow::onCmdHelpContents (FXObject*, FXSelector, void *) {
	FXDialogBox help (this, _stringTable->get ("HELPDLG").c_str (),
 		DECOR_TITLE | DECOR_BORDER,
		0,0,0,0, 0,0,0,0, 0,0);

	// Bottom part
	FXHorizontalFrame* closeBox = new FXHorizontalFrame (&help,
  		LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH);

	new FXButton (closeBox,
 		_stringTable->get ("OKBUTTON").c_str (),
 		NULL, &help,
  		FXDialogBox::ID_ACCEPT, BUTTON_INITIAL | BUTTON_DEFAULT | LAYOUT_RIGHT
   		| FRAME_RAISED | FRAME_THICK,
    	0,0,0,0, 20,20,5,5);

    // Editor part
    FXHorizontalFrame* editBox = new FXHorizontalFrame (&help, LAYOUT_SIDE_TOP
    	| LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK,
     	0,0,0,0, 0,0,0,0);
    FXText* helpText = new FXText (editBox, NULL, 0, TEXT_READONLY |
    	TEXT_WORDWRAP |
    	LAYOUT_FILL_X | LAYOUT_FILL_Y);
    helpText->setVisibleRows (30);
    helpText->setVisibleColumns (80);

    helpText->setText (_stringTable->get ("HELP").c_str (),
    	strlen (_stringTable->get ("HELP").c_str ()));
    helpText->setTabColumns (35);
    helpText->setFocus ();

    help.execute (PLACEMENT_OWNER);
	return 1;
}

long MainWindow::onCmdHelpAbout (FXObject*, FXSelector, void *) {
	FXDialogBox about (this, _stringTable->get ("ABOUTDLG").c_str (),
		DECOR_BORDER,
		0,0,0,0, 0,0,0,0, 0,0);
	FXHorizontalFrame* cont = new FXHorizontalFrame (&about, LAYOUT_SIDE_RIGHT |
		LAYOUT_FILL,
		0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel (cont, FXString::null, getApp ()->_splashIcon,
 		FRAME_THICK | LAYOUT_LEFT,
		0,0,0,0, 0,0,0,0);
	FXVerticalFrame* info = new FXVerticalFrame (cont, LAYOUT_RIGHT |
		LAYOUT_FILL,
		0,0,0,0, 10,10,10,10, 0,0);
	FXLabel* title = new FXLabel (info, _stringTable->get ("EDITION").c_str (), 0,
		JUSTIFY_RIGHT | LAYOUT_FILL_X);
	FXFont* font = new FXFont (getApp (), "Verdana", 16, FXFont::ExtraBold);
	title->setFont (font);
	new FXHorizontalSeparator (info, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	new FXLabel (info, _stringTable->get ("AUTHOR").c_str (), 0,
		JUSTIFY_RIGHT | LAYOUT_FILL);
	FXHorizontalFrame* cont2 = new FXHorizontalFrame (info, LAYOUT_FILL,
		0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel (cont2, FXString::null, getApp ()->_nerdIcon, LAYOUT_LEFT |
 		LAYOUT_BOTTOM,
		0,0,0,0, 0,0,0,0);
	FXButton* button = new FXButton (cont2, "",
 		getApp ()->_penguinIcon, &about, FXDialogBox::ID_ACCEPT,
   		BUTTON_INITIAL | BUTTON_DEFAULT |
	 	FRAME_RAISED | FRAME_THICK | LAYOUT_RIGHT | LAYOUT_BOTTOM,
   		0,0,0,0, 32,32,5,5);
	new FXLabel (info, _stringTable->get ("TECHINFO").c_str (),
		0, JUSTIFY_RIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_BOTTOM);
	button->setFocus ();
	about.execute (PLACEMENT_OWNER);

	delete font; // must be deleted here to avoid memory leaks
	return 1;
}

long MainWindow::onCmdSplashScreen (FXObject*, FXSelector, void *) {
	SplashScreen splash (this);
  	splash.execute (PLACEMENT_OWNER);
	return 1;
}

long MainWindow::onTimer (FXObject*, FXSelector, void *) {
	if (_ready == true) {
	    _ready = false;
        FXString temp;
        _CPULoad = (int) ((double) 100 * getApp ()->_rose->loadedCPU ());
        _leftLevel = (int) ((double) 100 * getApp ()->_left->get ());
    	_rightLevel = (int) ((double) 100 * getApp ()->_right->get ());

    	if (getApp ()->_rose->isRunning () == 1) {
    	    temp.format ("%s%d, %d",
         		_stringTable->get ("AUDIO").c_str (),
         		getApp ()->_rose->recordDevice (),
         		getApp ()->_rose->playbackDevice ());
        	_audioDevLabel->setText (temp);

        	long delSize = (long) ((double) getApp ()->_table->size ()
        		* getApp ()->_table->delay ());
        	double delTime = 0;

        	if (getApp ()->_live == true) {
//				double percFreq = _config->number ("Morphizer", "Center3") *
//					_config->number ("Morphizer", "Random3");
//				double maxFreq = _config->number ("Morphizer", "Center3") + percFreq;
//				double percGliss = _config->number ("Morphizer", "Center4") *
//					_config->number ("Morphizer", "Random4");
//				double maxGliss = _config->number ("Morphizer", "Center4") + percGliss;
//				double realMax = maxFreq > maxGliss ? maxFreq : maxGliss;
//
////				double percLen = _config->number ("Morphizer",
////        				"Center2") * _config->number ("Morphizer",
////        				"Random2");
////        		double maxLen = (percLen + _config->number ("Morphizer",
////        				"Center2")) * getApp ()->_info->samplingRate ();
//        		double use = fabs ((realMax *
//        			delSize) - delSize);
//        		use += ((double) _config->number ("Morphizer",
//        			"PhaseJitter") / 2);
//        		double perc = use / delSize;
//        		perc *= 100;
//	      	    temp.format ("%s%g%%",
//	         		_stringTable->get ("MORPHUSAGE").c_str (),
//	         		perc);
//	         	if (perc >= 100)
//	         		_morphUsageLabel->setTextColor (FXRGB (255, 0, 0));
//	         	else
//	         		_morphUsageLabel->setTextColor (FXRGB (0, 0, 127));
//	        	_morphUsageLabel->setText (temp);
//
//	        	realMax = 0;
//	        	for (int i = 0; i < MAX_VOICES; i++) {
//	        		std::stringstream s;
//	        		s << "FORM" << i + 1;
//	        		if (_config->number ("Harmonizer",
//	        			s.str ().c_str ()) >= realMax)
//	        			realMax = _config->number ("Harmonizer",
//	        				s.str ().c_str ());
//	        	}
//
//        		use = fabs ((realMax *
//        			(double) delSize) - delSize);
//        		perc = use / delSize;
//        		perc *= 100;
//	      	    temp.format ("%s%g%%",
//	         		_stringTable->get ("HARMONUSAGE").c_str (),
//	         		perc);
//	         	if (perc >= 100)
//	         		_harmonUsageLabel->setTextColor (FXRGB (255, 0, 0));
//	         	else
//	         		_harmonUsageLabel->setTextColor (FXRGB (0, 0, 127));
//	        	_harmonUsageLabel->setText (temp);
				delTime = (double) delSize / getApp ()->_info->samplingRate () * 1000;
        	}
//        	else {
//        		_harmonUsageLabel->setTextColor (FXRGB (0, 0, 127));
//        		_harmonUsageLabel->setText ("");
//        		_morphUsageLabel->setTextColor (FXRGB (0, 0, 127));
//        		_morphUsageLabel->setText ("");
//        	}
    	    temp.format ("%s%d",
         		_stringTable->get ("LATENCY").c_str (),
         		getApp ()->_rose->latency () + (int) delTime);
        	_engineLatencyLabel->setText (temp);

    	    temp.format ("%d-%d-%d-%d : %d",
         		getApp ()->_harmon->overlap (0),
         		getApp ()->_harmon->overlap (1),
         		getApp ()->_harmon->overlap (2),
         		getApp ()->_harmon->overlap (3),
         		getApp ()->_morph->overlap ());
        	_olapLabel->setText (temp);

    	    temp.format ("%g",
         		getApp ()->_harmon->pitch ());
        	_frequencyLabel->setText (temp);

    	    temp.format ("%s%d",
         		noteNames[getApp ()->_harmon->lastNote () % 12],
         		(getApp ()->_harmon->lastNote () / 12 - 1));
        	_noteLabel->setText (temp);

	    	if (getApp ()->_harmon->logic () == Pandora::SCALIC ||
	    		getApp ()->_harmon->logic () == Pandora::INTELLICHORD ||
	    		getApp ()->_harmon->logic () == Pandora::SINGLECHORD
	    		) {
	    		temp.format ("%d", getApp ()->_harmon->lastTone () + 1);
				_toneLabel->setText (temp);
	    	}
	    	else {
				_toneLabel->setText ("----");
	    	}
    	}
    	else {
    	    temp.format ("%s--, --",
         		_stringTable->get ("AUDIO").c_str ());
         	_audioDevLabel->setText (temp);

    	    temp.format ("%s--",
         		_stringTable->get ("LATENCY").c_str ());
        	_engineLatencyLabel->setText (temp);

         	_olapLabel->setText ("----");
     		_frequencyLabel->setText ("----");
     		_noteLabel->setText ("----");
     		_toneLabel->setText ("----");
    	}

    	if (getApp ()->_remi->isRunning () == 1) {
    	    temp.format ("%s%d, %d",
         		_stringTable->get ("MIDI").c_str (),
         		getApp ()->_remi->inputDevice (),
         		getApp ()->_remi->outputDevice ());
        	_MIDIDevLabel->setText (temp);
    	}
     	else {
    	    temp.format ("%s--, --",
         		_stringTable->get ("MIDI").c_str ());
         	_MIDIDevLabel->setText (temp);
    	}
       	if (getApp ()->_recording == true) {
        	_recordLabel->setText (_stringTable->get ("RECORDING").c_str ());
    	}
    	else {
    		_recordLabel->setText ("");
    	}

    	getApp ()->addTimeout ((FXObject*) this, ID_TIMER,
    		TIMEOUT);
    	_ready = true;
	}
	update ();
	return 1;
}


// reVoice specific...
long MainWindow::onCmdHelpProton (FXObject*, FXSelector, void *) {
#ifdef _WIN32
	char szCurrentLocation[500];
	GetModuleFileName (NULL, szCurrentLocation, sizeof (szCurrentLocation));
	std::string s = (std::string) szCurrentLocation;
	unsigned int right = s.find_last_of ('\\');
	s = s.substr (0, right + 1);
	s += "Proton.chm";

	SHELLEXECUTEINFO	seInfo;

	ZeroMemory (&seInfo, sizeof (seInfo));
	seInfo.cbSize = sizeof (seInfo);
	seInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	seInfo.lpVerb = "open";
	seInfo.lpFile = "Proton.chm";
	seInfo.lpParameters = "";
	seInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteEx (&seInfo);
#else
	FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
		_stringTable->get ("NOT_IMPLEMENTED").c_str (), 0, MBOX_OK);
	a.execute ();
#endif
	return 1;
}

long MainWindow::onCmdMultiplier (FXObject* sender, FXSelector sel, void*) {
//    FXRealSlider* s = (FXRealSlider*) sender;
//	getApp ()->_outputMultiplier = (double) s->getValue ();
//	_config->number ("Harmonizer", "OutputMultiplier",
//		getApp ()->_outputMultiplier);
//	std::stringstream st;
//	st << (int) getApp ()->_outputMultiplier << "x";
//	_multiValue->setText (st.str ().c_str ());
//	
    FXKnob* s = (FXKnob*) sender;
	getApp ()->_outputMultiplier = (double) s->getValue () / 100;
	_config->number ("Harmonizer", "OutputMultiplier",
		getApp ()->_outputMultiplier);
	std::stringstream st;
	st << getApp ()->_outputMultiplier * 100;
	_multiValue->setText (st.str ().c_str ());
	
	return 1;
}

//long MainWindow::onCmdVoiceOptions (FXObject*, FXSelector sel, void*) {
//    std::stringstream s;
//
////    switch (FXSELID (sel)) {
////    	case ID_VOICEOPTION1:
////    	    getApp ()->_harmon->activeVoices (1);
////			_config->number ("Harmonizer", "ActiveVoices", 1);
////        break;
////    	case ID_VOICEOPTION2:
////         	getApp ()->_harmon->activeVoices (2);
////			_config->number ("Harmonizer", "ActiveVoices", 2);
////        break;
////    	case ID_VOICEOPTION3:
////       	    getApp ()->_harmon->activeVoices (3);
////			_config->number ("Harmonizer", "ActiveVoices", 3);
////        break;
////    	case ID_VOICEOPTION4:
////       	    getApp ()->_harmon->activeVoices (4);
////			_config->number ("Harmonizer", "ActiveVoices", 4);
////        break;
////	}
////
////    for (int i = 0; i < PARTIALS; i++) {
////    	_partialsAmpValues[i]->disable ();
////		_partialsAmpSliders[i]->disable ();
////		_partialsFreqValues[i]->disable ();
////		_partialsFreqSliders[i]->disable ();
//// 	}
////	if ((int) _config->number ("Harmonizer", "Logic") == 0) {
////	    for (int i = 0; i < (int) _config->number ("Harmonizer",
////     		"ActiveVoices"); i++) {
////        	_partialsAmpValues[i]->enable ();
////    		_partialsAmpSliders[i]->enable ();
////			_partialsFreqValues[i]->enable ();
////    		_partialsFreqSliders[i]->enable ();
////     	}
////   	}
//}

long MainWindow::onCmdPanic (FXObject* sender, FXSelector sel, void*) {
	getApp ()->_table->clear ();
//	for (int i = 0; i < PARTIALS; i++) {
//		getApp()->_pdets[i]->reset ();
// 	}
	return 1;
}

long MainWindow::onCmdPhiReset (FXObject* sender, FXSelector sel, void*) {
	getApp ()->_harmon->reset ();
	_config->number ("Morphizer", "Speed", 1);
   	_speedFactorField->setText ("1");
	_speedFactorSlider->setValue (1);
	getApp ()->_morph->speedFactor (
		_config->number ("Morphizer", "Speed"),
		(long) _config->number ("Morphizer", "PhaseJitter"));
	_config->number ("Morphizer", "Grainlet", 0);
	getApp ()->_morph->generation (Pandora::ASYNC);
	getApp ()->_morph->reset ();
	getApp ()->_reverbLeft->clear ();
	getApp ()->_reverbRight->clear ();
	getApp ()->_pdet->reset ();
	_morphDetParamValues[1]->enable ();
	_morphDetParamSliders[1]->enable ();
	_morphRandParamValues[1]->enable ();
	_morphRandParamSliders[1]->enable ();
	_grainletCheck->setCheck (false);
	return 1;
}

long MainWindow::onCmdWaveform (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "Waveform", (double)
 		c->getCurrentItem ());

	getApp ()->_harmon->mute (true);
	getApp ()->_morph->mute (true);

	delete getApp ()->_table;
   	if (c->getCurrentItem () == 0) { // Sinusoid...
	    _fileButton->disable ();
	    _fileField->disable ();
		_speedFactorField->enable ();
		_speedFactorSlider->enable ();
		_revGrainCheck->enable ();

		getApp ()->_table = new SignalTable ((long)
   				standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
		SignalTable t0 ((long) standardSampleRates[(int)
   				_config->number ("Devices", "SamplingRate")]);
		t0.gen (1, 1);
		LoopOscillator osc (getApp ()->_info, &t0);
		osc.frequency (440);
		for (int i = 0; i < getApp ()->_table->size (); i++) {
        	getApp ()->_table->set (i, osc.tick ());
      	}
		getApp ()->_live = false;
   	}
   	else if (c->getCurrentItem () == 1) { // Square...
	    _fileButton->disable ();
	    _fileField->disable ();
		_speedFactorField->enable ();
		_speedFactorSlider->enable ();
		_revGrainCheck->enable ();

		getApp ()->_table = new SignalTable ((long)
   				standardSampleRates[(int) _config->number ("Devices", "SamplingRate")]);
		SignalTable t0 ((long) standardSampleRates[(int)
   				_config->number ("Devices", "SamplingRate")]);
		t0.square (20);
		LoopOscillator osc (getApp ()->_info, &t0);
		osc.frequency (440);
		for (int i = 0; i < getApp ()->_table->size (); i++) {
        	getApp ()->_table->set (i, osc.tick ());
      	}
		getApp ()->_live = false;
   	}
	else if (c->getCurrentItem () == 2) { // Sampled sound...
	    _fileButton->enable ();
	    _fileField->enable ();
		_speedFactorField->enable ();
		_speedFactorSlider->enable ();
		_revGrainCheck->enable ();

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
       		getApp ()->_table = new SignalTable (p);
            long n = 0;

            while (!inFile.eof()) {
                short s;
                inFile.read (&s, sizeof (short));
                double d = (double) s;
                d /= 32768;
                getApp ()->_table->set (n, (double) d);
                n++;
            }
       	}
       	catch (std::runtime_error& e) {
       	    getApp ()->_table = new SignalTable (TABLEN);
			getApp ()->_table->clear ();
       	    std::stringstream err;
       	    err << _stringTable->get ("FILEERROR").c_str ()
       	    	<< ":\n" << e.what ();
       	    FXMessageBox a (this,
       	    	_stringTable->get ("TITLE").c_str (),
   	    		err.str ().c_str (),
   	    		0,
   	    		MBOX_OK);
    		a.execute ();
        }
        getApp ()->_live = false;
	}
	else { // Live...
	    _fileButton->disable ();
	    _fileField->disable ();
		_speedFactorField->disable ();
		_speedFactorSlider->disable ();
		_revGrainCheck->disable ();

		getApp ()->_table = new SignalTable (
  			(long) _config->number ("Harmonizer", "DelayLine"));
		double r = _config->number ("Harmonizer",
			"UsedDelay") / _config->number ("Harmonizer",
			"DelayLine");
  		getApp ()->_table->delay (r);
		getApp ()->_live = true;
	}

	getApp ()->_harmon->table (getApp ()->_table);
	getApp ()->_morph->table (getApp ()->_table);
	if (c->getCurrentItem () == 3) {
		getApp ()->_morph->speedFactor (1,
			(long) _config->number ("Morphizer", "PhaseJitter"));
		getApp ()->_morph->grainDirection (1);
	}
	else {
		getApp ()->_morph->speedFactor (
			_config->number ("Morphizer", "Speed"),
			(long) _config->number ("Morphizer", "PhaseJitter"));
	   	if (_config->number ("Morphizer", "ReverseGrain") == 0) {
			getApp ()->_morph->grainDirection (1);
	   	}
	   	else {
		   	getApp ()->_morph->grainDirection (-1);
	   	}
	}
	double fn = getApp ()->_table->naturalFreq (getApp ()->_info->samplingRate ());
	getApp ()->_morph->frequency (fn * _config->number ("Morphizer", "Center3"),
		_config->number ("Morphizer", "Random3"),
		fn * _config->number ("Morphizer", "Center4"),
		_config->number ("Morphizer", "Random4"));
	getApp ()->_morph->grainDuration ((double) _config->number ("Morphizer", "Center2"),
		_config->number ("Morphizer", "Random2"),
		_config->number ("Morphizer", "Attack") / 100,
		_config->number ("Morphizer", "Decay") / 100);
	getApp ()->_reverbLeft->clear ();
	getApp ()->_reverbRight->clear ();
	getApp()->_pdet->reset ();

	getApp ()->_harmon->mute (false);
	getApp ()->_morph->mute (false);
	return 1;
}

long MainWindow::onCmdFileBrowse (FXObject* sender, FXSelector, void*) {
    FXFileDialog openDialog (this,_stringTable->get ("OPENFILE").c_str (),
    	DECOR_NONE);
    openDialog.setSelectMode (SELECTFILE_EXISTING);
    openDialog.setDirectory (_config->literal ("Global",
    	"LastDirectory").c_str ());
    openDialog.setPatternList ("Wave PCM (*.wav)");
    if (openDialog.execute ()){
        _config->literal ("Global", "LastDirectory",
        	openDialog.getDirectory ().text ());
        _fileField->setText (openDialog.getFilename ());
        _config->literal ("Harmonizer", "InputFile",
        	openDialog.getFilename ().text ());
	    getApp ()->_harmon->mute (true);
		getApp ()->_morph->mute (true);
//	    getApp ()->_osc->mute (true);

   	    delete getApp ()->_table;
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
       		getApp ()->_table = new SignalTable (p);
            long n = 0;

            while (!inFile.eof()) {
                short s;
                inFile.read (&s, sizeof (short));
                double d = (double) s;
                d /= 32768;
                getApp ()->_table->set (n, (double) d);
                n++;
            }
       	}
       	catch (std::runtime_error& e) {
       	    getApp ()->_table = new SignalTable (TABLEN);
			getApp ()->_table->clear ();
       	    std::stringstream err;
       	    err << _stringTable->get ("FILEERROR").c_str ()
       	    	<< ":\n" << e.what ();

            FXMessageBox a (this,
       	    	_stringTable->get ("TITLE").c_str (),
   	    		err.str ().c_str (),
   	    		0,
   	    		MBOX_OK);
    		a.execute ();
        }

        getApp ()->_harmon->table (getApp ()->_table);
        getApp ()->_morph->table (getApp ()->_table);
		double fn = getApp ()->_table->naturalFreq (getApp ()->_info->samplingRate ());
		getApp ()->_morph->frequency (fn * _config->number ("Morphizer", "Center3"),
			_config->number ("Morphizer", "Random3"),
			fn * _config->number ("Morphizer", "Center4"),
			_config->number ("Morphizer", "Random4"));
		getApp ()->_morph->speedFactor (_config->number ("Morphizer", "Speed"),
			(long) _config->number ("Morphizer", "PhaseJitter"));
		getApp ()->_morph->grainDuration ((double) _config->number ("Morphizer", "Center2"),
			_config->number ("Morphizer", "Random2"),
			_config->number ("Morphizer", "Attack") / 100,
			_config->number ("Morphizer", "Decay") / 100);
//        getApp ()->_osc->table (getApp ()->_table);
//    	getApp ()->_osc->frequency (fn);
        getApp ()->_harmon->mute (false);
        getApp ()->_morph->mute (false);
//        getApp ()->_osc->mute (false);
    }
	return 1;
}

long MainWindow::onCmdLogic (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "Logic", (double)
 		c->getCurrentItem ());

    switch (c->getCurrentItem ()) {
    	case 0:
    		getApp ()->_useMIDIStream = false;
    	    getApp ()->_harmon->logic (Pandora::CUSTOM_CHORD);
    	    for (int i = 0; i < PARTIALS; i++) {
    	        FXString temp;
    	        temp.format ("On%d", i + 1);
    	        if (_config->number ("Harmonizer", temp.text ()) == true) {
                	_partialsAmpValues[i]->enable ();
            		_partialsAmpSliders[i]->enable ();
    				_partialsFreqValues[i]->enable ();
            		_partialsFreqSliders[i]->enable ();
      			}
         	}
      		_defaultButton->enable ();
			_randomButton->enable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 1:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::DOMINANT_SEVENTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 2:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::MINOR_SEVENTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 3:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::MAJOR_SEVENTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 4:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::DIMINISHED_SEVENTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 5:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::AUGMENTED_SEVENTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 6:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::QUARTAL_CHORD);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 7:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::SCALIC);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->enable ();
			_scaleTypeCombo->enable ();
        break;
    	case 8:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::INTELLICHORD);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->enable ();
			_scaleTypeCombo->enable ();
        break;
    	case 9:
		   	for (int i = 0; i < PARTIALS; i++) {
	   		    FXString temp1, temp2;
		 		temp1.format ("A%d", i + 1);
		        temp2.format ("F%d", i + 1);
				getApp ()->_harmon->ratios (i,
		    		_config->number ("Harmonizer", temp1.text ()),
		     		_config->number ("Harmonizer", temp2.text ()));
		   	}
         	getApp ()->_harmon->logic (Pandora::CUSTOM_CHORD);
    		getApp ()->_useMIDIStream = true;
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 10:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::MINOR_WITH_SIXTH);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->disable ();
			_scaleTypeCombo->disable ();
        break;
    	case 11:
    		getApp ()->_useMIDIStream = false;
         	getApp ()->_harmon->logic (Pandora::SINGLECHORD);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
         	}
      		_defaultButton->disable ();
			_randomButton->disable ();
			_masterKeyCombo->enable ();
			_scaleTypeCombo->enable ();
        break;
	}
	return 1;
}

long MainWindow::onCmdDefault (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double amp = .25;
   		double freq = 1;
		FXString temp1, temp2;

    	_partialsAmpSliders[i]->setValue (amp);
    	temp1.format ("%g", amp);
    	_partialsAmpValues[i]->setText (temp1);
    	temp1.format ("A%d", i + 1);
     	_config->number ("Harmonizer", temp1.text (), amp);
		_partialsFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_partialsFreqValues[i]->setText (temp2);
    	temp2.format ("F%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->ratios (i,
	    	_config->number ("Harmonizer", temp1.text ()),
	    	_config->number ("Harmonizer", temp2.text ()));
 	}
 	return 1;
}

long MainWindow::onCmdRandom (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double amp = getApp ()->_urand->tick ();
   		double freq = getApp ()->_rrand->tick ();
		FXString temp1, temp2;

    	_partialsAmpSliders[i]->setValue (amp);
    	temp1.format ("%g", amp);
    	_partialsAmpValues[i]->setText (temp1);
    	temp1.format ("A%d", i + 1);
     	_config->number ("Harmonizer", temp1.text (), amp);
		_partialsFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_partialsFreqValues[i]->setText (temp2);
    	temp2.format ("F%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->ratios (i,
	    	_config->number ("Harmonizer", temp1.text ()),
	    	_config->number ("Harmonizer", temp2.text ()));
	}
	return 1;
}

long MainWindow::onCmdSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

    switch (FXSELID (sel)) {
    	case ID_A1:
			_config->number ("Harmonizer", "A1", c);
    	    getApp ()->_harmon->ratios (0,
    	    	_config->number ("Harmonizer", "A1"),
	    		_config->number ("Harmonizer", "F1"));
       		_partialsAmpValues[0]->setText (v.str ().c_str ());
	    break;
	    case ID_F1:
			_config->number ("Harmonizer", "F1", c);
    	    getApp ()->_harmon->ratios (0,
    	    	_config->number ("Harmonizer", "A1"),
	    		_config->number ("Harmonizer", "F1"));
       		_partialsFreqValues[0]->setText (v.str ().c_str ());
        break;
    	case ID_A2:
			_config->number ("Harmonizer", "A2", c);
    	    getApp ()->_harmon->ratios (1,
    	    	_config->number ("Harmonizer", "A2"),
	    		_config->number ("Harmonizer", "F2"));
       		_partialsAmpValues[1]->setText (v.str ().c_str ());
	    break;
	    case ID_F2:
			_config->number ("Harmonizer", "F2", c);
    	    getApp ()->_harmon->ratios (1,
    	    	_config->number ("Harmonizer", "A2"),
	    		_config->number ("Harmonizer", "F2"));
       		_partialsFreqValues[1]->setText (v.str ().c_str ());
        break;
    	case ID_A3:
			_config->number ("Harmonizer", "A3", c);
    	    getApp ()->_harmon->ratios (2,
    	    	_config->number ("Harmonizer", "A3"),
	    		_config->number ("Harmonizer", "F3"));
       		_partialsAmpValues[2]->setText (v.str ().c_str ());
	    break;
	    case ID_F3:
			_config->number ("Harmonizer", "F3", c);
    	    getApp ()->_harmon->ratios (2,
    	    	_config->number ("Harmonizer", "A3"),
	    		_config->number ("Harmonizer", "F3"));
       		_partialsFreqValues[2]->setText (v.str ().c_str ());
        break;
    	case ID_A4:
			_config->number ("Harmonizer", "A4", c);
    	    getApp ()->_harmon->ratios (3,
    	    	_config->number ("Harmonizer", "A4"),
	    		_config->number ("Harmonizer", "F4"));
			_partialsAmpValues[3]->setText (v.str ().c_str ());
	    break;
	    case ID_F4:
			_config->number ("Harmonizer", "F4", c);
    	    getApp ()->_harmon->ratios (3,
    	    	_config->number ("Harmonizer", "A4"),
	    		_config->number ("Harmonizer", "F4"));
       		_partialsFreqValues[3]->setText (v.str ().c_str ());
        break;
	}
	return 1;
}

long MainWindow::onCmdSliderText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s= (FXTextField*) sender;
   	std::stringstream v;
   	v << s->getText ().text ();
   	double c = atof (v.str ().c_str ());

    switch (FXSELID (sel)) {
    	case ID_A1TEXT:
			_config->number ("Harmonizer", "A1", c);
    	    getApp ()->_harmon->ratios (0,
    	    	_config->number ("Harmonizer", "A1"),
	    		_config->number ("Harmonizer", "F1"));
       		_partialsAmpSliders[0]->setValue (c);
	    break;
	    case ID_F1TEXT:
			_config->number ("Harmonizer", "F1", c);
    	    getApp ()->_harmon->ratios (0,
    	    	_config->number ("Harmonizer", "A1"),
	    		_config->number ("Harmonizer", "F1"));
       		_partialsFreqSliders[0]->setValue (c);
        break;
    	case ID_A2TEXT:
			_config->number ("Harmonizer", "A2", c);
    	    getApp ()->_harmon->ratios (1,
    	    	_config->number ("Harmonizer", "A2"),
	    		_config->number ("Harmonizer", "F2"));
       		_partialsAmpSliders[1]->setValue (c);
	    break;
	    case ID_F2TEXT:
			_config->number ("Harmonizer", "F2", c);
    	    getApp ()->_harmon->ratios (1,
    	    	_config->number ("Harmonizer", "A2"),
	    		_config->number ("Harmonizer", "F2"));
       		_partialsFreqSliders[1]->setValue (c);
        break;
    	case ID_A3TEXT:
			_config->number ("Harmonizer", "A3", c);
    	    getApp ()->_harmon->ratios (2,
    	    	_config->number ("Harmonizer", "A3"),
	    		_config->number ("Harmonizer", "F3"));
       		_partialsAmpSliders[2]->setValue (c);
	    break;
	    case ID_F3TEXT:
			_config->number ("Harmonizer", "F3", c);
    	    getApp ()->_harmon->ratios (2,
    	    	_config->number ("Harmonizer", "A3"),
	    		_config->number ("Harmonizer", "F3"));
       		_partialsFreqSliders[2]->setValue (c);
        break;
    	case ID_A4TEXT:
			_config->number ("Harmonizer", "A4", c);
    	    getApp ()->_harmon->ratios (3,
    	    	_config->number ("Harmonizer", "A4"),
	    		_config->number ("Harmonizer", "F4"));
       		_partialsAmpSliders[3]->setValue (c);
	    break;
	    case ID_F4TEXT:
			_config->number ("Harmonizer", "F4", c);
    	    getApp ()->_harmon->ratios (3,
    	    	_config->number ("Harmonizer", "A4"),
	    		_config->number ("Harmonizer", "F4"));
       		_partialsFreqSliders[3]->setValue (c);
        break;
	}
	return 1;
}

long MainWindow::onCmdVibratoLogic (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "VibratoLogic", (double)
 		c->getCurrentItem ());

    switch (c->getCurrentItem ()) {
    	case 0:
    	    getApp ()->_harmon->style (Pandora::CUSTOM_VIBRATO);
    	    for (int i = 0; i < PARTIALS; i++) {
    	        FXString temp;
    	        temp.format ("On%d", i + 1);
    	        if (_config->number ("Harmonizer", temp.text ()) == true) {
                	_vibratoAmpValues[i]->enable ();
            		_vibratoAmpSliders[i]->enable ();
    				_vibratoFreqValues[i]->enable ();
            		_vibratoFreqSliders[i]->enable ();
      			}
         	}
         	_defaultButton2->enable ();
  			_randomButton2->enable ();
        break;
    	case 1:
         	getApp ()->_harmon->style (Pandora::SMALL_VIBRATO);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_vibratoAmpValues[i]->disable ();
        		_vibratoAmpSliders[i]->disable ();
				_vibratoFreqValues[i]->disable ();
        		_vibratoFreqSliders[i]->disable ();
         	}
       		_defaultButton2->disable ();
			_randomButton2->disable ();
        break;
    	case 2:
         	getApp ()->_harmon->style (Pandora::MEDIUM_VIBRATO);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_vibratoAmpValues[i]->disable ();
        		_vibratoAmpSliders[i]->disable ();
				_vibratoFreqValues[i]->disable ();
        		_vibratoFreqSliders[i]->disable ();
         	}
       		_defaultButton2->disable ();
			_randomButton2->disable ();
        break;
    	case 3:
         	getApp ()->_harmon->style (Pandora::LARGE_VIBRATO);
    	    for (int i = 0; i < PARTIALS; i++) {
            	_vibratoAmpValues[i]->disable ();
        		_vibratoAmpSliders[i]->disable ();
				_vibratoFreqValues[i]->disable ();
        		_vibratoFreqSliders[i]->disable ();
           	}
       		_defaultButton2->disable ();
			_randomButton2->disable ();
        break;
    }
    return 1;
}

long MainWindow::onCmdRandVibratoLogic (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "RandomVibratoLogic", (double)
 		c->getCurrentItem ());

    switch (c->getCurrentItem ()) {
    	case 0:
    	    getApp ()->_harmon->randomStyle (Pandora::CUSTOM_RANDOM);
    	    for (int i = 0; i < PARTIALS; i++) {
    	        FXString temp;
    	        temp.format ("On%d", i + 1);
    	        if (_config->number ("Harmonizer", temp.text ()) == true) {
//                	_randVibratoAmpValues[i]->enable ();
//            		_randVibratoAmpSliders[i]->enable ();
    				_randVibratoFreqValues[i]->enable ();
            		_randVibratoFreqSliders[i]->enable ();
      			}
         	}
         	_defaultButton2a->enable ();
  			_randomButton2a->enable ();
        break;
    	case 1:
         	getApp ()->_harmon->randomStyle (Pandora::SMALL_RANDOM);
    	    for (int i = 0; i < PARTIALS; i++) {
//            	_randVibratoAmpValues[i]->disable ();
//        		_randVibratoAmpSliders[i]->disable ();
				_randVibratoFreqValues[i]->disable ();
        		_randVibratoFreqSliders[i]->disable ();
         	}
       		_defaultButton2a->disable ();
			_randomButton2a->disable ();
        break;
    	case 2:
         	getApp ()->_harmon->randomStyle (Pandora::MEDIUM_RANDOM);
    	    for (int i = 0; i < PARTIALS; i++) {
//            	_randVibratoAmpValues[i]->disable ();
//        		_randVibratoAmpSliders[i]->disable ();
				_randVibratoFreqValues[i]->disable ();
        		_randVibratoFreqSliders[i]->disable ();
         	}
       		_defaultButton2a->disable ();
			_randomButton2a->disable ();
        break;
    	case 3:
         	getApp ()->_harmon->randomStyle (Pandora::LARGE_RANDOM);
    	    for (int i = 0; i < PARTIALS; i++) {
//            	_randVibratoAmpValues[i]->disable ();
//        		_randVibratoAmpSliders[i]->disable ();
				_randVibratoFreqValues[i]->disable ();
        		_randVibratoFreqSliders[i]->disable ();
           	}
       		_defaultButton2a->disable ();
			_randomButton2a->disable ();
        break;
    }
    return 1;
}

long MainWindow::onCmdDefault2 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double amp = 0;
   		double freq = 4 + ((double) i * 0.1);
		FXString temp1, temp2;

    	_vibratoAmpSliders[i]->setValue (amp);
    	temp1.format ("%g", amp);
    	_vibratoAmpValues[i]->setText (temp1);
    	temp1.format ("VA%d", i + 1);
     	_config->number ("Harmonizer", temp1.text (), amp);
		_vibratoFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_vibratoFreqValues[i]->setText (temp2);
    	temp2.format ("VF%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->vibrato (i,
	    	_config->number ("Harmonizer", temp1.text ()),
	    	_config->number ("Harmonizer", temp2.text ()));
	}
	return 1;
}

long MainWindow::onCmdRandom2 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double amp = getApp ()->_urand2->tick ();
   		double freq = getApp ()->_rrand2->tick ();
		FXString temp1, temp2;

    	_vibratoAmpSliders[i]->setValue (amp);
    	temp1.format ("%g", amp);
    	_vibratoAmpValues[i]->setText (temp1);
    	temp1.format ("VA%d", i + 1);
     	_config->number ("Harmonizer", temp1.text (), amp);
		_vibratoFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_vibratoFreqValues[i]->setText (temp2);
    	temp2.format ("VF%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->vibrato (i,
	    	_config->number ("Harmonizer", temp1.text ()),
	    	_config->number ("Harmonizer", temp2.text ()));
	}
	return 1;
}

long MainWindow::onCmdDefault2a (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
//   		double amp = 0;
   		double freq = 0;
		FXString temp1, temp2;

//    	_randVibratoAmpSliders[i]->setValue (amp);
//    	temp1.format ("%g", amp);
//    	_randVibratoAmpValues[i]->setText (temp1);
    	temp1.format ("VARAND%d", i + 1);
//     	_config->number ("Harmonizer", temp1.text (), amp);
		_randVibratoFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_randVibratoFreqValues[i]->setText (temp2);
    	temp2.format ("VFRAND%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->randomVibrato (i,
	    	/* _config->number ("Harmonizer", temp1.text ()), */
	    	_config->number ("Harmonizer", temp2.text ()));
	}
	return 1;
}

long MainWindow::onCmdRandom2a (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
//   		double amp = getApp ()->_urand3->tick ();
   		double freq = getApp ()->_urand3->tick ();
		FXString temp1, temp2;

//    	_randVibratoAmpSliders[i]->setValue (amp);
//    	temp1.format ("%g", amp);
//    	_randVibratoAmpValues[i]->setText (temp1);
	   	temp1.format ("VARAND%d", i + 1);
//     	_config->number ("Harmonizer", temp1.text (), amp);
		_randVibratoFreqSliders[i]->setValue (freq);
		temp2.format ("%g", freq);
    	_randVibratoFreqValues[i]->setText (temp2);
    	temp2.format ("VFRAND%d", i + 1);
     	_config->number ("Harmonizer", temp2.text (), freq);
        getApp ()->_harmon->randomVibrato (i,
	    	/* _config->number ("Harmonizer", temp1.text ()), */
	    	_config->number ("Harmonizer", temp2.text ()));
	}
	return 1;
}

long MainWindow::onCmdVibratoSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

    switch (FXSELID (sel)) {
    	case ID_VA1:
			_config->number ("Harmonizer", "VA1", c);
    	    getApp ()->_harmon->vibrato (0,
    	    	_config->number ("Harmonizer", "VA1"),
	    		_config->number ("Harmonizer", "VF1"));
       		_vibratoAmpValues[0]->setText (v.str ().c_str ());
	    break;
	    case ID_VF1:
			_config->number ("Harmonizer", "VF1", c);
    	    getApp ()->_harmon->vibrato (0,
    	    	_config->number ("Harmonizer", "VA1"),
	    		_config->number ("Harmonizer", "VF1"));
 			_vibratoFreqValues[0]->setText (v.str ().c_str ());
        break;
    	case ID_VA2:
			_config->number ("Harmonizer", "VA2", c);
    	    getApp ()->_harmon->vibrato (1,
    	    	_config->number ("Harmonizer", "VA2"),
	    		_config->number ("Harmonizer", "VF2"));
       		_vibratoAmpValues[1]->setText (v.str ().c_str ());
	    break;
	    case ID_VF2:
			_config->number ("Harmonizer", "VF2", c);
    	    getApp ()->_harmon->vibrato (1,
    	    	_config->number ("Harmonizer", "VA2"),
	    		_config->number ("Harmonizer", "VF2"));
 			_vibratoFreqValues[1]->setText (v.str ().c_str ());
        break;
    	case ID_VA3:
			_config->number ("Harmonizer", "VA3", c);
    	    getApp ()->_harmon->vibrato (2,
    	    	_config->number ("Harmonizer", "VA3"),
	    		_config->number ("Harmonizer", "VF3"));
 			_vibratoAmpValues[2]->setText (v.str ().c_str ());
	    break;
	    case ID_VF3:
			_config->number ("Harmonizer", "VF3", c);
    	    getApp ()->_harmon->vibrato (2,
    	    	_config->number ("Harmonizer", "VA3"),
	    		_config->number ("Harmonizer", "VF3"));
 			_vibratoFreqValues[2]->setText (v.str ().c_str ());
        break;
    	case ID_VA4:
			_config->number ("Harmonizer", "VA4", c);
    	    getApp ()->_harmon->vibrato (3,
    	    	_config->number ("Harmonizer", "VA4"),
	    		_config->number ("Harmonizer", "VF4"));
			_vibratoAmpValues[3]->setText (v.str ().c_str ());
	    break;
	    case ID_VF4:
			_config->number ("Harmonizer", "VF4", c);
    	    getApp ()->_harmon->vibrato (3,
    	    	_config->number ("Harmonizer", "VA4"),
	    		_config->number ("Harmonizer", "VF4"));
 			_vibratoFreqValues[3]->setText (v.str ().c_str ());
        break;
	}
	return 1;
}

long MainWindow::onCmdVibratoSliderText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s= (FXTextField*) sender;
   	std::stringstream v;
   	v << s->getText ().text ();
   	double c = atof (v.str ().c_str ());

    switch (FXSELID (sel)) {
    	case ID_VA1TEXT:
			_config->number ("Harmonizer", "VA1", c);
    	    getApp ()->_harmon->vibrato (0,
    	    	_config->number ("Harmonizer", "VA1"),
	    		_config->number ("Harmonizer", "VF1"));
 			_vibratoAmpSliders[0]->setValue (c);
	    break;
	    case ID_VF1TEXT:
			_config->number ("Harmonizer", "VF1", c);
    	    getApp ()->_harmon->vibrato (0,
    	    	_config->number ("Harmonizer", "VA1"),
	    		_config->number ("Harmonizer", "VF1"));
 			_vibratoFreqSliders[0]->setValue (c);
        break;
    	case ID_VA2TEXT:
			_config->number ("Harmonizer", "VA2", c);
    	    getApp ()->_harmon->vibrato (1,
    	    	_config->number ("Harmonizer", "VA2"),
	    		_config->number ("Harmonizer", "VF2"));
 			_vibratoAmpSliders[1]->setValue (c);
	    break;
	    case ID_VF2TEXT:
			_config->number ("Harmonizer", "VF2", c);
    	    getApp ()->_harmon->vibrato (1,
    	    	_config->number ("Harmonizer", "VA2"),
	    		_config->number ("Harmonizer", "VF2"));
 			_vibratoFreqSliders[1]->setValue (c);
        break;
    	case ID_VA3TEXT:
			_config->number ("Harmonizer", "VA3", c);
    	    getApp ()->_harmon->vibrato (2,
    	    	_config->number ("Harmonizer", "VA3"),
	    		_config->number ("Harmonizer", "VF3"));
 			_vibratoAmpSliders[2]->setValue (c);
	    break;
	    case ID_VF3TEXT:
			_config->number ("Harmonizer", "VF3", c);
    	    getApp ()->_harmon->vibrato (2,
    	    	_config->number ("Harmonizer", "VA3"),
	    		_config->number ("Harmonizer", "VF3"));
 			_vibratoFreqSliders[2]->setValue (c);
        break;
    	case ID_VA4TEXT:
			_config->number ("Harmonizer", "VA4", c);
    	    getApp ()->_harmon->vibrato (3,
    	    	_config->number ("Harmonizer", "VA4"),
	    		_config->number ("Harmonizer", "VF4"));
 			_vibratoAmpSliders[3]->setValue (c);
	    break;
	    case ID_VF4TEXT:
			_config->number ("Harmonizer", "VF4", c);
    	    getApp ()->_harmon->vibrato (3,
    	    	_config->number ("Harmonizer", "VA4"),
	    		_config->number ("Harmonizer", "VF4"));
 			_vibratoFreqSliders[3]->setValue (c);
        break;
	}
	return 1;
}

long MainWindow::onCmdRandVibratoSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

    switch (FXSELID (sel)) {
//    	case ID_VARAND1:
//			_config->number ("Harmonizer", "VARAND1", c);
//    	    getApp ()->_harmon->randomVibrato (0,
//    	    	_config->number ("Harmonizer", "VARAND1"),
//	    		_config->number ("Harmonizer", "VFRAND1"));
//       		_randVibratoAmpValues[0]->setText (v.str ().c_str ());
//	    break;
	    case ID_VFRAND1:
			_config->number ("Harmonizer", "VFRAND1", c);
    	    getApp ()->_harmon->randomVibrato (0,
    	    	/* _config->number ("Harmonizer", "VARAND1"), */
	    		_config->number ("Harmonizer", "VFRAND1"));
 			_randVibratoFreqValues[0]->setText (v.str ().c_str ());
        break;
//    	case ID_VARAND2:
//			_config->number ("Harmonizer", "VARAND2", c);
//    	    getApp ()->_harmon->randomVibrato (1,
//    	    	_config->number ("Harmonizer", "VARAND2"),
//	    		_config->number ("Harmonizer", "VFRAND2"));
//       		_randVibratoAmpValues[1]->setText (v.str ().c_str ());
//	    break;
	    case ID_VFRAND2:
			_config->number ("Harmonizer", "VFRAND2", c);
    	    getApp ()->_harmon->randomVibrato (1,
    	    	/* _config->number ("Harmonizer", "VARAND2"), */
	    		_config->number ("Harmonizer", "VFRAND2"));
 			_randVibratoFreqValues[1]->setText (v.str ().c_str ());
        break;
//    	case ID_VARAND3:
//			_config->number ("Harmonizer", "VARAND3", c);
//    	    getApp ()->_harmon->randomVibrato (2,
//    	    	_config->number ("Harmonizer", "VARAND3"),
//	    		_config->number ("Harmonizer", "VFRAND3"));
//       		_randVibratoAmpValues[2]->setText (v.str ().c_str ());
//	    break;
	    case ID_VFRAND3:
			_config->number ("Harmonizer", "VFRAND3", c);
    	    getApp ()->_harmon->randomVibrato (2,
    	    	/* _config->number ("Harmonizer", "VARAND3"), */
	    		_config->number ("Harmonizer", "VFRAND3"));
 			_randVibratoFreqValues[2]->setText (v.str ().c_str ());
        break;
//    	case ID_VARAND4:
//			_config->number ("Harmonizer", "VARAND4", c);
//    	    getApp ()->_harmon->randomVibrato (3,
//    	    	_config->number ("Harmonizer", "VARAND4"),
//	    		_config->number ("Harmonizer", "VFRAND4"));
//       		_randVibratoAmpValues[3]->setText (v.str ().c_str ());
//	    break;
	    case ID_VFRAND4:
			_config->number ("Harmonizer", "VFRAND4", c);
    	    getApp ()->_harmon->randomVibrato (3,
    	    	/* _config->number ("Harmonizer", "VARAND4"), */
	    		_config->number ("Harmonizer", "VFRAND4"));
 			_randVibratoFreqValues[3]->setText (v.str ().c_str ());
        break;
	}
	return 1;
}

long MainWindow::onCmdRandVibratoSliderText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s= (FXTextField*) sender;
   	std::stringstream v;
   	v << s->getText ().text ();
   	double c = atof (v.str ().c_str ());

    switch (FXSELID (sel)) {
//    	case ID_VARAND1TEXT:
//			_config->number ("Harmonizer", "VARAND1", c);
//    	    getApp ()->_harmon->randomVibrato (0,
//    	    	_config->number ("Harmonizer", "VARAND1"),
//	    		_config->number ("Harmonizer", "VFRAND1"));
// 			_randVibratoAmpSliders[0]->setValue (c);
//	    break;
	    case ID_VFRAND1TEXT:
			_config->number ("Harmonizer", "VFRAND1", c);
    	    getApp ()->_harmon->randomVibrato (0,
    	    	/* _config->number ("Harmonizer", "VARAND1"), */
	    		_config->number ("Harmonizer", "VFRAND1"));
 			_randVibratoFreqSliders[0]->setValue (c);
        break;
//    	case ID_VARAND2TEXT:
//			_config->number ("Harmonizer", "VARAND2", c);
//    	    getApp ()->_harmon->randomVibrato (1,
//    	    	_config->number ("Harmonizer", "VARAND2"),
//	    		_config->number ("Harmonizer", "VFRAND2"));
// 			_randVibratoAmpSliders[1]->setValue (c);
//	    break;
	    case ID_VFRAND2TEXT:
			_config->number ("Harmonizer", "VFRAND2", c);
    	    getApp ()->_harmon->randomVibrato (1,
    	    	/* _config->number ("Harmonizer", "VARAND2"), */
	    		_config->number ("Harmonizer", "VFRAND2"));
 			_randVibratoFreqSliders[1]->setValue (c);
        break;
//    	case ID_VARAND3TEXT:
//			_config->number ("Harmonizer", "VARAND3", c);
//    	    getApp ()->_harmon->randomVibrato (2,
//    	    	_config->number ("Harmonizer", "VARAND3"),
//	    		_config->number ("Harmonizer", "VFRAND31"));
// 			_randVibratoAmpSliders[2]->setValue (c);
//	    break;
	    case ID_VFRAND3TEXT:
			_config->number ("Harmonizer", "VFRAND3", c);
    	    getApp ()->_harmon->randomVibrato (2,
    	    	/* _config->number ("Harmonizer", "VARAND3"), */
	    		_config->number ("Harmonizer", "VFRAND3"));
 			_randVibratoFreqSliders[2]->setValue (c);
        break;
//    	case ID_VARAND4TEXT:
//			_config->number ("Harmonizer", "VARAND4", c);
//    	    getApp ()->_harmon->randomVibrato (3,
//    	    	_config->number ("Harmonizer", "VARAND4"),
//	    		_config->number ("Harmonizer", "VFRAND4"));
// 			_randVibratoAmpSliders[3]->setValue (c);
//	    break;
	    case ID_VFRAND4TEXT:
			_config->number ("Harmonizer", "VFRAND4", c);
    	    getApp ()->_harmon->randomVibrato (3,
    	    	/* _config->number ("Harmonizer", "VARAND4"), */
	    		_config->number ("Harmonizer", "VFRAND4"));
 			_randVibratoFreqSliders[3]->setValue (c);
        break;
	}
	return 1;
}

long MainWindow::onCmdPanningLogic (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "PanningLogic", (double)
 		c->getCurrentItem ());

    switch (c->getCurrentItem ()) {
    	case 0:
    	    getApp ()->_harmon->disposition (Pandora::CUSTOM_POSITION);
    	    for (int i = 0; i < PARTIALS; i++) {
    	        FXString temp;
    	        temp.format ("On%d", i + 1);
    	        if (_config->number ("Harmonizer", temp.text ()) == true) {
                	_panProgress[i]->enable ();
            		_panSliders[i]->enable ();
      			}
         	}
         	_defaultButton3->enable ();
  			_randomButton3->enable ();
        break;
    	case 1:
         	getApp ()->_harmon->disposition (Pandora::TWO_TWO);
    	    for (int i = 0; i < PARTIALS; i++) {
               	_panProgress[i]->disable ();
        		_panSliders[i]->disable ();
         	}
         	_defaultButton3->disable ();
  			_randomButton3->disable ();
        break;
    	case 2:
         	getApp ()->_harmon->disposition (Pandora::ONE_THREE);
    	    for (int i = 0; i < PARTIALS; i++) {
               	_panProgress[i]->disable ();
        		_panSliders[i]->disable ();
         	}
         	_defaultButton3->disable ();
  			_randomButton3->disable ();
        break;
    	case 3:
         	getApp ()->_harmon->disposition (Pandora::THREE_ONE);
    	    for (int i = 0; i < PARTIALS; i++) {
               	_panProgress[i]->disable ();
        		_panSliders[i]->disable ();
         	}
         	_defaultButton3->disable ();
  			_randomButton3->disable ();
        break;
    }
    return 1;
}

long MainWindow::onCmdDefault3 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double pan = .5;
		FXString temp;

    	_panSliders[i]->setValue (pan);
    	_panProgress[i]->setProgress ((int) (pan * 100));
    	temp.format ("%g", pan);
    	temp.format ("P%d", i + 1);
     	_config->number ("Harmonizer", temp.text (), pan);
        getApp ()->_harmon->panning (i,
        	_config->number ("Harmonizer", temp.text ()));
	}
	return 1;
}

long MainWindow::onCmdRandom3 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double pan = getApp ()->_rrand3->tick ();
		FXString temp;

    	_panSliders[i]->setValue (pan);
    	_panProgress[i]->setProgress ((int) (pan * 100));
    	temp.format ("%g", pan);
    	temp.format ("P%d", i + 1);
     	_config->number ("Harmonizer", temp.text (), pan);
        getApp ()->_harmon->panning (i,
        	_config->number ("Harmonizer", temp.text ()));
	}
	return 1;
}

long MainWindow::onCmdPanSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

    switch (FXSELID (sel)) {
    	case ID_PANNING1:
			_config->number ("Harmonizer", "P1", c);
    	    getApp ()->_harmon->panning (0, c);
         	_panProgress[0]->setProgress ((int) (c * 100));
	    break;
    	case ID_PANNING2:
			_config->number ("Harmonizer", "P2", c);
    	    getApp ()->_harmon->panning (1, c);
         	_panProgress[1]->setProgress ((int) (c * 100));
	    break;
    	case ID_PANNING3:
			_config->number ("Harmonizer", "P3", c);
    	    getApp ()->_harmon->panning (2, c);
         	_panProgress[2]->setProgress ((int) (c * 100));
	    break;
    	case ID_PANNING4:
			_config->number ("Harmonizer", "P4", c);
    	    getApp ()->_harmon->panning (3, c);
         	_panProgress[3]->setProgress ((int) (c * 100));
	    break;
	}
	return 1;
}

///////////////**********//////////////////

long MainWindow::onCmdFormantsLogic (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
	_config->number ("Harmonizer", "FormantsLogic", (double)
 		c->getCurrentItem ());

    switch (c->getCurrentItem ()) {
    	case 0:
    	    getApp ()->_harmon->choral (Pandora::CUSTOM_CHORAL);
    	    for (int i = 0; i < PARTIALS; i++) {
    	        FXString temp;
    	        temp.format ("On%d", i + 1);
    	        if (_config->number ("Harmonizer", temp.text ()) == true) {
//                	_panProgress[i]->enable ();
            		_formantsSliders[i]->enable ();
      			}
         	}
         	_defaultButton4->enable ();
  			_randomButton4->enable ();
        break;
    	case 1:
         	getApp ()->_harmon->choral (Pandora::MIXED_CHORAL);
    	    for (int i = 0; i < PARTIALS; i++) {
//               	_panProgress[i]->disable ();
        		_formantsSliders[i]->disable ();
         	}
         	_defaultButton4->disable ();
  			_randomButton4->disable ();
        break;
    	case 2:
         	getApp ()->_harmon->choral (Pandora::FEMALE_CHORAL);
    	    for (int i = 0; i < PARTIALS; i++) {
//               	_panProgress[i]->disable ();
        		_formantsSliders[i]->disable ();
         	}
         	_defaultButton4->disable ();
  			_randomButton4->disable ();
        break;
    	case 3:
         	getApp ()->_harmon->choral (Pandora::MALE_CHORAL);
    	    for (int i = 0; i < PARTIALS; i++) {
//               	_panProgress[i]->disable ();
        		_formantsSliders[i]->disable ();
         	}
         	_defaultButton4->disable ();
  			_randomButton4->disable ();
        break;
    }
    return 1;
}

long MainWindow::onCmdDefault4 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double f = 1;
		FXString temp;

    	_formantsSliders[i]->setValue (f);
//    	_panProgress[i]->setProgress ((int) (pan * 100));
    	temp.format ("%g", f);
    	temp.format ("FORM%d", i + 1);
     	_config->number ("Harmonizer", temp.text (), f);
        getApp ()->_harmon->formant (i,
        	_config->number ("Harmonizer", temp.text ()));
	}
	return 1;
}

long MainWindow::onCmdRandom4 (FXObject* sender, FXSelector sel, void*) {
	for (int i = 0; i < PARTIALS; i++) {
   		double f = getApp ()->_rrand4->tick ();
		FXString temp;

    	_formantsSliders[i]->setValue (f);
//    	_panProgress[i]->setProgress ((int) (pan * 100));
    	temp.format ("%g", f);
    	temp.format ("FORM%d", i + 1);
     	_config->number ("Harmonizer", temp.text (), f);
        getApp ()->_harmon->formant (i,
        	_config->number ("Harmonizer", temp.text ()));
	}
	return 1;
}

long MainWindow::onCmdFormantSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

    switch (FXSELID (sel)) {
    	case ID_FORMANT1:
			_config->number ("Harmonizer", "FORM1", c);
    	    getApp ()->_harmon->formant (0, c);
         	//_panProgress[0]->setProgress ((int) (c * 100));
	    break;
    	case ID_FORMANT2:
			_config->number ("Harmonizer", "FORM2", c);
    	    getApp ()->_harmon->formant (1, c);
         	//_panProgress[1]->setProgress ((int) (c * 100));
	    break;
    	case ID_FORMANT3:
			_config->number ("Harmonizer", "FORM3", c);
    	    getApp ()->_harmon->formant (2, c);
         	//_panProgress[2]->setProgress ((int) (c * 100));
	    break;
    	case ID_FORMANT4:
			_config->number ("Harmonizer", "FORM4", c);
    	    getApp ()->_harmon->formant (3, c);
         	//_panProgress[3]->setProgress ((int) (c * 100));
	    break;
	}
	return 1;
}

/////////************////////////

long MainWindow::onCmdVoiceCheck (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

    switch (FXSELID (sel)) {
    	case ID_VOICECHECK1:
			getApp ()->_harmon->status (0, b);
			_config->number ("Harmonizer", "On1", (double) b);
  		break;
    	case ID_VOICECHECK2:
			getApp ()->_harmon->status (1, b);
			_config->number ("Harmonizer", "On2", (double) b);
  		break;
    	case ID_VOICECHECK3:
			getApp ()->_harmon->status (2, b);
			_config->number ("Harmonizer", "On3", (double) b);
  		break;
    	case ID_VOICECHECK4:
			getApp ()->_harmon->status (3, b);
			_config->number ("Harmonizer", "On4", (double) b);
  		break;
	}

	if (_config->number ("Harmonizer", "Logic") == 0) {
	    for (int i = 0; i < PARTIALS; i++) {
	        FXString temp;
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
            	_partialsAmpValues[i]->enable ();
        		_partialsAmpSliders[i]->enable ();
				_partialsFreqValues[i]->enable ();
        		_partialsFreqSliders[i]->enable ();
  			}
     		else {
             	_partialsAmpValues[i]->disable ();
        		_partialsAmpSliders[i]->disable ();
				_partialsFreqValues[i]->disable ();
        		_partialsFreqSliders[i]->disable ();
           }
     	}
	}

	if (_config->number ("Harmonizer", "VibratoLogic") == 0) {
	    for (int i = 0; i < PARTIALS; i++) {
	        FXString temp;
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
            	_vibratoAmpValues[i]->enable ();
        		_vibratoAmpSliders[i]->enable ();
				_vibratoFreqValues[i]->enable ();
        		_vibratoFreqSliders[i]->enable ();
  			}
     		else {
             	_vibratoAmpValues[i]->disable ();
        		_vibratoAmpSliders[i]->disable ();
				_vibratoFreqValues[i]->disable ();
        		_vibratoFreqSliders[i]->disable ();
           }
     	}
	}
	if (_config->number ("Harmonizer", "RandomVibratoLogic") == 0) {
	    for (int i = 0; i < PARTIALS; i++) {
	        FXString temp;
	        temp.format ("On%d", i + 1);
	        if (_config->number ("Harmonizer", temp.text ()) == true) {
//            	_randVibratoAmpValues[i]->enable ();
//        		_randVibratoAmpSliders[i]->enable ();
				_randVibratoFreqValues[i]->enable ();
        		_randVibratoFreqSliders[i]->enable ();
  			}
     		else {
//             	_randVibratoAmpValues[i]->disable ();
//        		_randVibratoAmpSliders[i]->disable ();
				_randVibratoFreqValues[i]->disable ();
        		_randVibratoFreqSliders[i]->disable ();
           }
     	}
	}
	if (_config->number ("Harmonizer", "PanningLogic") == 0) {
        for (int i = 0; i < PARTIALS; i++) {
            FXString temp;
            temp.format ("On%d", i + 1);
            if (_config->number ("Harmonizer", temp.text ()) == true) {
           		_panSliders[i]->enable ();
           		_panProgress[i]->enable ();
    		}
    		else {
             	_panSliders[i]->disable ();
              	_panProgress[i]->disable ();
    		}
     	}
    }
	if (_config->number ("Harmonizer", "FormantsLogic") == 0) {
        for (int i = 0; i < PARTIALS; i++) {
            FXString temp;
            temp.format ("On%d", i + 1);
            if (_config->number ("Harmonizer", temp.text ()) == true) {
           		_formantsSliders[i]->enable ();
           		//_panProgress[i]->enable ();
    		}
    		else {
             	_formantsSliders[i]->disable ();
              	//_panProgress[i]->disable ();
    		}
     	}
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
 	return 1;
}

long MainWindow::onCmdVoiceFilter (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

    switch (FXSELID (sel)) {
    	case ID_VOICEFILTER1:
			getApp ()->_harmon->highpass (0, b);
			_config->number ("Harmonizer", "HP1", (double) b);
  		break;
    	case ID_VOICEFILTER2:
			getApp ()->_harmon->highpass (1, b);
			_config->number ("Harmonizer", "HP2", (double) b);
  		break;
    	case ID_VOICEFILTER3:
			getApp ()->_harmon->highpass (2, b);
			_config->number ("Harmonizer", "HP3", (double) b);
  		break;
    	case ID_VOICEFILTER4:
			getApp ()->_harmon->highpass (3, b);
			_config->number ("Harmonizer", "HP4", (double) b);
  		break;
	}
	return 1;
}

long MainWindow::onCmdRadio (FXObject* sender, FXSelector sel, void*) {
	_radioButtons[0]->setCheck (false);
	_radioButtons[1]->setCheck (false);
	_radioButtons[2]->setCheck (false);

    switch (FXSELID (sel)) {
    	case ID_RADIO1:
			_config->number ("Harmonizer", "FundamentalType", 0);
			getApp ()->_useMIDIFreq = true;
			getApp ()->_harmon->fundamental (1);
       		_radioButtons[0]->setCheck (true);
         	_transposeCombo->disable ();
          	_ratioField->disable ();
          	_ratioDial->disable ();
	    break;
    	case ID_RADIO2: {
			_config->number ("Harmonizer", "FundamentalType", 1);
			getApp ()->_useMIDIFreq = false;
			int t = (int) _config->number ("Harmonizer", "Semitones");
            t -= 24;
            if (t < 0) {
            	t = abs (t);
        		double r = 1 / pow (TUNINGSEMITONE, (double) t);
            	getApp ()->_harmon->fundamental (r);
            }
            else if (t == 0) {
            	getApp ()->_harmon->fundamental (1);
            }
            else {
        		double r = pow (TUNINGSEMITONE, (double) t);
            	getApp ()->_harmon->fundamental (r);
            }
       		_radioButtons[1]->setCheck (true);
         	_transposeCombo->enable ();
          	_ratioField->disable ();
          	_ratioDial->disable ();
       	}
	    break;
    	case ID_RADIO3:
			_config->number ("Harmonizer", "FundamentalType", 2);
			getApp ()->_useMIDIFreq = false;
    	    getApp ()->_harmon->fundamental (_config->number ("Harmonizer",
         		"FundamentalRatio"));
       		_radioButtons[2]->setCheck (true);
         	_transposeCombo->disable ();
          	_ratioField->enable ();
          	_ratioDial->enable ();
	    break;
	}
	return 1;
}

long MainWindow::onCmdSemitones (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int t = c->getCurrentItem ();
    _config->number ("Harmonizer", "Semitones", t);
    t -= 24;
    if (t < 0) {
    	t = abs (t);
		double r = 1 / pow (TUNINGSEMITONE, (double) t);
    	getApp ()->_harmon->fundamental (r);
    }
    else if (t == 0) {
    	getApp ()->_harmon->fundamental (1);
    }
    else {
		double r = pow (TUNINGSEMITONE, (double) t);
    	getApp ()->_harmon->fundamental (r);
    }
    return 1;
}

long MainWindow::onCmdManual (FXObject* sender, FXSelector sel, void*) {
    FXDial* d = (FXDial*) sender;
    int t = d->getValue ();
    double r = t;
    r /= 100;
    _config->number ("Harmonizer", "FundamentalRatio", r);
    getApp ()->_harmon->fundamental (r);
    std::stringstream s;
    s << r;
    _ratioField->setText (s.str ().c_str ());
    return 1;
}

long MainWindow::onCmdManualText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "FundamentalRatio", r);
    getApp ()->_harmon->fundamental (r);
   _ratioDial->setValue ((int) (r * 100));
   return 1;
}

long MainWindow::onCmdEnvelope (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int it = c->getCurrentItem ();
    switch (it) {
        case 0:
    		getApp ()->_harmon->grainEnvelope (Pandora::RECTANGLE);
    	break;
        case 1:
    		getApp ()->_harmon->grainEnvelope (Pandora::BARTLETT);
    	break;
        case 2:
    		getApp ()->_harmon->grainEnvelope (Pandora::GAUSSIAN);
    	break;
        case 3:
    		getApp ()->_harmon->grainEnvelope (Pandora::HAMMING);
    	break;
        case 4:
    		getApp ()->_harmon->grainEnvelope (Pandora::HANNING);
    	break;
        case 5:
    		getApp ()->_harmon->grainEnvelope (Pandora::BLACKMAN);
    	break;
        case 6:
    		getApp ()->_harmon->grainEnvelope (Pandora::BLACKMAN_HARRIS);
    	break;
	}

    _config->number ("Harmonizer", "WindowType", it);
    return 1;
}

long MainWindow::onCmdAttackDecay (FXObject* sender, FXSelector sel, void*) {
    FXSlider* s = (FXSlider*) sender;
	double v = (double) s->getValue ();
    switch (FXSELID (sel)) {
        case ID_ATTACK:
			_attackProgress->setProgress ((int) v);
			_config->number ("Harmonizer", "Attack", v);
        break;
        case ID_DECAY:
			_decayProgress->setProgress ((int) v);
			_config->number ("Harmonizer", "Decay", v);
        break;
  	}

	getApp ()->_harmon->attackDecay (
		_config->number ("Harmonizer", "Attack") / 100,
		_config->number ("Harmonizer", "Decay") / 100);
	return 1;
}

long MainWindow::onCmdAutoMode (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

	if (b) {
    	_cyclesSpinner->disable ();
 	}
 	else {
    	_cyclesSpinner->enable ();
  	}

   	_config->number ("Harmonizer", "AutoMode", (double) b);
   	getApp ()->_harmon->autoMode (b);
   	return 1;
}

long MainWindow::onCmdCycles (FXObject* sender, FXSelector sel, void*) {
    FXSpinner* s = (FXSpinner*) sender;
	int b = s->getValue ();
   	_config->number ("Harmonizer", "Cycles", (double) b);
   	getApp ()->_harmon->cyclesPerGrain (b);
   	return 1;
}

long MainWindow::onCmdFreqThreshold (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "FreqThreshold", b);
   	getApp ()->_harmon->freqThreshold (b);
   	std::stringstream t;
   	t << b;
   	_freqThreshField->setText (t.str ().c_str ());
   	return 1;
}

long MainWindow::onCmdFreqThresholdText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_freqThreshSlider->setValue (b);
   	_config->number ("Harmonizer", "FreqThreshold", b);
   	getApp ()->_harmon->freqThreshold (b);
   	return 1;
}

long MainWindow::onCmdSemitonesThresh (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int it = c->getCurrentItem ();
    switch (it) {
        case 0:
		   	getApp ()->_harmon->freqThreshold (1);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 1:
		   	getApp ()->_harmon->freqThreshold (1.0595);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 2:
		   	getApp ()->_harmon->freqThreshold (1.1225);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 3:
		   	getApp ()->_harmon->freqThreshold (1.1892);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 4:
		   	getApp ()->_harmon->freqThreshold (1.2599);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 5:
		   	getApp ()->_harmon->freqThreshold (1.3348);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 6:
		   	getApp ()->_harmon->freqThreshold (1.4142);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 7:
		   	getApp ()->_harmon->freqThreshold (1.4983);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 8:
		   	getApp ()->_harmon->freqThreshold (1.5874);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 9:
		   	getApp ()->_harmon->freqThreshold (1.6818);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 10:
		   	getApp ()->_harmon->freqThreshold (1.7818);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 11:
		   	getApp ()->_harmon->freqThreshold (1.8897);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
        case 12:
		   	getApp ()->_harmon->freqThreshold (2);
		   	_freqThreshSlider->disable ();
		   	_freqThreshField->disable ();
    	break;
    	case 13:
    	   getApp ()->_harmon->freqThreshold (
        		_config->number ("Harmonizer", "FreqThreshold"));
		   	_freqThreshSlider->enable ();
		   	_freqThreshField->enable ();
	    break;
	}

    _config->number ("Harmonizer", "SemitonesTresh", it);
    return 1;
}

long MainWindow::onCmdMasterKey (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int it = c->getCurrentItem ();
	getApp ()->_harmon->masterKey ((Pandora::notes) it);
    _config->number ("Harmonizer", "MasterKey", it);
    return 1;
}

long MainWindow::onCmdScale (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int it = c->getCurrentItem ();
    switch (it) {
        case 0:
		   	getApp ()->_harmon->scale (Pandora::MAJOR1);
    	break;
        case 1:
		   	getApp ()->_harmon->scale (Pandora::MAJOR2);
    	break;
    	case 2:
		   	getApp ()->_harmon->scale (Pandora::MINOR1);
    	break;
    	case 3:
		 	getApp ()->_harmon->scale (Pandora::MINOR2);
    	break;
    	case 4:
		   	getApp ()->_harmon->scale (Pandora::MINOR3);
    	break;
    	case 5:
		   	getApp ()->_harmon->scale (Pandora::BLUES1);
    	break;
//    	case 6:
//		   	getApp ()->_harmon->scale (Pandora::CHROMATIC_MAJOR1);
//    	break;
//    	case 6:
//		   	getApp ()->_harmon->scale (Pandora::OCTOPHONIC1);
//    	break;
    }

    _config->number ("Harmonizer", "Scale", it);
    return 1;
}

long MainWindow::onCmdJitter (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double v = (double) s->getValue ();
	_jitterProgress->setProgress ((int) (v * 100));
	_config->number ("Harmonizer", "Jitter", v);
   	getApp ()->_harmon->spatialPosition (v);
   	return 1;
}

long MainWindow::onCmdPitchDetector (FXObject* sender, FXSelector sel, void*) {
    getApp ()->_harmon->mute (true);
    switch (FXSELID (sel)) {
    	case ID_PDET1:
    	    getApp ()->_pdet = getApp ()->_period;
			_config->number ("Harmonizer", "PitchDetector", 0);
        break;
    	case ID_PDET2:
         	getApp ()->_pdet = getApp ()->_autoTune;
			_config->number ("Harmonizer", "PitchDetector", 1);
        break;
    	case ID_PDET3:
         	getApp ()->_pdet = getApp ()->_spectral;
			_config->number ("Harmonizer", "PitchDetector", 2);
        break;
	}
////	getApp ()->_pdets.clear ();
////	for (int i = 0; i < PARTIALS; i++) {
//    	if (_config->number ("Harmonizer", "PitchDetector") == 0) {
//    //		getApp ()->_pdets.push_back (getApp ()->_periods[i]);
//    		getApp ()->_pdet = getApp ()->_period;
//    	}
//    	else {
//    		//getApp ()->_pdets.push_back (getApp ()->_autoTunes[i]);
//    		getApp ()->_pdet = getApp ()->_autoTune;
//    	}
////    }
//    //getApp ()->_harmon->pitchDetectors (getApp ()->_pdets);
    getApp ()->_harmon->pitchDetector (getApp ()->_pdet);
	getApp ()->_harmon->mute (false);
	return 1;
}

long MainWindow::onCmdTrackPitch (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Harmonizer", "TrackPitch", (double) b);
   trackPitch = b;
   return 1;
}

long MainWindow::onCmdBypass (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Harmonizer", "Bypass", (double) b);
	getApp()->_harmon->bypass (b);
	return 1;
}

long MainWindow::onCmdFreeze (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Harmonizer", "Freeze", (double) b);
	getApp()->_harmon->freeze (b);
	return 1;
}

long MainWindow::onCmdAntiRumble (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Harmonizer", "AntiRumble", (double) b);
	getApp ()->_harmon->antiRumble (b);
	return 1;
}

long MainWindow::onCmdFeedback (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "Feedback", b);
   	std::stringstream t;
   	t << b;
   	_feedbackField->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->feedback (b);
// 	}
	return 1;
}

long MainWindow::onCmdFeedbackText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_feedbackSlider->setValue (b);
   	_config->number ("Harmonizer", "Feedback", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->feedback (b);
// 	}
	return 1;
}

long MainWindow::onCmdQuality (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "FilterQuality", b);
   	std::stringstream t;
   	t << b;
   	_qualityField->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdQualityText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_qualitySlider->setValue (b);
   	_config->number ("Harmonizer", "FilterQuality", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdSplit (FXObject* sender, FXSelector sel, void*) {
    FXSpinner* s = (FXSpinner*) sender;
	int b = s->getValue ();
   	_config->number ("Harmonizer", "Split", (double) b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->split (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThreshold (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "AmpThreshold", b);
   	std::stringstream t;
   	t << b;
   	_ampThreshField->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->threshold (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThresholdText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_ampThreshSlider->setValue (b);
   	_config->number ("Harmonizer", "AmpThreshold", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_period->threshold (b);
// 	}
	return 1;
}

long MainWindow::onCmdZCCycles (FXObject* sender, FXSelector sel, void*) {
//    FXSpinner* s = (FXSpinner*) sender;
//	int b = s->getValue ();
//   	_config->number ("Harmonizer", "ZCCycles", (double) b);
//	for (int i = 0; i < PARTIALS; i++) {
//    	getApp ()->_periods[i]->cycles (b);
// 	}
	return 1;
}

long MainWindow::onCmdFilters (FXObject* sender, FXSelector sel, void*) {
	FXDialogBox data (this, _stringTable->get ("DATADLG").c_str (),
		DECOR_TITLE | DECOR_BORDER,
		0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame* cont = new FXVerticalFrame (&data,
 		LAYOUT_FILL_Y | LAYOUT_FILL_X,
		0,0,0,0, 0,0,0,0, 0,0);
	FXHorizontalFrame* firstFrame = new FXHorizontalFrame (cont,
		LAYOUT_FILL_X);
	FXGroupBox* rangeBox = new FXGroupBox (firstFrame,
		_stringTable->get ("RANGESTARTEND").c_str (),
		FRAME_THICK | LAYOUT_FILL_X | LAYOUT_TOP);
	FXHorizontalFrame* rangeFrame = new FXHorizontalFrame (rangeBox,
		LAYOUT_FILL_X);
	FXTextField* minField = new FXTextField (rangeFrame, 0, NULL, 0,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
		0,0,100,0, 4,4,4,4);
  	std::stringstream min;
 	min << _config->number ("Harmonizer", "RangeStart");
  	minField->setText (min.str ().c_str ());
  	FXTextField* maxField = new FXTextField (rangeFrame, 0, NULL, 0,
		FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH | TEXTFIELD_REAL,
		0,0,100,0, 4,4,4,4);
  	std::stringstream max;
 	max << _config->number ("Harmonizer", "RangeEnd");
  	maxField->setText (max.str ().c_str ());
	FXHorizontalFrame* secondFrame = new FXHorizontalFrame (cont,
		LAYOUT_FILL_X);
	FXGroupBox* levelsBox = new FXGroupBox (secondFrame,
		_stringTable->get ("FILTERLEVELS").c_str (),
  		FRAME_THICK | LAYOUT_FILL_X);
	FXHorizontalFrame* levelsFrame = new FXHorizontalFrame (levelsBox,
		LAYOUT_FILL_X);
    FXSpinner* levels =  new FXSpinner (levelsFrame, 0, NULL, 0,
    	SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    levels->setRange (1, 50);
    levels->setValue ((int) _config->number ("Harmonizer", "Levels"));
//    FXCheckButton* useBiquad = new FXCheckButton (secondFrame,
//    	_stringTable->get ("USEBIQUAD").c_str (), NULL, 0,
//     	ICON_BEFORE_TEXT | LAYOUT_CENTER_Y);
// 	if (_config->number ("Harmonizer", "UseBiquad") != 0) {
//    	useBiquad->setCheck (true);
//  	}
    new FXHorizontalSeparator (cont, SEPARATOR_RIDGE | LAYOUT_FILL_X);
    FXHorizontalFrame* closeBox = new FXHorizontalFrame (cont,
    	LAYOUT_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH);
    new FXButton (closeBox, _stringTable->get ("OKBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_ACCEPT,
    	BUTTON_INITIAL | BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED
     	| FRAME_THICK, 0,0,0,0, 20,20);
    new FXButton (closeBox, _stringTable->get ("CANCELBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_CANCEL,
    	BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED | FRAME_THICK,
     	0,0,0,0, 20,20);

	if (data.execute (PLACEMENT_OWNER)) {
    	double min = atof (minField->getText ().text ());
     	double max = atof (maxField->getText ().text ());
      	int lev = levels->getValue ();
//       	bool biquad = useBiquad->getCheck ();

        if (min > max) {
        	FXMessageBox a (this, _stringTable->get ("TITLE").c_str (),
         		_stringTable->get ("RANGEERROR").c_str (), 0, MBOX_OK);
          	a.execute ();
    	}
    	else {
         	getApp ()->_harmon->mute (true);
        	//delete [] getApp ()->_periods;
        	delete getApp ()->_period;

        	_config->number ("Harmonizer", "RangeStart", min);
        	_config->number ("Harmonizer", "RangeEnd", max);
        	_config->number ("Harmonizer", "Levels", (int) lev);
//        	_config->number ("Harmonizer", "UseBiquad", (double) biquad);

//         	getApp ()->_pdets.clear ();
//         	for (int i = 0; i < PARTIALS; i++) {
        		getApp ()->_period = new PeriodEstimator (
             		getApp ()->_info, _config->number ("Harmonizer",
               		"RangeStart"),
               		_config->number ("Harmonizer", "RangeEnd"),
                 	(int) _config->number ("Harmonizer", "Levels"),
                  	(int) _config->number ("Harmonizer", "Split"),
                   	_config->number ("Harmonizer", "AmpThreshold"));
//               	getApp ()->_periods[i]->cycles ((int)
//               		_config->number ("Harmonizer", "ZCCycles"));
              	getApp ()->_period->filterQuality (
              		_config->number ("Harmonizer", "FilterQuality"));
              	getApp ()->_period->feedback (
              		_config->number ("Harmonizer", "Feedback"));
              	getApp ()->_period->smoothingQuality (
              		_config->number ("Harmonizer", "SmoothingQuality"));
              	getApp ()->_period->smoothingRate (
              		_config->number ("Harmonizer", "SmoothingRate"));

//           		getApp ()->_pdets.push_back (getApp ()->_periods[i]);
//       		}
//            getApp ()->_harmon->pitchDetectors (getApp ()->_pdets);
			getApp ()->_pdet = getApp ()->_period;
			getApp ()->_harmon->pitchDetector (getApp ()->_pdet);
        	getApp ()->_harmon->mute (false);

        	std::stringstream filStat;
        	filStat << _stringTable->get ("FILTERLEVELS")
        		<< ": " << _config->number ("Harmonizer", "Levels") <<
        		" - " << _stringTable->get ("RANGESTARTEND") <<
        		": " << _config->number ("Harmonizer", "RangeStart") << ", " <<
        			_config->number ("Harmonizer", "RangeEnd");
        	_filterStatus->setText (filStat.str ().c_str ());
     	}
 	}
 	return 1;
}

long MainWindow::onCmdSmoothingQual (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* d = (FXRealSlider*) sender;
    double r = (double) d->getValue ();
    _config->number ("Harmonizer", "SmoothingQuality", r);
    getApp ()->_period->smoothingQuality (r);
    std::stringstream s;
    s << r;
    _smoothQualField->setText (s.str ().c_str ());
    return 1;
}

long MainWindow::onCmdSmoothingQualText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingQuality", r);
    getApp ()->_period->smoothingQuality (r);
   _smoothQualSlider->setValue ((r));
   return 1;
}

long MainWindow::onCmdSmoothing (FXObject* sender, FXSelector sel, void*) {
    FXDial* d = (FXDial*) sender;
    int t = d->getValue ();
    double r = (double) t;
    _config->number ("Harmonizer", "SmoothingRate", r);
    getApp ()->_period->smoothingRate (r);
    std::stringstream s;
    s << r;
    _smoothField->setText (s.str ().c_str ());
    return 1;
}

long MainWindow::onCmdSmoothingText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingRate", r);
    getApp ()->_period->smoothingRate (r);
   _smoothDial->setValue ((int) (r));
   return 1;
}

long MainWindow::onCmdRangeStartEnd1 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double v = (double) s->getValue ();
	std::stringstream t;
	t << v;
    switch (FXSELID (sel)) {
        case ID_RANGESTART1:
			_rangeStartField1->setText (t.str ().c_str ());
			_config->number ("Harmonizer", "RangeStart1", v);
			if (v > _config->number ("Harmonizer", "RangeEnd1")) {
				_config->number ("Harmonizer", "RangeEnd1", v);
    			_rangeEndSlider1->setValue (v);
       			_rangeEndField1->setText (t.str ().c_str ());
       		}
        break;
        case ID_RANGEEND1:
			_rangeEndField1->setText (t.str ().c_str ());
			_config->number ("Harmonizer", "RangeEnd1", v);
			if (v < _config->number ("Harmonizer", "RangeStart1")) {
				_config->number ("Harmonizer", "RangeStart1", v);
    			_rangeStartSlider1->setValue (v);
       			_rangeStartField1->setText (t.str ().c_str ());
       		}
        break;
  	}
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->range (_config->number ("Harmonizer",
     		"RangeStart1"),
     		_config->number ("Harmonizer", "RangeEnd1"));
// 	}
	return 1;
}

long MainWindow::onCmdRangeStartEndText1 (FXObject* sender,
	FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double v = atof (t.str ().c_str ());
    switch (FXSELID (sel)) {
        case ID_RANGESTARTTEXT1:
			_rangeStartSlider1->setValue (v);
			_config->number ("Harmonizer", "RangeStart1", v);
			if (v > _config->number ("Harmonizer", "RangeEnd1")) {
				_config->number ("Harmonizer", "RangeEnd1", v);
    			_rangeEndSlider1->setValue (v);
       			_rangeEndField1->setText (t.str ().c_str ());
       		}
        break;
        case ID_RANGEENDTEXT1:
			_rangeEndSlider1->setValue (v);
			_config->number ("Harmonizer", "RangeEnd1", v);
			if (v < _config->number ("Harmonizer", "RangeStart1")) {
				_config->number ("Harmonizer", "RangeStart1", v);
    			_rangeStartSlider1->setValue (v);
       			_rangeStartField1->setText (t.str ().c_str ());
       		}
        break;
  	}
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->range (_config->number ("Harmonizer",
     		"RangeStart1"),
     		_config->number ("Harmonizer", "RangeEnd1"));
// 	}
	return 1;
}

long MainWindow::onCmdQuality1 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "FilterQuality1", b);
   	std::stringstream t;
   	t << b;
   	_qualityField1->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdQualityText1 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_qualitySlider1->setValue (b);
   	_config->number ("Harmonizer", "FilterQuality1", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThreshold1 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "AmpThreshold1", b);
   	std::stringstream t;
   	t << b;
   	_ampThreshField1->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->threshold (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThresholdText1 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_ampThreshSlider1->setValue (b);
   	_config->number ("Harmonizer", "AmpThreshold1", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_autoTune->threshold (b);
//	}
	return 1;
}

//long MainWindow::onCmdSpeed1 (FXObject* sender, FXSelector sel, void*) {
//    FXRealSlider* s = (FXRealSlider*) sender;
//	double b = s->getValue ();
//   	_config->number ("Harmonizer", "Speed1", b);
//   	std::stringstream t;
//   	t << b;
//   	_speedField1->setText (t.str ().c_str ());
////	for (int i = 0; i < PARTIALS; i++) {
//    	getApp ()->_autoTune->speed (b);
//// 	}
//	return 1;
//}

//long MainWindow::onCmdSpeedText1 (FXObject* sender, FXSelector sel, void*) {
//    FXTextField* s = (FXTextField*) sender;
//	std::stringstream t;
//	t << s->getText ().text ();
//	double b = atof (t.str ().c_str ());
// 	_speedSlider1->setValue (b);
//   	_config->number ("Harmonizer", "Speed1", b);
////	for (int i = 0; i < PARTIALS; i++) {
//    	getApp ()->_autoTune->speed (b);
//// 	}
//	return 1;
//}

long MainWindow::onCmdSmoothingQual1 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* d = (FXRealSlider*) sender;
    double r = (double) d->getValue ();
    _config->number ("Harmonizer", "SmoothingQuality1", r);
    getApp ()->_autoTune->smoothingQuality (r);
    std::stringstream s;
    s << r;
    _smoothQualFieldATune->setText (s.str ().c_str ());
//    PRINT ("smooth qual 1")
    return 1;
}

long MainWindow::onCmdSmoothingQualText1 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingQuality1", r);
    getApp ()->_autoTune->smoothingQuality (r);
   _smoothQualSliderATune->setValue ((r));
   return 1;
}

long MainWindow::onCmdFilters1 (FXObject* sender, FXSelector sel, void*) {
	FXDialogBox data (this, _stringTable->get ("DATADLG").c_str (),
		DECOR_TITLE | DECOR_BORDER,
		0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame* cont = new FXVerticalFrame (&data,
 		LAYOUT_FILL_Y | LAYOUT_FILL_X,
		0,0,0,0, 0,0,0,0, 0,0);
	FXHorizontalFrame* firstFrame = new FXHorizontalFrame (cont,
		LAYOUT_FILL_X);
	FXGroupBox* levelsBox = new FXGroupBox (firstFrame,
		_stringTable->get ("FILTERLEVELS").c_str (),
  		FRAME_THICK | LAYOUT_FILL_X);
	FXHorizontalFrame* levelsFrame = new FXHorizontalFrame (levelsBox,
		LAYOUT_FILL_X);
    FXSpinner* levels =  new FXSpinner (levelsFrame, 0, NULL, 0,
    	SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    levels->setRange (1, 50);
    levels->setValue ((int) _config->number ("Harmonizer", "Levels1"));
    new FXHorizontalSeparator (cont, SEPARATOR_RIDGE | LAYOUT_FILL_X);
    FXHorizontalFrame* closeBox = new FXHorizontalFrame (cont,
    	LAYOUT_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH);
    new FXButton (closeBox, _stringTable->get ("OKBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_ACCEPT,
    	BUTTON_INITIAL | BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED
     	| FRAME_THICK, 0,0,0,0, 20,20);
    new FXButton (closeBox, _stringTable->get ("CANCELBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_CANCEL,
    	BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED | FRAME_THICK,
     	0,0,0,0, 20,20);

	if (data.execute (PLACEMENT_OWNER)) {
      	int lev = levels->getValue ();

        getApp ()->_harmon->mute (true);
//        delete [] getApp ()->_autoTunes;
        delete getApp ()->_autoTune;

    	_config->number ("Harmonizer", "Levels1", (int) lev);

//     	getApp ()->_pdets.clear ();
//    	for (int i = 0; i < PARTIALS; i++) {
         	getApp ()->_autoTune = new AdaptiveTuner (
         		getApp ()->_info, _config->number ("Harmonizer", "RangeStart1"),
           		_config->number ("Harmonizer", "RangeEnd1"),
             	(int) _config->number ("Harmonizer", "Levels1"),
              	_config->number ("Harmonizer", "AmpThreshold1"));
          	getApp ()->_autoTune->filterQuality (
          		_config->number ("Harmonizer", "FilterQuality1"));
          	getApp ()->_autoTune->smoothingQuality (
          		_config->number ("Harmonizer", "SmoothingQuality1"));
          	getApp ()->_autoTune->smoothingRate (
          		_config->number ("Harmonizer", "SmoothingRate1"));
//       		getApp ()->_pdets.push_back (getApp ()->_autoTunes[i]);
//		}

//        getApp ()->_harmon->pitchDetectors (getApp ()->_pdets);
		getApp ()->_pdet = getApp ()->_autoTune;
        getApp ()->_harmon->pitchDetector (getApp ()->_pdet);
      	getApp ()->_harmon->mute (false);

    	std::stringstream filStat;
		filStat << _stringTable->get ("FILTERLEVELS")
			<< ": " << _config->number ("Harmonizer", "Levels1");
    	_filterStatus1->setText (filStat.str ().c_str ());
 	}
 	return 1;
}

long MainWindow::onCmdSmoothing1 (FXObject* sender, FXSelector sel, void*) {
    FXDial* d = (FXDial*) sender;
    int t = d->getValue ();
    double r = (double) t;
    _config->number ("Harmonizer", "SmoothingRate1", r);
    getApp ()->_autoTune->smoothingRate (r);
    std::stringstream s;
    s << r;
    _smoothFieldATune->setText (s.str ().c_str ());
//    PRINT ("smooth 1")
    return 1;
}

long MainWindow::onCmdSmoothingText1 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingRate1", r);
    getApp ()->_autoTune->smoothingRate (r);
   _smoothDialATune->setValue ((int) (r));
   return 1;
}

// spectral

long MainWindow::onCmdRangeStartEnd2 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double v = (double) s->getValue ();
	std::stringstream t;
	t << v;
    switch (FXSELID (sel)) {
        case ID_RANGESTART2:
			_rangeStartField2->setText (t.str ().c_str ());
			_config->number ("Harmonizer", "RangeStart2", v);
			if (v > _config->number ("Harmonizer", "RangeEnd2")) {
				_config->number ("Harmonizer", "RangeEnd2", v);
    			_rangeEndSlider2->setValue (v);
       			_rangeEndField2->setText (t.str ().c_str ());
       		}
        break;
        case ID_RANGEEND2:
			_rangeEndField2->setText (t.str ().c_str ());
			_config->number ("Harmonizer", "RangeEnd2", v);
			if (v < _config->number ("Harmonizer", "RangeStart2")) {
				_config->number ("Harmonizer", "RangeStart2", v);
    			_rangeStartSlider2->setValue (v);
       			_rangeStartField2->setText (t.str ().c_str ());
       		}
        break;
  	}
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->range (_config->number ("Harmonizer",
     		"RangeStart2"),
     		_config->number ("Harmonizer", "RangeEnd2"));
// 	}
	return 1;
}

long MainWindow::onCmdRangeStartEndText2 (FXObject* sender,
	FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double v = atof (t.str ().c_str ());
    switch (FXSELID (sel)) {
        case ID_RANGESTARTTEXT2:
			_rangeStartSlider2->setValue (v);
			_config->number ("Harmonizer", "RangeStart2", v);
			if (v > _config->number ("Harmonizer", "RangeEnd2")) {
				_config->number ("Harmonizer", "RangeEnd2", v);
    			_rangeEndSlider2->setValue (v);
       			_rangeEndField2->setText (t.str ().c_str ());
       		}
        break;
        case ID_RANGEENDTEXT2:
			_rangeEndSlider1->setValue (v);
			_config->number ("Harmonizer", "RangeEnd2", v);
			if (v < _config->number ("Harmonizer", "RangeStart2")) {
				_config->number ("Harmonizer", "RangeStart2", v);
    			_rangeStartSlider2->setValue (v);
       			_rangeStartField2->setText (t.str ().c_str ());
       		}
        break;
  	}
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->range (_config->number ("Harmonizer",
     		"RangeStart2"),
     		_config->number ("Harmonizer", "RangeEnd2"));
// 	}
	return 1;
}

long MainWindow::onCmdQuality2 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "FilterQuality2", b);
   	std::stringstream t;
   	t << b;
   	_qualityField2->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdQualityText2 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_qualitySlider2->setValue (b);
   	_config->number ("Harmonizer", "FilterQuality2", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->filterQuality (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThreshold2 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Harmonizer", "AmpThreshold2", b);
   	std::stringstream t;
   	t << b;
   	_ampThreshField2->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->threshold (b);
// 	}
	return 1;
}

long MainWindow::onCmdAmpThresholdText2 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_ampThreshSlider2->setValue (b);
   	_config->number ("Harmonizer", "AmpThreshold2", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->threshold (b);
//	}
	return 1;
}

long MainWindow::onCmdSplit2 (FXObject* sender, FXSelector sel, void*) {
    FXSpinner* s = (FXSpinner*) sender;
	int b = s->getValue ();
   	_config->number ("Harmonizer", "Split2", (double) b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_spectral->split (b);
// 	}
	return 1;
}

long MainWindow::onCmdSmoothingQual2 (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* d = (FXRealSlider*) sender;
    double r = (double) d->getValue ();
    _config->number ("Harmonizer", "SmoothingQuality2", r);
    getApp ()->_spectral->smoothingQuality (r);
    std::stringstream s;
    s << r;
    _smoothQualFieldSpectral->setText (s.str ().c_str ());
//    PRINT ("smooth qual 1")
    return 1;
}

long MainWindow::onCmdSmoothingQualText2 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingQuality2", r);
    getApp ()->_spectral->smoothingQuality (r);
   _smoothQualSliderSpectral->setValue ((r));
   return 1;
}

long MainWindow::onCmdFFTSize (FXObject* sender, FXSelector sel, void*) {
	FXDialogBox data (this, _stringTable->get ("DATADLG").c_str (),
		DECOR_TITLE | DECOR_BORDER,
		0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame* cont = new FXVerticalFrame (&data,
 		LAYOUT_FILL_Y | LAYOUT_FILL_X,
		0,0,0,0, 0,0,0,0, 0,0);
	FXHorizontalFrame* firstFrame = new FXHorizontalFrame (cont,
		LAYOUT_FILL_X);
	FXGroupBox* levelsBox = new FXGroupBox (firstFrame,
		_stringTable->get ("FILTERLEVELS").c_str (),
  		FRAME_THICK | LAYOUT_FILL_X);
	FXHorizontalFrame* levelsFrame = new FXHorizontalFrame (levelsBox,
		LAYOUT_FILL_X);
    FXSpinner* levels =  new FXSpinner (levelsFrame, 0, NULL, 0,
    	SPIN_NORMAL |
    	FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
    levels->setRange (1, 50);
    levels->setValue ((int) _config->number ("Harmonizer", "Levels2"));
	FXGroupBox* envelopeBox = new FXGroupBox (cont,
 		_stringTable->get ("SIZEINBYTES").c_str (),
		LAYOUT_SIDE_TOP | LAYOUT_LEFT | FRAME_GROOVE | LAYOUT_FILL_X,
  		0,0,0,0);
    FXComboBox* envelopeCombo = new FXComboBox (envelopeBox, 0, NULL,
    	0,
    	COMBOBOX_STATIC | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP |
     	LAYOUT_FILL_X);

    envelopeCombo->setNumVisible (6);
	for (int i = 0; i < 6; i++) {
		std::stringstream temp;
		int w = (int) pow (2, i + 7);
		temp << w;
	    envelopeCombo->appendItem (temp.str ().c_str ());
	}
 	envelopeCombo->setCurrentItem ((int)
 		_config->number ("Harmonizer", "FFTSize"));
    new FXHorizontalSeparator (cont, SEPARATOR_RIDGE | LAYOUT_FILL_X);
    FXHorizontalFrame* closeBox = new FXHorizontalFrame (cont,
    	LAYOUT_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH);
    new FXButton (closeBox, _stringTable->get ("OKBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_ACCEPT,
    	BUTTON_INITIAL | BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED
     	| FRAME_THICK, 0,0,0,0, 20,20);
    new FXButton (closeBox, _stringTable->get ("CANCELBUTTON").c_str (), NULL,
    	&data, FXDialogBox::ID_CANCEL,
    	BUTTON_DEFAULT | LAYOUT_RIGHT | FRAME_RAISED | FRAME_THICK,
     	0,0,0,0, 20,20);

	if (data.execute (PLACEMENT_OWNER)) {
	    int it = envelopeCombo->getCurrentItem ();
      	int lev = levels->getValue ();

        getApp ()->_harmon->mute (true);
        delete getApp ()->_spectral;

  		_config->number ("Harmonizer", "FFTSize", (double) it);
     	_config->number ("Harmonizer", "Levels2", (int) lev);

 		int size = (int) pow (2, it + 7); // starting from 2^7 = 128

     	getApp ()->_spectral = new SpectralEstimator (
     		getApp ()->_info,
     		(int) size,
     		_config->number ("Harmonizer", "RangeStart2"),
       		_config->number ("Harmonizer", "RangeEnd2"),
       		(int) _config->number ("Harmonizer", "Levels2"),
         	_config->number ("Harmonizer", "Split2"),
          	_config->number ("Harmonizer", "AmpThreshold2"),
			_config->number ("Harmonizer", "SmoothingRate2"));
      	getApp ()->_spectral->smoothingQuality (
      		_config->number ("Harmonizer", "SmoothingQuality2"));

		getApp ()->_pdet = getApp ()->_spectral;
        getApp ()->_harmon->pitchDetector (getApp ()->_pdet);
      	getApp ()->_harmon->mute (false);

    	std::stringstream FFTStatus;
		FFTStatus << _stringTable->get ("FILTERLEVELS")
        		<< ": " << _config->number ("Harmonizer", "Levels2") << " - "
        		<< size << " bytes";
    	_FFTStatus->setText (FFTStatus.str ().c_str ());
 	}
 	return 1;
}

long MainWindow::onCmdSmoothing2 (FXObject* sender, FXSelector sel, void*) {
    FXDial* d = (FXDial*) sender;
    int t = d->getValue ();
    double r = (double) t;
    _config->number ("Harmonizer", "SmoothingRate2", r);
    getApp ()->_spectral->smoothingRate (r);
    std::stringstream s;
    s << r;
    _smoothFieldSpectral->setText (s.str ().c_str ());
//    PRINT ("smooth 1")
    return 1;
}

long MainWindow::onCmdSmoothingText2 (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    double r = atof (s.str ().c_str ());
    _config->number ("Harmonizer", "SmoothingRate2", r);
    getApp ()->_spectral->smoothingRate (r);
   _smoothDialSpectral->setValue ((int) (r));
   return 1;
}

// end pdet...


long MainWindow::onCmdMorphEnvelope (FXObject* sender, FXSelector sel, void*) {
    FXComboBox* c = (FXComboBox*) sender;
    int it = c->getCurrentItem ();
    switch (it) {
        case 0:
    		getApp ()->_morph->grainEnvelope (Pandora::RECTANGLE);
    	break;
        case 1:
    		getApp ()->_morph->grainEnvelope (Pandora::BARTLETT);
    	break;
        case 2:
    		getApp ()->_morph->grainEnvelope (Pandora::GAUSSIAN);
    	break;
        case 3:
    		getApp ()->_morph->grainEnvelope (Pandora::HAMMING);
    	break;
        case 4:
    		getApp ()->_morph->grainEnvelope (Pandora::HANNING);
    	break;
        case 5:
    		getApp ()->_morph->grainEnvelope (Pandora::BLACKMAN);
    	break;
        case 6:
    		getApp ()->_morph->grainEnvelope (Pandora::BLACKMAN_HARRIS);
    	break;
	}

    _config->number ("Morphizer", "WindowType", it);
    return 1;
}

long MainWindow::onCmdMorphAttackDecay (FXObject* sender, FXSelector sel, void*) {
    FXSlider* s = (FXSlider*) sender;
	double v = (double) s->getValue ();
    switch (FXSELID (sel)) {
        case ID_MORPHATTACK:
			_morphAttackProgress->setProgress ((int) v);
			_config->number ("Morphizer", "Attack", v);
        break;
        case ID_MORPHDECAY:
			_morphDecayProgress->setProgress ((int) v);
			_config->number ("Morphizer", "Decay", v);
        break;
  	}

	getApp ()->_morph->attackDecay (
		_config->number ("Morphizer", "Attack") / 100,
		_config->number ("Morphizer", "Decay") / 100);
	return 1;
}

long MainWindow::onCmdMorphSlider (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

	double fn = getApp ()->_table->naturalFreq (getApp ()->_info->samplingRate ());
    switch (FXSELID (sel)) {
    	case ID_MORPH1DET:
			_config->number ("Morphizer", "Center1", c);
       		_morphDetParamValues[0]->setText (v.str ().c_str ());
       		getApp ()->_morph->density (
       			_config->number ("Morphizer", "Center1"),
				_config->number ("Morphizer", "Random1"));
	    break;
	    case ID_MORPH1RAND:
			_config->number ("Morphizer", "Random1", c);
       		_morphRandParamValues[0]->setText (v.str ().c_str ());
       		getApp ()->_morph->density (
       			_config->number ("Morphizer", "Center1"),
				_config->number ("Morphizer", "Random1"));
        break;
    	case ID_MORPH2DET:
			_config->number ("Morphizer", "Center2", c);
       		_morphDetParamValues[1]->setText (v.str ().c_str ());
       		getApp ()->_morph->grainDuration (
       			_config->number ("Morphizer", "Center2"),
				_config->number ("Morphizer", "Random3"));
	    break;
	    case ID_MORPH2RAND:
			_config->number ("Morphizer", "Random2", c);
       		_morphRandParamValues[1]->setText (v.str ().c_str ());
       		getApp ()->_morph->grainDuration (
       			_config->number ("Morphizer", "Center2"),
				_config->number ("Morphizer", "Random3"));
        break;
    	case ID_MORPH3DET:
			_config->number ("Morphizer", "Center3", c);
       		_morphDetParamValues[2]->setText (v.str ().c_str ());
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
	    break;
	    case ID_MORPH3RAND:
			_config->number ("Morphizer", "Random3", c);
       		_morphRandParamValues[2]->setText (v.str ().c_str ());
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
        break;
    	case ID_MORPH4DET:
			_config->number ("Morphizer", "Center4", c);
       		_morphDetParamValues[3]->setText (v.str ().c_str ());
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
	    break;
	    case ID_MORPH4RAND:
			_config->number ("Morphizer", "Random4", c);
       		_morphRandParamValues[3]->setText (v.str ().c_str ());
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
        break;
	}

	return 1;
}

long MainWindow::onCmdMorphSliderText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s= (FXTextField*) sender;
   	std::stringstream v;
   	v << s->getText ().text ();
   	double c = atof (v.str ().c_str ());

	double fn = getApp ()->_table->naturalFreq (getApp ()->_info->samplingRate ());
    switch (FXSELID (sel)) {
    	case ID_MORPH1DETTEXT:
			_config->number ("Morphizer", "Center1", c);
       		_morphDetParamSliders[0]->setValue (c);
       		getApp ()->_morph->density (
       			_config->number ("Morphizer", "Center1"),
				_config->number ("Morphizer", "Random1"));
	    break;
	    case ID_MORPH1RANDTEXT:
			_config->number ("Morphizer", "Random1", c);
       		_morphRandParamSliders[0]->setValue (c);
       		getApp ()->_morph->density (
       			_config->number ("Morphizer", "Center1"),
				_config->number ("Morphizer", "Random1"));
        break;
    	case ID_MORPH2DETTEXT:
			_config->number ("Morphizer", "Center2", c);
       		_morphDetParamSliders[1]->setValue (c);
       		getApp ()->_morph->grainDuration (
       			_config->number ("Morphizer", "Center2"),
				_config->number ("Morphizer", "Random3"));
	    break;
	    case ID_MORPH2RANDTEXT:
			_config->number ("Morphizer", "Random2", c);
       		_morphRandParamSliders[1]->setValue (c);
       		getApp ()->_morph->grainDuration (
       			_config->number ("Morphizer", "Center2"),
				_config->number ("Morphizer", "Random3"));
        break;
    	case ID_MORPH3DETTEXT:
			_config->number ("Morphizer", "Center3", c);
       		_morphDetParamSliders[2]->setValue (c);
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
	    break;
	    case ID_MORPH3RANDTEXT:
			_config->number ("Morphizer", "Random3", c);
       		_morphRandParamSliders[2]->setValue (c);
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
        break;
    	case ID_MORPH4DETTEXT:
			_config->number ("Morphizer", "Center4", c);
       		_morphDetParamSliders[3]->setValue (c);
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
	    break;
	    case ID_MORPH4RANDTEXT:
			_config->number ("Morphizer", "Random4", c);
       		_morphRandParamSliders[3]->setValue (c);
       		getApp ()->_morph->frequency (
       			fn * _config->number ("Morphizer", "Center3"),
				_config->number ("Morphizer", "Random3"),
				fn * _config->number ("Morphizer", "Center4"),
				_config->number ("Morphizer", "Random4"));
        break;
	}
	return 1;
}

long MainWindow::onCmdPhJitter (FXObject* sender, FXSelector sel, void*) {
    FXDial* d = (FXDial*) sender;
    int t = d->getValue ();

    _config->number ("Morphizer", "PhaseJitter", (double) t);
    getApp ()->_morph->speedFactor (
    	_config->number ("Morphizer", "Speed"),  t);
    std::stringstream s;
    s << t;
    _phJitterField->setText (s.str ().c_str ());
    return 1;
}

long MainWindow::onCmdPhJitterText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* t = (FXTextField*) sender;
    std::stringstream s;
    s << t->getText ().text ();
    int r = atoi (s.str ().c_str ());
    _config->number ("Morphizer", "PhaseJitter", (double) r);
    getApp ()->_morph->speedFactor (
		_config->number ("Morphizer", "Speed"), r);
   _phJitterDial->setValue (r);
   return 1;
}

long MainWindow::onCmdWidth (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double v = (double) s->getValue ();
	_widthProgress->setProgress ((int) (v * 100));
	_config->number ("Morphizer", "SpatialJitter", v);
   	getApp ()->_morph->spatialPosition (v);
   	return 1;
}

long MainWindow::onCmdSpeedFactor (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s = (FXRealSlider*) sender;
	double b = s->getValue ();
   	_config->number ("Morphizer", "Speed", b);
   	std::stringstream t;
   	t << b;
   	_speedFactorField->setText (t.str ().c_str ());
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_morph->speedFactor (
    		_config->number ("Morphizer", "Speed"),
    		(long) _config->number ("Morphizer", "PhaseJitter"));
// 	}
	return 1;
}

long MainWindow::onCmdSpeedFactorText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
	std::stringstream t;
	t << s->getText ().text ();
	double b = atof (t.str ().c_str ());
 	_speedFactorSlider->setValue (b);
   	_config->number ("Morphizer", "Speed", b);
//	for (int i = 0; i < PARTIALS; i++) {
    	getApp ()->_morph->speedFactor (
    		_config->number ("Morphizer", "Speed"),
    		(long) _config->number ("Morphizer", "PhaseJitter"));
// 	}
	return 1;
}

long MainWindow::onCmdReverseGrain (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Morphizer", "ReverseGrain", (double) b);
   	if (b == 0) {
		getApp ()->_morph->grainDirection (1);
   	}
   	else {
	   	getApp ()->_morph->grainDirection (-1);
   	}
	return 1;
}

long MainWindow::onCmdGrainlet (FXObject* sender, FXSelector sel, void*) {
    FXCheckButton* s = (FXCheckButton*) sender;
	bool b = s->getCheck ();

   	_config->number ("Morphizer", "Grainlet", (double) b);
	if ((int) _config->number ("Morphizer", "Grainlet") == 1) {
		getApp ()->_morph->generation (Pandora::GRAINLET);
		_morphDetParamValues[1]->disable ();
		_morphDetParamSliders[1]->disable ();
		_morphRandParamValues[1]->disable ();
		_morphRandParamSliders[1]->disable ();
	}
	else {
		getApp ()->_morph->generation (Pandora::ASYNC);
		_morphDetParamValues[1]->enable ();
		_morphDetParamSliders[1]->enable ();
		_morphRandParamValues[1]->enable ();
		_morphRandParamSliders[1]->enable ();
	}
	return 1;
}

// mix

long MainWindow::onCmdReverb (FXObject* sender, FXSelector sel, void*) {
    FXRealSlider* s= (FXRealSlider*) sender;
   	double c = s->getValue ();
   	std::stringstream v;
   	v << c;

 	_config->number ("Harmonizer", "T60", c);
	getApp ()->_reverbLeft->setT60 (c);
	getApp ()->_reverbRight->setT60 (c);
	_reverbField->setText (v.str ().c_str ());
	return 1;
}

long MainWindow::onCmdReverbText (FXObject* sender, FXSelector sel, void*) {
    FXTextField* s = (FXTextField*) sender;
   	double c = atof (s->getText ().text ());

 	_config->number ("Harmonizer", "T60", c);
	getApp ()->_reverbLeft->setT60 (c);
	getApp ()->_reverbRight->setT60 (c);
	_reverbSlider->setValue (c);
	return 1;
}

long MainWindow::onCmdMixer (FXObject* sender, FXSelector sel, void*) {
    FXDial* s = (FXDial*) sender;
	double v = (double) s->getValue ();

    switch (FXSELID (sel)) {
        case ID_LEVEL1:
        	_config->number ("Harmonizer", "SourceLevel", (double) v / 100);
         	_sourceProgress->setProgress ((int) v);
         	getApp ()->_harmon->sourceLevel (v / 100);
        break;
        case ID_LEVEL2:
        	_config->number ("Harmonizer", "HarmonyLevel", (double) v / 100);
         	_harmonyProgress->setProgress ((int) v);
         	getApp ()->_harmon->harmonyLevel (v / 100);
        break;
        case ID_LEVEL3:
        	_config->number ("Morphizer", "OutputLevel", (double) v / 100);
         	_morphProgress->setProgress ((int) v);
         	getApp ()->_morphizerLevel = v / 100;
        break;
        case ID_LEVEL4:
        	_config->number ("Harmonizer", "EffectsLevel", (double) v / 100);
         	_effectsProgress->setProgress ((int) v);
			getApp ()->_reverbLeft->setEffectMix (v / 100);
			getApp ()->_reverbRight->setEffectMix (v / 100);
		break;
  	}
  	return 1;
}

// EOF
