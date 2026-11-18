# Development checkpoint: 0.1.0

The project is ready for its first GitHub CI run. Version 0.1.0 has not yet been
tagged or published; check all remote builds before making that release.

## Completed

- Public repository: https://github.com/CarmineCella/reVoice
- Original source import: `5c852ce`. Historical FOX code is in `legacy/fox`.
- BSD 2-Clause application license; Pandora's LGPL 2.1 is preserved separately.
- FLTK 1.4.5 UI, CMake presets, pinned dependencies, native audio/MIDI backend,
  background recording, WAV input, presets, offline rendering, and packaging.
- AdaptiveTuner is omitted from the UI. Available detectors: Period and Spectral.
- Eleven Release tests and eleven AddressSanitizer/UBSan tests pass locally on
  Apple Silicon. Coverage includes actual output pitch, held MIDI notes, every
  harmony/scale combination, queue ordering, WAV bounds and recording finalization.
- Real audio-device integration passed with a muted sine source: detected pitch
  220.08 Hz, about 2.5% CPU and 31.6 ms reported latency at 48 kHz / 256 frames
  on this machine. The microphone was not opened.
- Mac GUI construction/exit smoke test passed and the initial layout was inspected.
- Local DMG: `reVoice-0.1.0-Darwin-arm64.dmg` (about 613 KB). Dependency notices
  are installed inside the app bundle so they remain when copied to Applications.
- Commits use Carmine Emanuele Cella's configured Git identity. No AI co-author
  trailers are permitted or included.

## Resume here

1. Inspect https://github.com/CarmineCella/reVoice/actions and fix any Windows,
   Linux, Intel Mac, or Linux leak-check failures. The workflow also runs a GUI
   smoke test and builds packages on every desktop platform.
2. Validate the generated packages and do a manual GUI/control, microphone,
   listening-quality and physical MIDI check. Automated tests do not cover
   physical MIDI hardware or subjective audio quality.
3. Verify a clean working tree and passing CI at the exact release commit.
4. Tag `v0.1.0` and push the tag. The tag workflow waits for all builds/tests,
   then publishes native packages, the source archive and SHA-256 checksums.

Useful commands:

```sh
gh run list --repo CarmineCella/reVoice --limit 5
cmake --build --preset default --parallel 6
ctest --preset default
ctest --preset sanitize
build/release/app/reVoice.app/Contents/MacOS/reVoice --audio-smoke-test
```

The packages are unsigned. Code signing/notarization, additional installers,
visual redesign and historical preset import remain future work.
