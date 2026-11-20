# reVoice plug-in preview

Development builds of reVoice 0.3: VST3 for macOS Apple Silicon/Intel, Windows
x64 and Linux x86_64; AUv2 for macOS. The standalone 0.2 release is unchanged.

The plug-ins reuse the YIN/Pandora engine and expose their controls through the
DAW's generic parameter editor. The Studio black custom editor is a later step.
Insert reVoice on a vocal audio track; the DAW handles audio devices and routing.

## Install and test

Quit your DAW before copying a new build. Extract the preview archive and copy:

- macOS VST3: `VST3/reVoice.vst3` to `~/Library/Audio/Plug-Ins/VST3/`.
- macOS AU: `AU/reVoice.component` and optional `AU/reVoice MIDI.component` to
  `~/Library/Audio/Plug-Ins/Components/`.
- Windows: `VST3/reVoice.vst3` to `C:\Program Files\Common Files\VST3\` or your
  DAW's configured VST3 folder. Copy the entire bundle directory.
- Linux: `VST3/reVoice.vst3` to `~/.vst3/`. Linux CI builds target Ubuntu 24.04
  or a compatible newer distribution.

Rescan plug-ins in the DAW. The AU folder contains a regular **reVoice** audio
effect and a **reVoice MIDI** controlled effect. Use the regular effect on a
vocal audio track. Choose the host's Controls view for the initial editor.
Host routing of MIDI to an audio effect varies between DAWs. MIDI control defaults to Off; enable Fundamental note
or Four-voice chord when routing MIDI notes to the effect.

Harmony starts at 0 dB. For a clear test, reduce Original voice to its minimum,
select Custom chord, enable one voice and set its Custom interval to +12. Sing
a steady note: the effect should produce the octave above. Reverb and Morphizer
can be adjusted separately. Use headphones with live monitoring.

## Processing behavior

- Mono/stereo input and output buses, 32-bit float processing, sample rates
  from 8 to 192 kHz. Stereo input is summed to mono for vocal harmonization.
  YIN uses a fixed 60–1397 Hz vocal range in this preview.
- The Original control follows the same mono vocal path. Bypass preserves
  the original stereo signal and uses the reported latency, with a short fade.
- Sample-offset automation and MIDI events; stable IDs and versioned project
  state; no audio devices opened by the plug-in and no callback allocation.
- Structural settings use fixed defaults while active. YIN's input gate,
  harmony/scale modes, voice controls, mix, granular controls and reverb remain
  available. The configured table delay is reported to the host; pitch tracking
  also requires a short acquisition window after a new note.
- Different instances own their reverb sample-rate and decay configuration.
- Builds use local ad-hoc signing on macOS. Developer ID notarization and
  Windows signing are not configured for this preview.

## Build

Requires CMake 3.25+, Git and C++17. AU targets additionally use C++23 supported
by the Apple toolchain; Xcode 16.2 is tested locally. SDKs are pinned to official
VST3 SDK 3.8.1 and AudioUnitSDK 1.4.0 revisions. No GUI framework is added.

```sh
cmake --preset plugins
cmake --build --preset plugins --parallel
ctest --preset plugins --parallel 3
cpack --config build/plugins/CPackConfig.cmake -C Release
```

The build does not install anything into your DAW's folders. VST3 bundles are in
`build/plugins/VST3/Release/`; the Mac AUs are in `build/plugins/AU/`. Each AU contains
its own VST3 binary, so installing the separate VST3 is optional.

The official VST3 validator runs through CTest. Adapter tests measure shifted
audio from quiet input, MIDI targets, automation offsets, session recall,
latency-aligned bypass, in-place/oversized buffers and allocation-free callbacks.
The AU test registers the local component only in its test process and checks
audio, MIDI, mix automation, project recall and latency without audio hardware.
On a sandboxed local machine, the AU test may require permission to contact
Apple's component manager.
