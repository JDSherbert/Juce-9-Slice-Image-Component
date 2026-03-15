# Copyright (c) 2026 JDSherbert. All rights reserved.

cmake_minimum_required(VERSION 3.22)
 
project(NineSliceImageComponent VERSION 1.0.0 LANGUAGES CXX)
 
# ======================================================================= #
# Library Target
# ======================================================================= #
 
add_library(NineSliceImageComponent STATIC
    NineSliceImageComponent.cpp
    NineSliceImageComponent.h
)
 
target_compile_features(NineSliceImageComponent PUBLIC cxx_std_17)
 
# ======================================================================= #
# JUCE Dependency
# ======================================================================= #
# If the consuming project has already called find_package / add_subdirectory
# for JUCE, that target will already exist and this block is skipped.
# Otherwise we fetch JUCE ourselves so the library is self-contained.
 
if(NOT TARGET juce::juce_gui_basics)
    include(FetchContent)
 
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
 
    FetchContent_MakeAvailable(JUCE)
 
    juce_add_module(modules/juce_core)
    juce_add_module(modules/juce_graphics)
    juce_add_module(modules/juce_gui_basics)
endif()
 
target_link_libraries(NineSliceImageComponent
    PUBLIC
        juce::juce_gui_basics
)
 
# Suppress JUCE's default requirement for a JUCE_APPLICATION_NAME etc.
# when used as a standalone library rather than inside a JUCE project.
target_compile_definitions(NineSliceImageComponent
    PUBLIC
        JUCE_STANDALONE_APPLICATION=0
)
 
# ======================================================================= #
# Include Paths
# ======================================================================= #
# Expose the repo root so consumers can #include "NineSliceImageComponent.h"
 
target_include_directories(NineSliceImageComponent
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
