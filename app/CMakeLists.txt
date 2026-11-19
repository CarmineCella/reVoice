add_executable(reVoice WIN32 MACOSX_BUNDLE main.cpp MainWindow.cpp StudioWidgets.cpp)
target_compile_definitions(reVoice PRIVATE REVOICE_VERSION="${PROJECT_VERSION}")
target_link_libraries(reVoice PRIVATE revoice_devices fltk::fltk revoice_options)
if(APPLE)
  set_target_properties(reVoice PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/packaging/Info.plist.in
    MACOSX_BUNDLE_GUI_IDENTIFIER org.carminecella.reVoice
    MACOSX_BUNDLE_BUNDLE_NAME reVoice
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
  install(TARGETS reVoice BUNDLE DESTINATION . COMPONENT Runtime)
elseif(WIN32)
  target_sources(reVoice PRIVATE ${PROJECT_SOURCE_DIR}/packaging/reVoice.rc)
  install(TARGETS reVoice RUNTIME DESTINATION . COMPONENT Runtime)
  # Supply the compiler runtime alongside the portable ZIP.
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT Runtime)
  include(InstallRequiredSystemLibraries)
else()
  install(TARGETS reVoice RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
  install(FILES ${PROJECT_SOURCE_DIR}/packaging/revoice.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications COMPONENT Runtime)
endif()
