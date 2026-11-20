# Third-party notices

The reVoice application and its new engine integration are BSD 2-Clause.

| Component | License | Source |
| --- | --- | --- |
| Pandora | LGPL 2.1, retained from the original distribution | `libs/pandora/LICENSE` |
| FLTK | LGPL 2 with the FLTK linking and subclassing exceptions | https://www.fltk.org/COPYING.php |
| PortAudio | MIT-style PortAudio license | https://github.com/PortAudio/portaudio |
| PortMidi | MIT | https://github.com/PortMidi/portmidi |
| Synthesis ToolKit (STK) | STK permissive license | https://github.com/thestk/stk |
| VST3 SDK (plug-in builds) | MIT | https://github.com/steinbergmedia/vst3sdk |
| AudioUnitSDK (macOS AU builds) | Apache 2.0 | https://github.com/apple/AudioUnitSDK |

reVoice is based in part on the work of the FLTK project (https://www.fltk.org).
The FLTK exception permits static linking without relicensing reVoice.
The reverb uses STK's NRev implementation. Dependency license files are included
in binary packages, along with Pandora's LGPL text and the reVoice license.
Each engine configures its own NRev delay lengths and decay coefficients without
modifying STK's global sample rate. Plug-in bundles include their SDK licenses;
the AU bundle contains a private VST3 bundle and Apple's SDK license.

The source distribution and documented CMake build allow recipients to rebuild
the application with a modified Pandora library. Dependency sources are fetched
from the pinned upstream revisions listed in `cmake/Dependencies.cmake`.
Plug-in SDK revisions and required submodules are pinned in `plugins/CMakeLists.txt`.
