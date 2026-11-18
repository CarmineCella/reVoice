# reVoice

Real-time vocal harmonizer by Carmine Emanuele Cella, rebuilt from the original
2004 application for macOS, Windows, and Linux. Pandora supplies pitch detection,
formant-preserving granular pitch shifting, four-voice harmonization, and granular
morphing. The desktop interface uses FLTK; audio and MIDI use PortAudio and
PortMidi. Reverb uses STK NRev.

The application is being rebuilt for release 0.1.0. The original FOX sources are
archived in `legacy/fox`. AdaptiveTuner is unavailable and is deliberately omitted
from the rebuilt application; Period and Spectral are the available detectors.

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
**Audio / MIDI**, press **Apply**, then **Start audio**. Input is mono and output
is stereo. Use headphones for live input. The initial release uses CoreAudio on
macOS, WASAPI/DirectSound/MME on Windows, and ALSA on Linux. ASIO is not bundled.

**Source & Mix** selects live input, test tones or a WAV file and controls the
dry, harmony, morphizer and reverb levels. **Harmony** sets chord/scale behavior;
**Voices** exposes each voice's interval, level, pan, vibrato, formants and filter.
**Morphizer** controls the separate granular effect. **Pitch Detector** offers
Period and Spectral; AdaptiveTuner is omitted. A structural change such as detector
size, grain envelope or device selection restarts the stream. Stop recording
before making those changes. Other controls update while audio runs.

Recordings are stereo 16-bit PCM WAV. The reader accepts PCM 8/16/24/32-bit and
float 32/64-bit WAV, downmixes to mono and resamples the source during playback.
MIDI supports a held fundamental note or up to four chord notes, note-off,
velocity, channel selection, all-notes-off and optional MIDI thru. New presets
use `.rvpreset`; historical FOX settings are not imported automatically.

`reVoice --smoke-test` briefly opens and closes the interface without starting
audio or saving preferences. `reVoice --audio-smoke-test` checks the default output
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
