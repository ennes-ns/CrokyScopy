cmake_minimum_required(VERSION 3.22)

project(CrokyScopy VERSION 0.1.0)

# 0. Global Project Configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch JUCE 8
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)
FetchContent_MakeAvailable(juce)

# 2. Add the Plugin Target
juce_add_plugin(CrokyScopy
    COMPANY_NAME "Ennes"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_STEP_LABEL "Copying CrokyScopy to VST3 folder"
    FORMATS VST3
    PRODUCT_NAME "CrokyScopy"
)

# 3. Source Files
target_sources(CrokyScopy
    PRIVATE
    Source/DSP/PluginProcessor.cpp
    Source/DSP/PluginProcessor.h
    Source/UI/HUDWindow.cpp
    Source/UI/HUDWindow.h
    Source/UI/ScopeComponent.cpp
    Source/UI/ScopeComponent.h
    Source/Core/ScopeBuffer.h
)

# 4. Compile Definitions & Options
# Reverting to default rendering first to ensure 0 lag. Direct2D can be enabled
# later if required, but standard rendering is most stable.
target_compile_definitions(CrokyScopy
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# 5. Link Libraries
target_link_libraries(CrokyScopy
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

# 6. Generate JUCE Header
juce_generate_juce_header(CrokyScopy)

