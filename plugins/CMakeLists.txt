include(FetchContent)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "" FORCE)
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "" FORCE)
set(SMTG_RUN_VST_VALIDATOR OFF CACHE BOOL "" FORCE)
set(SMTG_USE_STATIC_CRT ON CACHE BOOL "" FORCE)
if(APPLE)
  enable_language(OBJCXX)
endif()
FetchContent_Declare(vst3sdk
  GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
  GIT_TAG 3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96 # 3.8.1 build 84, MIT
  GIT_SUBMODULES base cmake pluginterfaces public.sdk
  GIT_SUBMODULES_RECURSE FALSE)
FetchContent_MakeAvailable(vst3sdk)
set(SDK_ROOT ${vst3sdk_SOURCE_DIR})
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)

add_library(revoice_vst_core STATIC PluginModel.cpp VstProcessor.cpp VstController.cpp)
target_include_directories(revoice_vst_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDK_ROOT})
target_link_libraries(revoice_vst_core PUBLIC revoice_engine sdk PRIVATE revoice_options)
smtg_add_vst3plugin(reVoiceVST3 PACKAGE_NAME reVoice SOURCES_LIST Factory.cpp)
smtg_target_add_library_main(reVoiceVST3)
target_compile_definitions(reVoiceVST3 PRIVATE REVOICE_VERSION="${PROJECT_VERSION}")
target_link_libraries(reVoiceVST3 PRIVATE revoice_vst_core)
set_target_properties(reVoiceVST3 PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER org.carminecella.reVoice.vst3
  MACOSX_BUNDLE_BUNDLE_NAME reVoice
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
get_target_property(vst3_package reVoiceVST3 SMTG_PLUGIN_PACKAGE_PATH)
set_property(TARGET reVoiceVST3 APPEND PROPERTY LINK_DEPENDS
  ${PROJECT_SOURCE_DIR}/LICENSE ${PROJECT_SOURCE_DIR}/THIRD_PARTY_NOTICES.md)

add_custom_command(TARGET reVoiceVST3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${vst3_package}/Contents/Resources/licenses"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/LICENSE "${vst3_package}/Contents/Resources/licenses/reVoice.txt"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/libs/pandora/LICENSE "${vst3_package}/Contents/Resources/licenses/Pandora-LGPL-2.1.txt"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${stk_SOURCE_DIR}/LICENSE "${vst3_package}/Contents/Resources/licenses/STK.txt"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDK_ROOT}/LICENSE.txt "${vst3_package}/Contents/Resources/licenses/VST3-SDK.txt"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/THIRD_PARTY_NOTICES.md "${vst3_package}/Contents/Resources/THIRD_PARTY_NOTICES.md")
if(APPLE)
  configure_file(VstInfo.plist.in ${CMAKE_CURRENT_BINARY_DIR}/VstInfo.plist @ONLY)
  set_target_properties(reVoiceVST3 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/VstInfo.plist)
  set_property(TARGET reVoiceVST3 APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/VstInfo.plist)
  add_custom_command(TARGET reVoiceVST3 POST_BUILD COMMAND codesign --force --sign - "${vst3_package}")
endif()
install(DIRECTORY "${vst3_package}" DESTINATION VST3 USE_SOURCE_PERMISSIONS COMPONENT Plugins)
install(FILES README.md DESTINATION . COMPONENT Plugins)

if(APPLE AND REVOICE_BUILD_AU)
  FetchContent_Declare(audiounitsdk
    GIT_REPOSITORY https://github.com/apple/AudioUnitSDK.git
    GIT_TAG bd98b31feff57a15989fcfab4cd86dc63382b1ac # 1.4.0, Apache 2.0
    SOURCE_SUBDIR revoice-no-upstream-build)
  FetchContent_MakeAvailable(audiounitsdk)
  file(GLOB au_sources CONFIGURE_DEPENDS ${audiounitsdk_SOURCE_DIR}/src/AudioUnitSDK/*.cpp)
  add_library(revoice_au_sdk STATIC ${au_sources})
  target_include_directories(revoice_au_sdk SYSTEM PUBLIC ${audiounitsdk_SOURCE_DIR}/include)
  target_compile_features(revoice_au_sdk PUBLIC cxx_std_23)
  target_compile_options(revoice_au_sdk PRIVATE -Wno-deprecated-declarations)
  target_link_libraries(revoice_au_sdk PUBLIC "-framework AudioToolbox" "-framework CoreAudio" "-framework CoreFoundation" "-framework CoreMIDI")

  set(au_wrapper ${SDK_ROOT}/public.sdk/source/vst/auwrapper)
  math(EXPR REVOICE_AU_VERSION "(${PROJECT_VERSION_MAJOR} << 16) | (${PROJECT_VERSION_MINOR} << 8) | ${PROJECT_VERSION_PATCH}")
  function(revoice_add_au target name identifier subtype kind audio_only)
    add_library(${target} MODULE ${au_wrapper}/auwrapper.mm ${au_wrapper}/aucocoaview.mm ${au_wrapper}/NSDataIBStream.mm)
    target_include_directories(${target} PRIVATE ${SDK_ROOT})
    target_compile_features(${target} PRIVATE cxx_std_23)
    target_compile_definitions(${target} PRIVATE SMTG_AUWRAPPER_USES_AUSDK=1
      CA_USE_AUDIO_PLUGIN_ONLY=${audio_only} SMTG_AUCocoaUIBase_CLASS_NAME=${target}CocoaView)
    target_compile_options(${target} PRIVATE -Wno-deprecated-declarations)
    target_link_libraries(${target} PRIVATE revoice_au_sdk sdk_hosting
      "-framework AudioUnit" "-framework Cocoa" "-framework Carbon")
    set(REVOICE_AU_NAME ${name})
    set(REVOICE_AU_IDENTIFIER ${identifier})
    set(REVOICE_AU_SUBTYPE ${subtype})
    set(REVOICE_AU_TYPE ${kind})
    configure_file(AUInfo.plist.in ${CMAKE_CURRENT_BINARY_DIR}/${target}-Info.plist @ONLY)
    set_target_properties(${target} PROPERTIES
      BUNDLE TRUE BUNDLE_EXTENSION component PREFIX "" OUTPUT_NAME ${name}
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/AU
      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/${target}-Info.plist)
    add_dependencies(${target} reVoiceVST3)
    # Keep the private VST3 copy current even when only DSP code changes.
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "$<TARGET_FILE:reVoiceVST3>" ${CMAKE_CURRENT_BINARY_DIR}/${target}-Info.plist)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:${target}>/Contents/Resources"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${vst3_package}" "$<TARGET_BUNDLE_DIR:${target}>/Contents/Resources/plugin.vst3"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${audiounitsdk_SOURCE_DIR}/LICENSE.txt "$<TARGET_BUNDLE_DIR:${target}>/Contents/Resources/AudioUnitSDK-LICENSE.txt"
      COMMAND codesign --force --sign - "$<TARGET_BUNDLE_DIR:${target}>")
    install(DIRECTORY "$<TARGET_BUNDLE_DIR:${target}>" DESTINATION AU USE_SOURCE_PERMISSIONS COMPONENT Plugins)
  endfunction()
  revoice_add_au(reVoiceAU reVoice org.carminecella.reVoice.au rVce aufx 1)
  revoice_add_au(reVoiceMIDIAU "reVoice MIDI" org.carminecella.reVoice.au-midi rVcM aumf 0)
  if(BUILD_TESTING)
    add_executable(revoice-au-tests tests/AudioUnitTests.cpp)
    target_link_libraries(revoice-au-tests PRIVATE revoice_vst_core "-framework AudioToolbox" "-framework CoreFoundation")
    add_dependencies(revoice-au-tests reVoiceAU reVoiceMIDIAU)
    add_test(NAME au-host COMMAND revoice-au-tests "$<TARGET_BUNDLE_DIR:reVoiceAU>" "$<TARGET_BUNDLE_DIR:reVoiceMIDIAU>")
    set_tests_properties(au-host PROPERTIES TIMEOUT 120)
  endif()
endif()

if(BUILD_TESTING)
  add_executable(revoice-plugin-tests tests/HostTests.cpp ${SDK_ROOT}/public.sdk/source/common/memorystream.cpp)
  target_link_libraries(revoice-plugin-tests PRIVATE revoice_vst_core sdk_hosting revoice_options)
  if(APPLE)
    target_link_libraries(revoice-plugin-tests PRIVATE "-framework CoreFoundation")
  endif()
  add_test(NAME plugin-host COMMAND revoice-plugin-tests)
  set_tests_properties(plugin-host PROPERTIES TIMEOUT 120)
  add_test(NAME vst3-validator COMMAND $<TARGET_FILE:validator> "${vst3_package}")
  set_tests_properties(vst3-validator PROPERTIES TIMEOUT 300)
endif()
