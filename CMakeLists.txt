
#
# Copyright (C) 2021 magicxqq <xqq@xqq.im>. All rights reserved.
#
# This file is part of libaribcaption.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

cmake_minimum_required(VERSION 3.11)

project(libaribcaption
    VERSION
        1.0.0
    DESCRIPTION
        "Portable ARIB STD-B24 Caption Decoder/Renderer"
    HOMEPAGE_URL
        "https://github.com/xqq/libaribcaption"
    LANGUAGES
        C CXX
)

# main project detecting
# avoid building test subdirectory if used as subproject (e.g. add_subdirectory)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ARIBCC_IS_MAIN_PROJECT TRUE)
endif()

# Release build by default
if(NOT CMAKE_BUILD_TYPE)
    message("Build type will be Release. Set CMAKE_BUILD_TYPE to indicate other types.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default to Release build" FORCE)
endif()

# Indicate -DARIBCC_BUILD_TESTS:BOOL=ON to build tests
option(ARIBCC_BUILD_TESTS "Build libaribcaption tests" OFF)

# Indicate -DARIBCC_SHARED_LIBRARY:BOOL=ON to build as shared library
option(ARIBCC_SHARED_LIBRARY "Build libaribcaption as shared library" OFF)

# Indicate -DARIBCC_NO_EXCEPTIONS:BOOL=ON to disable C++ exceptions
option(ARIBCC_NO_EXCEPTIONS "Disable C++ Exceptions" OFF)

# Indicate -DARIBCC_NO_RTTI:BOOL=ON to disable C++ RTTI
option(ARIBCC_NO_RTTI "Disable C++ RTTI" OFF)

# Indicate -DARIBCC_NO_RENDERER:BOOL=ON to disable renderer
option(ARIBCC_NO_RENDERER "Disable Renderer" OFF)

if(ARIBCC_NO_RENDERER)
    # Renderer is disabled
    message(STATUS "Renderer has been disabled by ARIBCC_NO_RENDERER")
elseif(WIN32)
    # Windows, use DirectWrite by default
    option(ARIBCC_USE_DIRECTWRITE "Enable DirectWrite text rendering backend" ON)
    option(ARIBCC_USE_GDI_FONT "Enable Win32 GDI font provider" OFF)
elseif(APPLE)
    # macOS or iOS, use CoreText by default
    option(ARIBCC_USE_CORETEXT "Enable CoreText text rendering backend" ON)
elseif(ANDROID OR (${CMAKE_SYSTEM_NAME} STREQUAL "Android") OR ARIBCC_IS_ANDROID)
    # Android, FreeType required
    set(ARIBCC_IS_ANDROID TRUE CACHE BOOL "Specify target OS is Android")
    option(ARIBCC_USE_FREETYPE "Enable FreeType text rendering backend" ON)
else()
    # Linux or other Unix systems, requires Fontconfig & FreeType
    option(ARIBCC_USE_FONTCONFIG "Enable Fontconfig font provider" ON)
    option(ARIBCC_USE_FREETYPE "Enable FreeType text rendering backend" ON)
endif()

option(ARIBCC_USE_EMBEDDED_FREETYPE "Use embedded FreeType instead of find_package from system" OFF)

if(ARIBCC_USE_CORETEXT)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
    find_library(CORETEXT_FRAMEWORK CoreText)
endif()

if(ARIBCC_USE_FONTCONFIG)
    find_package(Fontconfig REQUIRED)
endif()

function(import_embedded_freetype)
    include(FetchContent)
    FetchContent_Declare(freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-11-1
        GIT_SHALLOW TRUE             # Get only target tag version
        GIT_PROGRESS TRUE            # Show download progress
        USES_TERMINAL_DOWNLOAD TRUE  # show download progress in ninja
        USES_TERMINAL_CONFIGURE TRUE # show configure progress in ninja
        USES_TERMINAL_BUILD TRUE     # show build progress in ninja
    )
    FetchContent_GetProperties(freetype)
    if(NOT freetype_POPULATED)
        # Fetch the content
        FetchContent_Populate(freetype)
        # Set variables for FreeType
        set(FT_DISABLE_ZLIB ON CACHE BOOL "Disable zlib for embedded Freetype" FORCE)
        set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable bzip2 for embedded Freetype" FORCE)
        set(FT_DISABLE_PNG ON CACHE BOOL "Disable libpng for embedded Freetype" FORCE)
        set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable harfbuzz for embedded Freetype" FORCE)
        set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable brotli for embedded Freetype" FORCE)
        set(BUILD_SHARED_LIBS OFF)       # Force static link

        # Suppress CMake Policy CMP0048 warning
        set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/EnableCMP0048.cmake")
        # Import FreeType directory
        add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR} EXCLUDE_FROM_ALL)
        unset(CMAKE_PROJECT_INCLUDE_BEFORE)

        set(FREETYPE_INCLUDE_DIRS "${freetype_BINARY_DIR}/include;${freetype_SOURCE_DIR}/include" PARENT_SCOPE)
        set(FREETYPE_LIBRARIES freetype PARENT_SCOPE)
    endif()
endfunction()

if(ARIBCC_USE_FREETYPE AND (NOT ARIBCC_USE_EMBEDDED_FREETYPE))
    # Try Use system-wide installed freetype
    find_package(Freetype)
    if(NOT FREETYPE_FOUND)
        # System-wide freetype not found, use FetchContent to import embedded freetype
        message(STATUS "System-wide installed FreeType not found, fallback to use embedded FreeType")
        set(ARIBCC_USE_EMBEDDED_FREETYPE ON CACHE BOOL "Force embedded FreeType" FORCE)
    else()
        message(STATUS "Use system-wide installed FreeType library from find_package")
    endif()
endif()

if(ARIBCC_USE_EMBEDDED_FREETYPE)
    set(ARIBCC_USE_FREETYPE ON CACHE BOOL "Enable Freetype by ARIBCC_USE_EMBEDDED_FREETYPE" FORCE)
    message(STATUS "Use static-linked embedded FreeType library")
    import_embedded_freetype()
endif()

if(ARIBCC_SHARED_LIBRARY AND (NOT BUILD_SHARED_LIBS))
    set(BUILD_SHARED_LIBS ON)
endif()

if(BUILD_SHARED_LIBS AND (NOT ARIBCC_SHARED_LIBRARY))
    set(ARIBCC_SHARED_LIBRARY ON CACHE BOOL "Indicate built as shared library" FORCE)
endif()

# Generate config header