// SplashScreen.h
//

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "MainWindow.h"
#include "Configure.h"
#include "StringTable.h"
#include <fx.h>

class SplashScreen : public FXDialogBox {
	FXDECLARE (SplashScreen)

	Configure* _config;
	StringTable* _stringTable;
public:
   	enum {
   	    ID_ACCEPT = FXMainWindow::ID_LAST,
   	    ID_LAST
	};
	
	SplashScreen (MainWindow* owner);
	~SplashScreen ();
	
	long onCmdAccept (FXObject*, FXSelector, void*);
private:
    FXTimer* _timer;
    FXFont* _font;
	SplashScreen () {}
};

#endif	// SPLASHSCREEN_H

// EOF


