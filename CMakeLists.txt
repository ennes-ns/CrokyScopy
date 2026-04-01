cmake_minimum_required(VERSION 3.22)

project(CrokyScopy VERSION 0.1.0)

# 0. Global JUCE Configuration (MUST BE BEFORE FETCH)
set(JUCE_UNIT_TESTS 1 CACHE BOOL "" FORCE)

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

# 3. Source Files (Modular)
target_sources(CrokyScopy
    PRIVATE
    Source/DSP/PluginProcessor.cpp
    Source/DSP/PluginProcessor.h
    Source/UI/HUDWindow.cpp
    Source/UI/HUDWindow.h
    Source/UI/ScopeComponent.cpp
    Source/UI/ScopeComponent.h
    Source/Core/ScopeBuffer.h
    Source/Common.h
)

# 4. Compile Definitions & Options
target_compile_definitions(CrokyScopy
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DIRECT2D=1
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

# 7. Add Unit Test Target (Console App)
juce_add_console_app(CrokyScopyTests
    PRODUCT_NAME "CrokyScopyTests"
)

target_sources(CrokyScopyTests
    PRIVATE
    Source/Core/ScopeBufferTest.cpp
    Source/Core/ScopeBuffer.h
)

target_link_libraries(CrokyScopyTests
    PRIVATE
    juce::juce_core
    juce::juce_audio_basics
)

juce_generate_juce_header(CrokyScopyTests)
