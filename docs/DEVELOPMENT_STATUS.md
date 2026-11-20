# Development checkpoint: 0.3 plug-in preview

The user confirmed that the Studio black interface and YIN harmonizer work.
Version 0.2.0 is the standalone release baseline. The tagged release workflow
requires native builds, regression/UI checks and sanitizers before publishing
packages. The release is published at
https://github.com/CarmineCella/reVoice/releases/tag/v0.2.0 and its CI passed on
all four architectures, including Linux sanitizers. Plug-in development is on
`plugins/vst3-au`; see [the current plug-in checkpoint](PLUGIN_DEVELOPMENT.md).

## Local Studio build

- Graphite and mint FLTK theme, sidebar navigation, four voice cards, live pitch
  history, source/preset controls and a shared mix strip. Advanced parameters,
  audio/MIDI settings, recording and existing presets remain available.
- Design references and prompts: `docs/ui-mockups/`.
- App: `build/release/app/reVoice.app`.
- Release DMG: `reVoice-0.2.0-Darwin-arm64.dmg`.
- Release build, all thirteen regression suites and the expanded UI smoke test
  pass locally. The UI check exercises sidebar and voice navigation callbacks,
  complete parameter coverage, mirrored controls, custom interval mapping,
  device/channel selection and mix restoration.
- Reproduced the original quiet-input failure: a .02-amplitude 220 Hz live
  signal returned no pitch. The new standard-library-only streaming YIN detector
  fixes that case and shares its result with all four pitch shifters. YIN uses
  preallocated storage, filtered decimation, a 5 ms analysis hop, RMS gating,
  normalized differences and interpolated minima. No audio-thread allocation.
- Regression coverage now includes voice-like harmonics/noise, 55–1396 Hz pitch
  at 8–192 kHz sample rates, silence/DC/noise rejection, gain/gate changes, and
  actual shifted output from each isolated voice with dry/reverb muted at
  44.1/48 kHz. Channel 2 input is tested with a silent channel 1.
- Input/output and input-channel selectors are visible in Audio / MIDI, reachable
  through the footer or Cmd/Ctrl+,. Start audio reads current selections rather
  than cached settings. The menu can restore an audible harmony/mix balance.
- Harmony starts at 0 dB (unity gain). A one-time preference update replaces an
  older saved startup level; later adjustments and explicit presets retain their
  chosen levels. The user confirmed the YIN harmonizer works in their test.
- Updated muted device integration passed: 220.013 Hz, about 3.1% CPU; no live
  microphone was opened by the automated test. Audio starts stopped. The user
  confirmed harmonization works; further listening and physical MIDI checks
  remain part of release validation.

## Completed

- Public repository: https://github.com/CarmineCella/reVoice
- Original source import: `5c852ce`. Historical FOX code is in `legacy/fox`.
- BSD 2-Clause application license; Pandora's LGPL 2.1 is preserved separately.
- FLTK 1.4.5 UI, CMake presets, pinned dependencies, native audio/MIDI backend,
  background recording, WAV input, presets, offline rendering, and packaging.
- AdaptiveTuner is omitted from the UI. Available detectors: YIN and legacy Spectral.
- Thirteen Release tests and thirteen AddressSanitizer/UBSan tests pass locally on
  Apple Silicon. Coverage includes actual output pitch, held MIDI notes, every
  harmony/scale combination, queue ordering, WAV bounds and recording finalization.
- Real audio-device integration passed with a muted sine source: detected pitch
  220.08 Hz, about 2.5% CPU and 31.6 ms reported latency at 48 kHz / 256 frames
  on this machine. The microphone was not opened.
- Mac GUI smoke test passed and the Studio layout was visually inspected.
- Original local DMG: `reVoice-0.1.0-Darwin-arm64.dmg`. Dependency notices
  are installed inside the app bundle so they remain when copied to Applications.
- Commits use Carmine Emanuele Cella's configured Git identity. No AI co-author
  trailers are permitted or included.

## Resume here

1. Check CI for the plug-in pull request at
   https://github.com/CarmineCella/reVoice/actions and fix platform failures.
2. Test the macOS plug-in preview in a DAW. Automated tests cover actual shifted
   audio, host MIDI, automation, project state and latency; subjective audio
   quality, host-specific routing and physical MIDI need manual checks.
3. Adapt the Studio black plug-in editor after the host preview is confirmed.
   The current plug-ins use the DAW's generic controls.
4. Keep `v0.2.0` unchanged. A new release requires a separate user decision,
   release notes and verified native packages.

Useful commands:

```sh
gh run list --repo CarmineCella/reVoice --limit 5
cmake --build --preset default --parallel 6
ctest --preset default
ctest --preset sanitize
cmake --build --preset plugins --parallel 6
ctest --preset plugins
build/release/app/reVoice.app/Contents/MacOS/reVoice --audio-smoke-test
```

The packages are unsigned. Code signing/notarization, additional installers and
historical preset import remain future work.
