cpmaddpackage(
  NAME
    googletest
  GITHUB_REPOSITORY
    google/googletest
  GIT_TAG
    v1.14.0
  OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt ON"
)

# Third-party code: don't let warnings from newer compilers (e.g. Apple clang's
# -Wcharacter-conversion in gtest-printers.h) fail the build.
foreach(gtest_target gtest gtest_main gmock gmock_main)
  if(TARGET ${gtest_target})
    set_target_properties(${gtest_target} PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
  endif()
endforeach()

add_executable(SonicMuffTests
  BigMuffDSPTests.cpp
  PluginProcessorTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Source/PluginProcessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Source/PluginEditor.cpp
)

target_include_directories(SonicMuffTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Source)

# ELK_HEADLESS=0: PluginProcessor::createEditor() constructs the real
# SonicMuffAudioProcessorEditor, so PluginEditor.cpp must be compiled and linked
# in here too, even though no test exercises the editor directly.
target_compile_definitions(SonicMuffTests PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  ELK_HEADLESS=0
  JUCE_PLUGINHOST_VST=0
  JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

target_link_libraries(SonicMuffTests PRIVATE
  GTest::gtest_main
  juce::juce_audio_processors
  juce::juce_audio_basics
  juce::juce_gui_basics
  juce::juce_recommended_config_flags
)

include(GoogleTest)
gtest_discover_tests(SonicMuffTests)
