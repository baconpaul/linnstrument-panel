cmake_minimum_required(VERSION 3.10)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11 CACHE STRING "Build for 10.11")

project(linnstrument-panel VERSION 0.9.0 LANGUAGES CXX)

add_subdirectory(libs/JUCE)

execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

juce_add_gui_app(linnstrument-panel
    PRODUCT_NAME "linnstrument-panel"
    COMPANY_NAME "Surge Synth Team"  
    BUNDLE_ID "org.surge-synth-team.linnstrument-panel"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    
    FORMATS Standalone
)

juce_generate_juce_header( linnstrument-panel )

file(GLOB LSPAN_RESOURCES_GLOB
  resources/*.ttf 
  resources/*.png 
  resources/*.svg
  )

juce_add_binary_data( linnstrument-panel-binary
  SOURCES ${LSPAN_RESOURCES_GLOB}
)

set_target_properties(linnstrument-panel-binary PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
    
target_include_directories(linnstrument-panel 
  PRIVATE
  src
  libs/tuning-library/include
)

configure_file(
  ${CMAKE_SOURCE_DIR}/Source/version.cpp.in
  ${CMAKE_BINARY_DIR}/generated/version.cpp
)

target_sources(linnstrument-panel PRIVATE
  ${CMAKE_BINARY_DIR}/generated/version.cpp
)

target_compile_definitions(linnstrument-panel PUBLIC
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1

    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0

    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0

    JUCE_ALSA=1
    JUCE_JACK=1
)

target_link_libraries(linnstrument-panel PRIVATE
    linnstrument-panel-binary
    juce::juce_analytics
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_blocks_basics
    juce::juce_box2d
    juce::juce_dsp
)

# Build installers
get_target_property( OUTPUT_DIR linnstrument-panel RUNTIME_OUTPUT_DIRECTORY )
add_custom_target( well-named-assets )

add_custom_target( well-named-standalone )
add_dependencies( well-named-standalone linnstrument-panel_Standalone )
add_dependencies( well-named-assets well-named-standalone )

add_custom_target( installer-pkg )
add_dependencies( installer-pkg well-named-assets )
add_dependencies( installer-pkg linnstrument-panel_Standalone )

set( INSTALLER_DIR ${CMAKE_BINARY_DIR}/asset )
set( WELL_NAMED_ASSET_DIR ${CMAKE_BINARY_DIR}/product )
set( INSTALLER_BASENAME "linnstrument-panel" )

add_custom_command(
  TARGET well-named-standalone
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory  ${WELL_NAMED_ASSET_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${OUTPUT_DIR}/Standalone ${WELL_NAMED_ASSET_DIR}
  )


if( APPLE )
  add_custom_command(
    TARGET installer-pkg
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${INSTALLER_DIR}
    COMMAND hdiutil create ${INSTALLER_DIR}/${INSTALLER_BASENAME}-macos-${GIT_COMMIT_HASH}.dmg -ov -volname "${INSTALLER_BASENAME}-${GIT_COMMIT_HASH}" -fs HFS+ -srcfolder ${WELL_NAMED_ASSET_DIR}
    )
elseif( UNIX )
  execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE LINUX_BUILD_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  add_custom_command(
    TARGET installer-pkg
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${INSTALLER_DIR}
    COMMAND ls -l ${OUTPUT_DIR}
    COMMAND cd ${WELL_NAMED_ASSET_DIR} && tar czf "${INSTALLER_DIR}/${INSTALLER_BASENAME}-linux-${LINUX_BUILD_ARCH}-${GIT_COMMIT_HASH}.tgz" .
    )
else()
  file( TO_NATIVE_PATH "${INSTALLER_DIR}/${INSTALLER_BASENAME}-win-${GIT_COMMIT_HASH}.zip" OUTPUT_ZIP )
  add_custom_command(
    TARGET installer-pkg
    POST_BUILD
    WORKING_DIRECTORY ${WELL_NAMED_ASSET_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${INSTALLER_DIR}
    COMMAND powershell Compress-Archive 
             -DestinationPath "${OUTPUT_ZIP}"
             -Force
             -Path "${WELL_NAMED_ASSET_DIR}"
    )
endif()
