# Plug-in development checkpoint

The standalone release is published as
[v0.2.0](https://github.com/CarmineCella/reVoice/releases/tag/v0.2.0), commit
`667535f`. All four desktop builds and sanitizers passed; the release contains
Mac arm64/Intel DMGs, Windows x64 ZIP, Linux x86_64 archive, source and checksums.

Plug-in work is on `plugins/vst3-au`; it does not change the release tag.

## Chosen design

- VST3 for macOS arm64/x86_64, Windows x64 and Linux x86_64.
- AUv2 on macOS, using Steinberg's VST3-to-AU wrapper and Apple's AudioUnit SDK.
- Reuse the engine. The DAW supplies audio/MIDI and the first parameter editor;
  adapting Studio black is a later editor step. No device opening in a plug-in.
- Mono/stereo host buses, monophonic vocal analysis, stable parameter IDs based
  on persisted keys, normalized automation, project state and delayed bypass.
- Fixed structural DSP settings while active, preallocated audio scratch storage,
  and sample-offset handling for automation and MIDI. No callback allocation.

## Completed

- Engine reverb now configures decay/delay from each instance's own sample rate,
  without modifying STK's global rate. A mixed-rate regression and an actual
  impulse-latency test pass, along with all fourteen engine suites.
- Initial VST3 processor/controller/factory and parameter/state model are saved
  in `plugins/`. This is a source checkpoint; the plug-in targets are not yet
  connected to CMake or validated and there are no usable plug-in binaries yet.
- Official SDK sources are locally available in the ignored `build/sdk-inspect/`:
  VST3 SDK 3.8.1, commit `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`, and
  AudioUnitSDK 1.4.0, commit `bd98b31feff57a15989fcfab4cd86dc63382b1ac`.

## Next

1. Add optional pinned SDK/CMake targets and a separate plug-in build preset.
   Disable automatic copying into user plug-in directories. Apple SDK 1.4 uses
   C++23; keep the core/VST3 in C++17 and apply C++23 only to AU targets.
2. Compile the adapter, address lifecycle/state-concurrency issues and run the
   official VST3 validator plus host-level audio, MIDI, preset and bypass tests.
3. Build the AU wrapper with a private copy of the VST3 bundle; bundle SDK and
   Pandora/STK notices. Validate with Apple's AU tools.
4. Extend CI for the selected plug-in targets and package reviewable prototypes.

Keep commit authorship under the user's configured identity, with no AI co-author
trailers. The user authorized committing/pushing 0.2 and starting plug-in work.
