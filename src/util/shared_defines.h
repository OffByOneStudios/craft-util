#pragma once

/******************************************************************************
** Defines
******************************************************************************/

//
// Platform
//
#if defined _WIN32 || defined __CYGWIN__
#define CRAFT_PLATFORM_WINDOWS
// Arch
#ifdef _WIN64
#define CRAFT_ARCH_X64
#else
#define CRAFT_ARCH_X32
#endif
// Compiler
#if _MSC_VER == 1900
#define CRAFT_COMPILER_VC140
#endif

#endif

#ifdef  __APPLE__
#define CRAFT_COMPILER_CLANG
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
#define CRAFT_PLATFORM_IOS
#define CRAFT_ARCH_X64
#elif TARGET_OS_IPHONE
#ifdef __ARM_ARCH_7A__
#define CRAFT_ARCH_ARM7A
#elif __ARM_ARCH_ISA_A64
#define CRAFT_ARCH_ARM64
#endif
#else
// MacOS
#define CRAFT_PLATFORM_OSX
#define CRAFT_ARCH_X64
#endif
#endif


#ifdef __linux__
#define CRAFT_COMPILER_CLANG
#ifdef __ANDROID__
#define CRAFT_PLATFORM_ANDROID
#ifdef __ARM_ARCH_7A__
#define CRAFT_ARCH_ARM7A
#elif __ARM_ARCH_ISA_A64
#define CRAFT_ARCH_ARM64
#endif
#else
#define CRAFT_PLATFORM_LINUX
#define CRAFT_ARCH_X64
#endif
#endif

#if defined __linux__ || defined __APPLE__
#define CRAFT_PLATFORM_POSIX
#endif

#if defined CRAFT_PLATFORM_LINUX  || defined CRAFT_PLATFORM_WINDOWS || defined CRAFT_PLATFORM_OSX
// Platforms should use OpenGL, GLEW, etc
#define CRAFT_PLATFORM_DESKTOP
#else
// Engine should use embeded Khronos standards, etc
#define CRAFT_PLATFORM_MOBILE
#endif

//
// Compile Levels
//

// Trace
#if defined BUILD_TRACE && !defined BUILD_DEBUG && !defined BUILD_RELEASE
#define CRAFT_TRACE
#define CRAFT_DEBUG

// Debug
#elif !defined BUILD_TRACE && defined BUILD_DEBUG && !defined BUILD_RELEASE
#define CRAFT_DEBUG

// Release
#elif !defined BUILD_TRACE && !defined BUILD_DEBUG && defined BUILD_RELEASE

// Error
#else
#error Must choose a single BUILD_DIRECTIVE
#endif

//
// Compile Features
//

#ifdef CRAFT_TRACE
#define SPDLOG_TRACE_ON
#endif

#ifdef CRAFT_DEBUG
#define SPDLOG_DEBUG_ON
#endif

//
// Fuck Microsoft
//

#ifndef _MSC_VER
#define abstract
#else
// Disable microsoft's fuckery with the C++ std
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
