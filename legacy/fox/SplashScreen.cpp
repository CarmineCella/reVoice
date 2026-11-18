// SplashScreen.cpp
//

#include "SplashScreen.h"

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

const int SPLASH_TIMEOUT = 3000;

FXDEFMAP (SplashScreen) SplashScreenMap[] = {
    FXMAPFUNC (SEL_LEFTBUTTONPRESS, SplashScreen::ID_ACCEPT, SplashScreen::onCmdAccept),
    FXMAPFUNC (SEL_TIMEOUT,  	    SplashScreen::ID_ACCEPT, SplashScreen::onCmdAccept),
	FXMAPFUNC (SEL_COMMAND, 	    SplashScreen::ID_ACCEPT, SplashScreen::onCmdAccept),
 	FXMAPFUNC (SEL_KEYPRESS,	    0,          		     SplashScreen::onCmdAccept),
};

FXIMPLEMENT (SplashScreen, FXDialogBox, SplashScreenMap,
	ARRAYNUMBER (SplashScreenMap))

SplashScreen::SplashScreen (MainWindow* owner) : FXDialogBox (owner, "",
	LAYOUT_CENTER_X | LAYOUT_CENTER_Y | DECOR_BORDER, 0,0,0,0, 0,0,0,0, 0,0) {
	_config = Configure::instance ();
	_stringTable = StringTable::instance ();

	FXHorizontalFrame* cont = new FXHorizontalFrame (this, LAYOUT_SIDE_RIGHT |
		LAYOUT_FILL,
		0,0,0,0, 0,0,0,0, 0,0);
	new FXButton (cont, FXString::null, owner->getApp ()->_splashIcon, this,
 		SplashScreen::ID_ACCEPT, FRAME_THICK | LAYOUT_FILL,
		0,0,0,0, 0,0,0,0);
	FXVerticalFrame* info = new FXVerticalFrame (cont, LAYOUT_RIGHT |
		LAYOUT_FILL,
		0,0,0,0, 10,10,10,10, 0,0);
	FXLabel* title = new FXLabel (info, _stringTable->get ("EDITION").c_str (), 0,
		JUSTIFY_RIGHT | LAYOUT_FILL_X);
	_font = new FXFont (getApp (), "Verdana", 16, FXFont::ExtraBold);
	title->setFont (_font);
	new FXHorizontalSeparator (info, SEPARATOR_GROOVE | LAYOUT_FILL_X);
	new FXLabel (info, _stringTable->get ("AUTHOR").c_str (), 0,
		JUSTIFY_RIGHT | LAYOUT_FILL);
	FXHorizontalFrame* cont2 = new FXHorizontalFrame (info, LAYOUT_FILL,
		0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel (cont2, FXString::null, owner->getApp ()->_nerdIcon, LAYOUT_CENTER_X |
 		LAYOUT_BOTTOM,
		0,0,0,0, 0,0,0,0);
	new FXLabel (info, _stringTable->get ("TECHINFO").c_str (),
		0, JUSTIFY_RIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_BOTTOM);
	owner->getApp ()->addTimeout ((FXObject*) this,
		SplashScreen::ID_ACCEPT, SPLASH_TIMEOUT);
}

SplashScreen::~SplashScreen () {
	getApp ()->removeTimeout ((FXObject*) this, SplashScreen::ID_ACCEPT);
	delete _font;
}

long SplashScreen::onCmdAccept (FXObject* sender, FXSelector, void*) {
    handle (this, FXSEL (SEL_CLOSE, NULL), 0);
	return 1;
}

// EOF
