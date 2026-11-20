
cmake_minimum_required(VERSION 3.22)

project(ParticulaDubMatrix VERSION 0.1.0)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(ParticulaDubMatrix
    COMPANY_NAME "Particula Primordial"
    BUNDLE_ID "audio.particulaprimordial.dubmatrix"
    PLUGIN_MANUFACTURER_CODE Ppdm
    PLUGIN_CODE DbMx
    FORMATS VST3
    PRODUCT_NAME "Particula Dub Matrix"
    DESCRIPTION "Stereo dub delay and filtered feedback processor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Generate JuceHeader.h required by the source files
juce_generate_juce_header(ParticulaDubMatrix)

target_sources(ParticulaDubMatrix PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(ParticulaDubMatrix PRIVATE cxx_std_20)

target_compile_definitions(ParticulaDubMatrix PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ParticulaDubMatrix PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
