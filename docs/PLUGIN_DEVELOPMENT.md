# Plug-in development checkpoint

The standalone release is published as
[v0.2.0](https://github.com/CarmineCella/reVoice/releases/tag/v0.2.0), commit
`667535f`. All four desktop builds and sanitizers passed; the release contains
Mac arm64/Intel DMGs, Windows x64 ZIP, Linux x86_64 archive, source and checksums.

Plug-in development is on `plugins/vst3-au`, version 0.3.0. It does not change
the released 0.2 binaries or tag. No 0.3 release has been tagged.

## Implemented

- VST3 for macOS arm64/x86_64, Windows x64 and Linux x86_64, using the official
  MIT-licensed VST3 SDK 3.8.1, revision
  `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`.
- Two macOS AUv2 bundles: a regular audio effect and a MIDI-controlled effect.
  Each wraps its private VST3 copy using Steinberg's AU wrapper and Apple's
  Apache-2.0 AudioUnitSDK 1.4.0, revision
  `bd98b31feff57a15989fcfab4cd86dc63382b1ac`.
- The shared YIN/Pandora engine processes host audio/MIDI. No device backend or
  GUI framework is linked into the plug-ins. Hosts supply the initial parameter
  editor; the Studio black custom editor remains a later step.
- Mono/stereo buses, float32 at 8–192 kHz, stable parameter IDs, logarithmic gain
  controls, normalized automation, versioned project state and delayed stereo
  bypass with a short crossfade. Vocal processing itself is mono.
- Sample-offset automation/MIDI, preallocated audio buffers, chunked oversized
  host blocks and no allocation in the tested process callbacks. Structural
  settings remain fixed while active. State loading uses atomic publication so
  the audio thread never waits for the host's state writer.
- Reverb configures decay/delay from each instance's own sample rate without
  modifying STK's global rate. Tests cover mixed-rate instances and actual
  impulse latency. Harmony defaults to 0 dB.
- Optional CMake plug-in preset, bundled dependency notices, macOS ad-hoc signing,
  installation instructions and separate native preview packages. The build
  does not install plug-ins into user folders.
- Four native plug-in CI jobs plus the existing desktop/sanitizer jobs. Tagged
  releases wait for all jobs and include plug-in archives.

## Local validation

On Apple Silicon with Xcode 16.2, all sixteen CTest entries pass, including the
official VST3 validator and direct VST3/AU host tests. Both AU variants pass
Apple's `auval`. The wrapper does not expose the recommended AU tail-time property;
`auval` reports that warning and passes. No DAW GUI or listening test is claimed.

Host tests measure octave-up output from quiet 220 Hz input, MIDI target pitch,
sample-offset gain automation, session recall, bypass latency, in-place and
oversized buffers, finite output and callback allocation counts. VST3 tests also
exercise state loading during rendering and preservation across reactivation.
The headless engine/renderer sanitizer build passes all fourteen entries.

Build and installation details are in [plugins/README.md](../plugins/README.md).
The local Mac preview is `reVoice-plugins-0.3.0-Darwin-arm64.zip`.

## Continue

1. Check the draft pull request and all four plug-in CI jobs. Fix any platform
   failures before treating the previews as portable.
2. Test the built plug-ins in a DAW: discovery, dry/harmony balance, live vocal
   quality, MIDI routing, bypass, project save/reopen and multiple instances.
3. Adapt the Studio black editor after host integration is confirmed. Review AU
   tail reporting and reset behavior during DAW seeks as part of host testing.
4. Keep 0.3 as a preview until the user has tested it. Do not retag 0.2 or publish
   0.3 without the release decision. Signing/notarization remains future work.

Keep commit authorship under the user's configured identity, with no AI co-author
trailers. The user authorized committing/pushing 0.2 and starting plug-in work.
