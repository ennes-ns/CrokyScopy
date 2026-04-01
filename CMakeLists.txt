cmake_minimum_required(VERSION 3.22)
project(CrokyScope VERSION 1.0.0)

# Fetch JUCE automatically from github (latest 8.x)
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.1
)
FetchContent_MakeAvailable(juce)

# Setup VST3 project
juce_add_plugin(CrokyScope
    COMPANY_NAME "Ennes"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Crky
    PLUGIN_CODE Scpe
    FORMATS VST3 Standalone
    PRODUCT_NAME "CrokyScope"
)

target_sources(CrokyScope
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/ScopeHUDWindow.cpp
        src/ScopeHUDWindow.h
)

target_compile_definitions(CrokyScope
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(CrokyScope
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
