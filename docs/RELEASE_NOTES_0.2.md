# reVoice 0.2.0

Restored standalone real-time vocal harmonizer for macOS Apple Silicon and
Intel, Windows x64, and Linux x86_64. Based on Carmine Emanuele Cella's original
2004 application, with the Studio black interface and a working vocal signal path.

- Dependency-free C++ YIN pitch detection shared by all four harmony voices,
  including quiet-input tracking, an adjustable RMS gate and live pitch history.
- Four-voice granular harmonization, chord/scale modes, formant control, vibrato,
  granular morphing and stereo reverb.
- Graphite/mint Studio layout with four voice cards, a common mixer, advanced
  editing pages, and presets. Harmony defaults to 0 dB; saved older startup
  levels are updated once. A menu command restores an audible harmony/mix balance.
- Input/output device and microphone/line-channel selection. Start audio always
  uses the displayed selections. WAV/test-tone sources and background WAV recording.
- MIDI fundamental and four-note chord control, optional MIDI thru, and all-notes-off.
- CMake builds, four native CI/package targets, address/undefined/leak checking,
  thirteen regression suites and native UI smoke checks. Output tests measure
  each shifted voice with the original muted, including quiet and harmonic-rich
  input and a microphone on the second interface channel.
- Linux header and Windows standard-C++ build fixes in the original Pandora library.

YIN is the default detector; Spectral is retained as a legacy option.
AdaptiveTuner is omitted because its implementation is unavailable. Historical
FOX source remains in `legacy/fox`, excluded from builds. The application is
BSD 2-Clause; Pandora retains LGPL 2.1 and dependency notices are bundled.

macOS targets version 11 or newer. Linux targets Ubuntu 22.04 or compatible
newer distributions with X11/XWayland. Packages are unsigned. Physical MIDI,
driver/device combinations and listening quality require validation on the target
system; the user has confirmed working harmonization on macOS.

On macOS copy `reVoice.app` from the DMG to Applications. On Windows extract the
ZIP and run `reVoice.exe`. On Linux extract the archive and run `bin/reVoice`.
Use Live input with the correct input device/channel, and use headphones.

Plug-ins are a separate development phase following this standalone release.
