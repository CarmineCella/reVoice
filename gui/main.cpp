// main.cpp
//

// TODO:	1. Stringa "Ready." della status bar
//			2. Menu popup

#ifdef __WINDOWS__
	#include <windows.h>
#endif

#include "MyApp.h"
#include <iostream> // iostream must be included *BEFORE* fx.h
#include "Configure.h"
#include "StringTable.h"
#include "MainWindow.h"
#include <fx.h>
#include <signal.h>
#include <stdexcept>

//#if !defined (OPTIMIZED)
//	#include "memCheck.h"
//#endif

using namespace std;

MainWindow* w;

void startup (MyApp& app, int argc, char* argv[]) {
   	app.init (argc, argv);
	
	w = new MainWindow (&app);
	#ifndef _WIN32
		app.addSignal (SIGQUIT, w, MainWindow::ID_FILEQUIT);
		app.addSignal (SIGHUP, w, MainWindow::ID_FILEQUIT);
		app.addSignal (SIGPIPE, w, MainWindow::ID_FILEQUIT);
	#endif
	app.addSignal (SIGINT, w, MainWindow::ID_FILEQUIT);

	new FXToolTip (&app, 0);

	app.create ();
	
	w->show (PLACEMENT_DEFAULT);
	w->setFocus ();
}

int main (int argc, char* argv[]) {
    Configure* config = Configure::instance ();
#if defined (REPITCH_ONLY)
	config->program ("rePitch");
#else
	config->program ("reVoice");
#endif
	config->vendor ("Viscount");

  	MyApp app (config->program ().c_str (),
		config->vendor ().c_str ());
	try {
		startup (app, argc, argv);

    	// Startup...
	   	if ((int) config->number ("Windows", "ShowSplashScreen") == 1) {
			w->handle (&app, FXSEL (SEL_COMMAND, MainWindow::ID_SPLASHSCREEN),
				NULL);
    	}
    	if ((int) config->number ("Devices", "ConnectEngineOnStartup") == 1) {
    		w->handle (&app, FXSEL (SEL_COMMAND, MainWindow::ID_RUNPLAY), NULL);
    	}
    	if ((int) config->number ("Devices", "EnableMIDIOnStartup") == 1) {
    		w->handle (&app, FXSEL (SEL_COMMAND, MainWindow::ID_RUNENABLE), NULL);
    	}

		return app.run ();
	}
	catch (runtime_error& e) {
		#ifdef __WINDOWS__
		    ::MessageBox (NULL, e.what (), "reVoice runtime error",
		    	MB_OK | MB_ICONSTOP);
		#else
			std::cout << "Runtime error: " << e.what () << std::endl;
		#endif
	}
	catch (exception& e) {
		#ifdef __WINDOWS__
		    ::MessageBox (NULL, e.what (), "reVoice unhandled exception",
		    	MB_OK | MB_ICONSTOP);
		#else
			std::cout << "Unhandled exception: " << e.what () << std::endl;
		#endif
	}
	catch (...) {
		#ifdef __WINDOWS__
		    ::MessageBox (NULL, "undefined excpetion", "reVoice fatal error",
		    	MB_OK | MB_ICONSTOP);
		#else
			std::cout << "Fatal error: undefined exception" << std::endl;
		#endif
	}
	return 0;
}

// EOF

