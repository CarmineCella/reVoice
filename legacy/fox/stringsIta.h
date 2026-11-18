// stringsIta.h - String table for the italian language
//

#ifndef STRINGSITA_H
#define STRINGSITA_H

// TODO: Customize program title...
_table["STRING_NOT_FOUND"] = "Stringa non presente";
#if defined (REPITCH_ONLY)
	_table["TITLE"] = "rePitch, ver. 1.00";
	_table["AUTHOR"] = "Build 0.0.1, novembre 2004";
#else
	_table["TITLE"] = "reVoice, ver. 1.10";
	_table["AUTHOR"] = "Build 1.0.3, maggio 2007";
#endif
_table["EDITION"] = "Development edition";
_table["TECHINFO"] = "\nQuesto software utilizza il toolkit grafico FOX\n(www.fox-toolkit.org), la libreria Portaudio\nwww.portaudio.com)ed il \nPandora Audio Framework,\nscritto dall'autore.\nE' stato scritto utilizzando\nsolamente strumenti open source.";
// TODO: Customize program credits...
_table["CREDITS"] = "Si ringrazia...";
_table["STATUSTAB"] = "Stato\tMostra lo stato\tMostra informazioni sullo stato del sistema.";
_table["MIDIMONTAB"] = "MIDI monitor\tMostra l'input MIDI\tMostra i messaggi MIDI in input.";
_table["INFOTAB"] = "Info\tMostra informazioni\tMostra altre informazioni.";
_table["DEVICES"] = "Dispositivi";
_table["LEVELS"] = "Livelli";
_table["CPULOAD"] = "Carico CPU";
_table["MIDI"] = "MIDI: ";
_table["AUDIO"] = "Audio: ";
_table["RECORDING"] = "Registrazione...";
_table["LATENCY"] = "Latenza (msec): ";
_table["NOT_IMPLEMENTED"] = "Funzione non implementata per questo sistema";
_table["OPENFILE"] = "Selezionare i file da aprire...";
_table["ENGINEERROR"] = "Errore del motore audio";
_table["RECORDERROR"] = "Errore nella registrazione audio";
_table["MIDIERROR"] = "Errore MIDI";
_table["FILEERROR"] = "Errore nelle operazioni di scrittura/lettura su file";
_table["LISTSAVED"] = "La lista dei messaggi e' stata salvata";
_table["SETTINGSDLG"] = "Impostazioni..."; 
_table["GLOBALTAB"] = "Impostazioni globali";
_table["AUDIOTAB"] = "Impostazioni audio";
_table["AUDIOINPUTTAB"] = "Registrazione";
_table["AUDIOOUTPUTTAB"] = "Riproduzione";
_table["MIDITAB"] = "Impostazioni MIDI";
_table["MIDIINPUTTAB"] = "Input";
_table["MIDIOUTPUTTAB"] = "Output";
_table["PREFERENCES"] = "Preferenze";
_table["MAXCPULOAD"] = "Massimo carico della CPU";
_table["SAMPLINGRATE"] = "Tasso di campionamento";
_table["ENGINEFRAMES"] = "Frames di generazione";
_table["INPUTCHANNELS"] = "Canali in ingresso";
_table["OUTPUTPCM"] = "File PCM d'uscita";
_table["ENGINEONSTARTUP"] = "Attiva la generazione audio all'avvio";
_table["MIDIINPUT"] = "Abilita input MIDI";
_table["MIDITHRU"] = "Abilita MIDI thru";
_table["ACTIVESENSING"] = "Filtra active sensing (0xFE)";
_table["SYSEX"] = "Filtra system exclusives (0xF0)";
_table["REALTIMEMSG"] = "Filtra messaggi di sincronizzazione (0xF8 solo)";
_table["MIDIONSTARTUP"] = "Abilita MIDI all'avvio";
// TODO: Customize the help information...
#if defined (REPITCH_ONLY) 
	_table["HELP"] = "Non implementato...";
#else
	_table["HELP"] = "Non implementato...";
#endif

_table["HELPDLG"] = "Guida...";
// TODO: Customize web page and mail address...
_table["WEBPAGE"] = "www.cryptosound.org";
#if defined (REPITCH_ONLY)
	_table["MAILCOMMAND"] = "mailto:sarabellissima@libero.it?Subject:Program report";
#else
	_table["MAILCOMMAND"] = "mailto:cecily@tele2.it?Subject:Program report";
#endif
_table["ABOUTDLG"] = "Informazioni...";  
  
// menu items...
_table["FILEMENU"] = "&File";
_table["FILEOPENMENU"] = "&Apri...\tCtrl-O\tApre un file.";
_table["FILEDUMPMENU"] = "&Salva configurazione\tCtrl-D\tSalva la configurazione corrente su file.";
_table["FILEQUITMENU"] = "&Esci\t\tChiude il programma.";
_table["FILERESETQUITMENU"] = "A&zzera e esci\t\tChiude il programma azzerando la configurazione attuale.";
_table["RUNMENU"] = "&Esegui";
_table["RUNPLAYMENU"] = "&Avvia/ferma la generazione audio\tF5\tAvvia o arresta il motore di generazione dell'audio.";
_table["RUNRECORDMENU"] = "&Registra/ferma registrazione\t\tAvvia o arresta la registrazione audio.";
_table["RUNENABLEMENU"] = "Abilita/&disabilita il MIDI\tF6\tAbilita o disabilita lo streaming sui device MIDI.";
_table["VIEWMENU"] = "&Visualizza";
_table["VIEWTOOLBARMENU"] = "&Barra degli strumenti\t\tNasconde o mostra la barra.";
_table["VIEWSTATUSBARMENU"] = "Barra di &stato\t\tNasconde o mostra la barra.";
_table["VIEWCONTROLPANELMENU"] = "&Pannello di controllo\t\tNasconde o mostra il pannello.";
_table["TOOLSMENU"] = "&Strumenti";
_table["TOOLSMIDIMONMENU"] = "MIDI monitor";
_table["TOOLSMIDIMONSAVEMENU"] = "&Salva la lista\t\tSalva su file la lista dei messaggi del MIDI monitor.";
_table["TOOLSMIDIMONCLEARMENU"] = "&Pulisci\tCtrl-M\tElimina la lista dei messaggi nel MIDI monitor.";
#ifdef _WIN32
	_table["TOOLSSNDVOLMENU"] = "&Configurazione audio...\t\tModifica la configurazione audio di sistema.";
#endif
_table["TOOLSSETTINGSMENU"] = "&Impostazioni...\t\tModifica la configurazione del programma.";
_table["HELPMENU"] = "&?";
_table["HELPCONTENTSMENU"] = "&Guida...\tF1\tMostra i contenuti della guida.";
_table["HELPWEBMENU"] = "&Sito internet...\t\tSi connette al sito internet del programma.";
_table["HELPMAILMENU"] = "&Spedisci un commento...\t\tManda una e-mail agli autori del programma.";
_table["HELPABOUTMENU"] = "&Informazioni...\t\tMostra le informazioni sul programma.";

// buttons...       
_table["OKBUTTON"] = "OK\tAccetta le modifiche\tAccetta le modifiche effettuate";
_table["CANCELBUTTON"] = "Annulla\tAnnulla le modifiche\tAnnulla le modifiche effettuate";
_table["BROWSEBUTTON"] = "Sfoglia\tSfoglia le cartelle\tSfoglia le cartelle per trovare files";
_table["FILEOPENBUTTON"] = "\tApri\tApre un file";
_table["PLAYSTOPBUTTON"] = "\tAvvia/ferma\tAvvia o arresta il motore di generazione dell'audio";
_table["RECSTOPBUTTON"] = "\tRegistra/ferma reg.\tAvvia o arresta la registrazione audio";
_table["ENABLEDISABLEBUTTON"] = "\tAbilita/disabilit\tAbilita o disabilita lo streaming sui device MIDI";
_table["TOOLSSETTINGSBUTTON"] = "\tImpostazioni\tModifica la configurazione del programma";
_table["HELPCONTENTSBUTTON"] = "\tGuida\tMostra i contenuti della guida";
_table["HELPABOUTBUTTON"] = 
	"\tInformazioni\tMostra informazioni sul programma";
_table["GLOBALTABBUTTON"] = 
	"Globali\tImpostazioni globali\tModifica la configurazione del programma";
_table["AUDIOTABBUTTON"] = 
	"Audio\tImpostazioni audio\tModifica la configurazione audio";
_table["MIDITABBUTTON"] = 
	"MIDI\tImpostazioni MIDI\tModifica la configurazione MIDI";
_table["SHOWSPLASHBUTTON"] = "Mostra il logo all'avvio";
		
// reVoice specific...
_table["HELPPROTONHELPMENU"] = "Guida di &Pandora...\t\tMostra la guida del Pandora audio framework";
#if defined (REPITCH_ONLY)
    _table["HARMONIZER"] = "Pitch shifter";
    _table["HARMONIZERCR"] = "Shifter";
	_table["PITCHDETECTOR"] = "Pitch detector";
#else
	_table["HARMONIZER"] = "Harmonizer";
	_table["HARMONIZERCR"] = "Harmon";
	_table["PITCHDETECTOR"] = "Pitch detectors";
#endif

_table["PITCHDETECTOR"] = "Pitch detectors";
_table["PITCHDETECTORCR"] = "P.D.";
_table["MIX"] = "Mixer ed effetti";
_table["MIXCR"] = "Mix";
_table["VOICES"] = "Configurazione voci";
_table["VOICESCR"] = "Voci";
_table["WAVEFORM"] = "Forma d'onda";
_table["SINE"] = "Seno (440 Hz)";
_table["SQUARE"] = "Onda quadra (440 Hz)";
_table["FROMFILE"] = "File audio";
_table["LIVEINPUT"] = "Live input";
_table["INPUTPCM"] = "File d'ingresso";
_table["PRESET"] = "Preset";
_table["CUSTOM"] = "Personalizzato";
_table["DOMINANTSEVENTH"] = "Settima dominante";
_table["MINORSEVENTH"] = "Settima minore";
_table["MAJORSEVENTH"] = "Settima maggiore";
_table["DIMINISHEDSEVENTH"] = "Settima diminuita";
_table["AUGMENTEDSEVENTH"] = "Settima aumentata";
_table["MINORWITHSIXTH"] = "Minore con sesta";
_table["QUARTALCHORD"] = "Accordo per quarte";
_table["SMALLVIBRATO"] = "Vibrato lieve";
_table["MEDIUMVIBRATO"] = "Vibrato medio";
_table["LARGEVIBRATO"] = "Vibrato ampio";
_table["SMALLRANDVIBRATO"] = "Vib. random lieve";
_table["MEDIUMRANDVIBRATO"] = "Vib. random medio";
_table["LARGERANDVIBRATO"] = "Vib. random ampio";
_table["TWOTWO"] = "Due - due";
_table["ONETHREE"] = "Una - tre";
_table["THREEONE"] = "Tre - una";
_table["MIXED"] = "Coro misto";
_table["FEMALE"] = "Coro femminile";
_table["MALE"] = "Coro maschile";
_table["FEMALELABEL"] = "Donna";
_table["MALELABEL"] = "Uomo";
_table["FUNDAMENTAL"] = "Fondamentale";  
_table["FOLLOWMIDI"] = "Nota MIDI (can. 0)";
_table["TRANSPOSE"] = "Trasporta";
_table["MANUAL"] = "Rapporto";
_table["SEMITONES"] = "semitoni";
_table["DEFAULTBUTTON"] = "Default\tImposta la configurazione di default";
_table["RANDOMBUTTON"] = "Random\tRandomizza le parziali";
_table["ENVELOPE"] = "Inviluppo";
_table["RECTANGLE"] = "Rettangolare";
_table["BARTLETT"] = "Bartlett";
_table["GAUSSIAN"] = "Gaussiana";
_table["HAMMING"] = "Hamming";
_table["HANNING"] = "Hanning";
_table["BLACKMAN"] = "Blackman";
_table["BLACKMANHARRIS"] = "Blackman-Harris";
_table["ATTACKDECAY"] = "Attack / decay";
_table["AUTOMODE"] = "Cicli automatici";
_table["CYCLES"] = "Cicli per grano";
_table["FREQUENCYTHRESHOLD"] = "Confidenza frequenziale";
_table["STUNISON"] = "Unisono";
_table["STMINORSECOND"] = "Seconda minore";
_table["STMAJORSECOND"] = "Seconda maggiore";
_table["STMINORTHIRD"] = "Terza minore";
_table["STMAJORTHIRD"] = "Terza maggiore";
_table["STPERFECTFOURTH"] = "Quarta giusta";
_table["STTRITONE"] = "Tritono";
_table["STPERFECTFIFTH"] = "Quinta giusta";
_table["STMINORSIXTH"] = "Sesta minore";
_table["STMAJORSIXTH"] = "Sesta maggiore";
_table["STMINORSEVENTH"] = "Settima minore";
_table["STMAJORSEVENTH"] = "Settima maggiore";
_table["SCALIC"] = "Su scala";
_table["MIDISTREAM"] = "Stream MIDI (Ch. 1)";
_table["INTELLICHORD"] = "IntelliChord";
_table["SINGLECHORD"] = "SingleChord";
_table["STOCTAVE"] = "Ottava giusta";
_table["STSMOOTH"] = "Continuo";
_table["MASTERKEY"] = "Tono d'impianto";
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
_table["SCALE"] = "Modo";
_table["MAJOR1"] = "Maggiore 1";
_table["MAJOR2"] = "Maggiore 2";
_table["MINOR1"] = "Minore 1";
_table["MINOR2"] = "Minore 2";
_table["MINOR3"] = "Minore 3";
_table["BLUES1"] = "Blues 1";
_table["CHROMA_MAJOR1"] = "Cromatica magg. 1";
_table["OCTOPHONIC1"] = "Ottofonica 1";
_table["JITTER"] = "Posizione sorgente";
_table["PDET"] = "Pitch detector";
_table["PERIODESTIMATOR"] = "Rilevatore di periodo";
_table["AUTOTUNER"] = "Filtro autoaccordante";
_table["SPECTRALESTIMATOR"] = "Stima spettrale";
_table["RANGESTART"] = "Inizio intervallo (Hz)";
_table["QUALITY"] = "Q dei filtri";
_table["SPLIT"] = "Split";
_table["FEEDBACK"] = "Feedback";
_table["AMPLITUDETHRESHOLD"] = "Soglia d'ampiezza";
_table["ZCCYCLES"] = "Cicli zero-crossing";
_table["CORRECTPITCH"] = "Correggi pitch";
_table["FILTERS"] = "Filtri";
_table["FILTERSBUTTON"] = "Cambia\tModifica i filtri";
_table["FILTERLEVELS"] = "Livelli (x 12 dB/ott.)";
_table["FFTSIZE"] = "Filtri e dimensione FFT";
_table["FREQDELTA"] = "Frequenza min / max (Hz)";
_table["RANGESTARTEND"] = "Inizio / fine intervallo (Hz)";
_table["SPEED"] = "Velocita'";
_table["OVERLAP"] = "Overlap";
_table["FREQUENCY"] = "Freq. rilevata (Hz)";
_table["NOTENAME"] = "Nota";
_table["SCALETONE"] = "Grado della scala";
_table["DATADLG"] = "Configurazione filtri";
_table["FFTDATADLG"] = "Filtri e FFT";
_table["SIZEINBYTES"] = "Campioni FFT";
_table["USEBIQUAD"] = "Usa filtri biquad";
_table["RANGEERROR"] = "Errore di range; ripetere l'operazione";
_table["PANICBUTTON"] = "\tAzzera\tAzzera la tabella corrente";
_table["PHIRESETBUTTON"] = "\tFase 0\tAzzera la fase";
_table["SMOOTHING"] = "Tasso di approssimazione (Hz)";
_table["SMOOTHQUAL"] = "Q di approssimazione";
_table["GLOBAL"] = "Controlli";
_table["TRACKPITCH"] = "Registra frequenza";
_table["BYPASS"] = "Bypass";
_table["FREEZE"] = "Congela pitch";
_table["RUMBLE"] = "Anti-rimbombo";
_table["PREPROCESSING"] = "Pre-elaborazione";
_table["PARTIALS"] = "Rapporti";
_table["VIBRATO"] = "Vibrato";
_table["RANDVIBRATO"] = "Vib. random";
_table["PANNING"] = "Panning";
_table["FORMANTS"] = "Formanti";
_table["SETUP"] = "Setup";
_table["CONTROLS"] = "Controlli";
_table["ACTIVATION"] = "Attivazione";
_table["HIGHPASSVOICES"] = "Filtro highpass";
_table["VOICE"] = "Voce";
_table["POSITIONS"] = "Posizioni";
_table["GENDER"] = "Genere";
_table["SOURCE"] = "Sorgente";
_table["HARMONY"] = "Armonia";
_table["MORPHIZER"] = "Morphizer";
_table["EFFECTS"] = "Riverbero";
_table["REVERB"] = "Tempo di riverberazione (sec.)";
_table["DELAYLINE"] = "Delay tot.";
_table["DELAYUSED"] = "Delay usato";
_table["DENSITY"] = "Densita'";
_table["DURATION"] = "Durata";
_table["MORPHFREQUENCY"] = "Frequenza (rapp.)";
_table["GLISSATO"] = "Glissato (rapp.)";
_table["PHASEJITTER"] = "Jitter di fase (camp.)";
_table["SPATIALWIDTH"] = "Distribuzione";
_table["REVGRAIN"] = "Inverti grano";
_table["GRAINLET"] = "Grainlet";
_table["MORPHUSAGE"] = "Morph buffer usato: ";
_table["HARMONUSAGE"] = "Harmon buffer usato: ";

#ifdef OPTIMIZED
	_table["OPTIMIZED"] = "[Build ottimizzato 1.0.3]";
#else
	_table["DEBUG"] = "[Build 1.0.3]";	
#endif

_table["AREYOUSURE"] = "Sei sicuro?";

#endif	// STRINGSITA_H

// EOF

