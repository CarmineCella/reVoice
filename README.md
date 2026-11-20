# reVoice

Real-time vocal harmonizer by Carmine Emanuele Cella, rebuilt from the original
2004 application for macOS, Windows, and Linux. Pandora supplies pitch detection,
formant-preserving granular pitch shifting, four-voice harmonization, and granular
morphing. The desktop interface uses FLTK; audio and MIDI use PortAudio and
PortMidi. Reverb uses STK NRev.

The application is being rebuilt for release 0.1.0. The original FOX sources are
archived in `legacy/fox`. AdaptiveTuner is unavailable and is deliberately omitted
from the rebuilt application. A dependency-free C++ YIN detector is the default;
the historical Spectral detector remains an optional legacy choice.

## Build

Requires CMake 3.24+, a C++17 compiler, Git, and internet access for the first
configure. Dependencies are fetched from pinned archives with SHA-256 checksums.
On macOS install the Xcode command line tools. On Windows use Visual Studio 2022
with the Desktop development with C++ workload. On Debian/Ubuntu install:

```sh
sudo apt-get install build-essential cmake libasound2-dev libx11-dev libxext-dev libxft-dev libxinerama-dev libxcursor-dev libxfixes-dev libxrender-dev
```

```sh
cmake --preset default
cmake --build --preset default --parallel
ctest --preset default
```

For a build without desktop or device dependencies, use
`cmake -S . -B build/headless -DREVOICE_BUILD_APP=OFF`.
For memory and undefined behavior checks on Clang/GCC, use the `sanitize` preset.
To package a Release build, run `cpack --config build/release/CPackConfig.cmake -C Release`.

The offline renderer does not access audio hardware:

```sh
build/release/revoice-render --tone demo.wav 220 2
build/release/revoice-render vocal.wav harmonized.wav 0
```

## Use

Run `build/release/app/reVoice.app` on macOS,
`build/release/app/Release/reVoice.exe` on Windows, or
`build/release/app/reVoice` on Linux. Choose devices and a buffer size in
**Audio / MIDI** (or **Audio devices** in the footer). Select the microphone/line
**Input channel** if your interface has multiple inputs, then **Start audio**.
Start always uses the displayed device selections; **Apply** updates a running
stream. Cmd/Ctrl+, opens this page. The selected input channel is mono and output
is stereo. Use headphones for live input. The initial release uses CoreAudio on
macOS, WASAPI/DirectSound/MME on Windows, and ALSA on Linux. ASIO is not bundled.

The **Studio** home page has four voice cards, pitch history and a shared mix
strip. Select a harmony preset and input source in the header. Choose **Custom
chord** to edit intervals, or **Advanced edit** on a voice for vibrato, formants
and filtering. **Voices > Harmony** sets chord/scale behavior. **Input settings**
selects live input, test tones or a WAV file and exposes the full source and mix
controls. Drag dials vertically; hold Shift for fine adjustment. Double-click a
dial or fader value to type a number.

**Morphizer** controls the separate granular effect. **Pitch detector** offers
YIN and legacy Spectral. YIN analyzes the same input that feeds the harmonizer;
all four pitch shifters share its estimate. It includes an adjustable **YIN input
gate (RMS)** for quiet microphones and rejects unvoiced/noise frames. The display
clears when pitch is lost. The implementation uses preallocated buffers, filtered
decimation, normalized differences and interpolated minima, based on the
[YIN paper](https://doi.org/10.1121/1.1458024), with no new library dependency.
Existing settings select YIN by default when first opened by this build; other
source, mix and device preferences are preserved.

If the original voice masks the harmony, use the **...** menu's **Restore harmony
and mix** command. It restores voice/mix defaults, clears bypass/freeze and MIDI
chord gating, and selects YIN while keeping the source and device selections.
To hear only the harmony, turn **Original** all the way down. Choose **Live input**
in the header for the microphone; device selection does not change the source.
Harmony defaults to **0 dB**. This build updates the previous startup preference
once; subsequent mix adjustments and levels explicitly loaded from presets are
preserved.

AdaptiveTuner is omitted. A structural change such as detector
size, grain envelope or device selection restarts the stream. Stop recording
before making those changes. Other controls update while audio runs.

Recordings are stereo 16-bit PCM WAV. The reader accepts PCM 8/16/24/32-bit and
float 32/64-bit WAV, downmixes to mono and resamples the source during playback.
MIDI supports a held fundamental note or up to four chord notes, note-off,
velocity, channel selection, all-notes-off and optional MIDI thru. New presets
use `.rvpreset`; historical FOX settings are not imported automatically.

`reVoice --smoke-test` opens the interface and checks page navigation, parameter
coverage, mirrored controls, interval mapping, device/channel settings and mix
restoration without starting audio or saving
preferences. `reVoice --audio-smoke-test` checks the default output
using a muted test tone; it does not use the microphone. Physical MIDI hardware
and microphone quality still require manual validation.

## CI and releases

Pushes to `main` and pull requests build and test Linux x86_64, Windows x64,
macOS arm64 and macOS x86_64. Linux additionally runs address, undefined behavior
and leak checks. Every platform also opens the GUI in a smoke test and uploads
its package. Audio-device tests are local; hosted CI runners need no microphone
or MIDI hardware. The `pitchshift` suite measures output spectra and MIDI tuning.

After updating `project(VERSION ...)` and the release notes, pushing a `v*` tag
runs all checks and publishes the packages, full source archive and SHA-256
checksums as a GitHub release. Release packages are unsigned; Developer ID
notarization and Windows code signing are not configured in 0.1.

## Layout

- `app/`: FLTK desktop interface and offline rendering command.
- `engine/`: DSP integration, parameters, device management, recording and WAV I/O.
- `libs/pandora/`: original audio algorithms and regression fixes.
- `tests/`: DSP, WAV, queue, memory and real-time regression coverage.
- `legacy/`: historical application source, excluded from the build.
- `docs/pandora/`: original framework documentation.

## License

reVoice and the new integration code use the BSD 2-Clause license. Pandora retains
its original LGPL 2.1 notice. Dependencies keep their own licenses; see
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
reVoice is based in part on the work of the FLTK project (https://www.fltk.org).
