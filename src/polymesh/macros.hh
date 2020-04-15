#pragma once

// =========
// configurations

#ifndef NDEBUG
#define POLYMESH_DEBUG
#endif

#if defined(_MSC_VER)
#define POLYMESH_COMPILER_MSVC
#elif defined(__clang__)
#define POLYMESH_COMPILER_CLANG
#elif defined(__GNUC__)
#define POLYMESH_COMPILER_GCC
#else
#error "Unknown compiler"
#endif


// =========
// compiler specific builtins

#if defined(POLYMESH_COMPILER_MSVC)

#define POLYMESH_PRETTY_FUNC __FUNCTION__

#define POLYMESH_FORCE_INLINE __forceinline
#define POLYMESH_DONT_INLINE __declspec(noinline)

#define POLYMESH_LIKELY(x) x
#define POLYMESH_UNLIKELY(x) x
#define POLYMESH_COLD_FUNC


#elif defined(POLYMESH_COMPILER_CLANG) || defined(POLYMESH_COMPILER_GCC)

#define POLYMESH_PRETTY_FUNC __PRETTY_FUNCTION__

#define POLYMESH_FORCE_INLINE __attribute__((always_inline))
#define POLYMESH_DONT_INLINE __attribute__((noinline))

#define POLYMESH_LIKELY(x) __builtin_expect((x), 1)
#define POLYMESH_UNLIKELY(x) __builtin_expect((x), 0)
#define POLYMESH_COLD_FUNC __attribute__((cold))

#else
#error "Unknown compiler"
#endif


// =========
// common helper

#define POLYMESH_DETAIL_MACRO_JOIN(arg1, arg2) arg1##arg2
#define POLYMESH_MACRO_JOIN(arg1, arg2) POLYMESH_DETAIL_MACRO_JOIN(arg1, arg2)

#define POLYMESH_UNUSED(expr)       \
    do                              \
    {                               \
        void(sizeof(bool((expr)))); \
    } while (false) // force ;
