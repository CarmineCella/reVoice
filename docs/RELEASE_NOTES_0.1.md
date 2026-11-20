First cross-platform rebuild of Carmine Emanuele Cella's original real-time
vocal harmonizer.

- Native FLTK application for macOS Apple Silicon and Intel, Windows x64, and Linux x86_64.
- Dependency-free C++ YIN pitch detection driving four-voice Pandora granular harmonization, formant control,
  vibrato, scale and chord modes, granular morphing, and STK NRev reverb.
- Live microphone, WAV, sine, and square sources; stereo WAV recording on a
  background writer; MIDI fundamental/chord control and optional MIDI thru.
- Selectable input/output devices and microphone/line input channels. Start
  audio uses the displayed selections. Restore harmony and mix from the menu
  when saved levels or bypass/MIDI settings hide the harmony.
- CMake builds with pinned dependency archives and a GitHub Actions build,
  test, sanitizer, package, and tag-release workflow.
- Regression fixes for window buffer bounds, frame copying and division,
  DSP ownership leaks, pitch detection, and parameter changes during audio.
- BSD 2-Clause application license. Pandora retains LGPL 2.1.

AdaptiveTuner is omitted because its implementation is no longer available.
YIN replaces the old Period detector as the default; Spectral remains a legacy
option. Pre-YIN settings migrate to YIN while keeping source/mix/device values.
The historical FOX application is retained
in `legacy/fox`; it is not compiled. New `.rvpreset` files replace the historical
FOX configuration format. Presets and preferences are not imported automatically.

These initial packages are not Developer ID signed/notarized or Windows signed.
macOS targets version 11 or newer. The Linux package targets Ubuntu 22.04 or a
compatible newer distribution and uses X11/XWayland. Audio hardware, driver
latency, microphone permissions, MIDI equipment, and listening quality require
testing on the target system; CI does not substitute for those checks.

On macOS, open the DMG and copy `reVoice.app` to Applications. On Windows, extract
the ZIP and run `reVoice.exe`. On Linux, extract the archive and run `bin/reVoice`.
Use headphones when testing live microphone input.
