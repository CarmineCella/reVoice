add_library(revoice_engine STATIC src/Parameters.cpp src/Processor.cpp src/YinDetector.cpp src/WavFile.cpp src/Recorder.cpp)
target_include_directories(revoice_engine PUBLIC include)
target_link_libraries(revoice_engine PUBLIC Pandora::Pandora PRIVATE revoice_stk Threads::Threads revoice_options)
if(REVOICE_BUILD_APP)
  add_library(revoice_devices STATIC src/AudioEngine.cpp)
  target_include_directories(revoice_devices PUBLIC include)
  target_include_directories(revoice_devices PRIVATE ${portaudio_SOURCE_DIR}/include
    ${portmidi_SOURCE_DIR}/pm_common ${portmidi_SOURCE_DIR}/porttime)
  target_link_libraries(revoice_devices PUBLIC revoice_engine PRIVATE
    portaudio_static portmidi Threads::Threads revoice_options)
endif()
