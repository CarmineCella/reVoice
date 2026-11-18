include(FetchContent)
find_package(Threads REQUIRED)

# PortAudio 19.7 still declares CMake 2.8 compatibility. Set a policy floor for
# that unmodified dependency so it also configures with CMake 4.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static dependencies" FORCE)

# Only the three STK translation units needed for NRev are compiled. No STK
# devices, examples, instruments, raw-wave data, or duplicate audio backends.
FetchContent_Declare(stk
  URL https://codeload.github.com/thestk/stk/tar.gz/refs/tags/5.0.1
  URL_HASH SHA256=1dd1a5063efa43d4bc9792d1394b395672fb2222cdb0e56c7c8e3f6db22ece86
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_SUBDIR revoice-no-upstream-build)
FetchContent_MakeAvailable(stk)
add_library(revoice_stk STATIC
  ${stk_SOURCE_DIR}/src/Stk.cpp ${stk_SOURCE_DIR}/src/Delay.cpp ${stk_SOURCE_DIR}/src/NRev.cpp)
target_include_directories(revoice_stk SYSTEM PUBLIC ${stk_SOURCE_DIR}/include)

if(REVOICE_BUILD_APP)
  set(FLTK_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_FORMS OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_GL OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_HTML_DOCS OFF CACHE BOOL "" FORCE)
  set(FLTK_BUILD_PDF_DOCS OFF CACHE BOOL "" FORCE)
  set(FLTK_OPTION_PRINT_SUPPORT OFF CACHE BOOL "" FORCE)
  set(FLTK_BACKEND_WAYLAND OFF CACHE BOOL "Use portable X11 backend on Linux" FORCE)
  set(FLTK_USE_PANGO OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(fltk
    URL https://codeload.github.com/fltk/fltk/tar.gz/refs/tags/release-1.4.5
    URL_HASH SHA256=7715e69ce081fa9ce6da48bb0dd3b07a4cf2cf937813814c04272f36fff593ea
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(fltk)

  set(PA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(PA_BUILD_STATIC ON CACHE BOOL "" FORCE)
  set(PA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(PA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(PA_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
  set(PA_USE_ASIO OFF CACHE BOOL "ASIO SDK is not redistributed" FORCE)
  set(PA_USE_JACK OFF CACHE BOOL "" FORCE)
  set(PA_USE_WDMKS OFF CACHE BOOL "" FORCE)
  set(PA_DLL_LINK_WITH_STATIC_RUNTIME OFF CACHE BOOL "Use consistent MSVC runtime" FORCE)
  FetchContent_Declare(portaudio
    URL https://codeload.github.com/PortAudio/portaudio/tar.gz/refs/tags/v19.7.0
    URL_HASH SHA256=5af29ba58bbdbb7bbcefaaecc77ec8fc413f0db6f4c4e286c40c3e1b83174fa0
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(portaudio)

  set(PM_USE_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
  set(BUILD_PORTMIDI_TESTS OFF CACHE BOOL "" FORCE)
  set(BUILD_JAVA_NATIVE_INTERFACE OFF CACHE BOOL "" FORCE)
  set(BUILD_DOC OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(portmidi
    URL https://codeload.github.com/PortMidi/portmidi/tar.gz/refs/tags/v2.0.7
    URL_HASH SHA256=43fa65b4ed7ebaa68b0028a538ba8b2ca4dc9b86a7e22ec48842070e010f823f
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(portmidi)
endif()

foreach(dependency IN ITEMS stk fltk portaudio portmidi)
  if(DEFINED ${dependency}_SOURCE_DIR)
    foreach(notice IN ITEMS LICENSE LICENSE.txt license.txt COPYING)
      if(EXISTS "${${dependency}_SOURCE_DIR}/${notice}")
        install(FILES "${${dependency}_SOURCE_DIR}/${notice}"
          DESTINATION ${REVOICE_DATA_DIR}/licenses
          RENAME "${dependency}.txt" COMPONENT Runtime)
        break()
      endif()
    endforeach()
  endif()
endforeach()
