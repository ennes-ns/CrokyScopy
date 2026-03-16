cmake_minimum_required(VERSION 3.22)

project(CrokyScope VERSION 0.0.1)

set(JUCE_PLUGINS_NAMES_AND_IDS
    "CrokyScope" "Cs01"
)

# Optional: Add JUCE as a subdirectory or find it
# For now, we assume JUCE is available via FetchContent or a local path.
# We'll use FetchContent for JUCE 8.

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0 # Or specific commit/branch
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(CrokyScope
    COMPANY_NAME "Ennes"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_STEP_LABEL "Copying CrokyScope to VST3 folder"
    FORMATS VST3
    PRODUCT_NAME "CrokyScope"
)

target_sources(CrokyScope
    PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/HUDWindow.cpp
    Source/HUDWindow.h
)

target_compile_definitions(CrokyScope
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(CrokyScope
    PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_audio_devices
)

juce_generate_juce_header(CrokyScope)
