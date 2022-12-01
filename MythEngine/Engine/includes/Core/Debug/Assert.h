#pragma once
#ifndef _WIN32
    #include <signal.h> // SIGTRAP
#endif

#if !defined(_WIN32) && !defined(__linux__)
    #error Platform not supported
#endif

// I borrowed the assert from LumixEngine
// https://github.com/nem0/LumixEngine/blob/master/src/engine/lumix.h

#ifndef MYTH_ASSERT
    #ifdef _DEBUG
        #ifdef _WIN32
            #define MYTH_DEBUG_BREAK() __debugbreak()
        #else
            #define MYTH_DEBUG_BREAK()  raise(SIGTRAP) 
        #endif
        #define MYTH_ASSERT(x) do { const volatile bool myth_assert_b____ = !(x); if(myth_assert_b____) MYTH_DEBUG_BREAK(); } while (false)
    #else
        #if defined(_MSC_VER) && !defined(__clang__)
            #define MYTH_ASSERT(x) __assume(x)
        #else
            #define MYTH_ASSERT(x) { false ? (void)(x) : (void)0; }
        #endif
    #endif
#endif