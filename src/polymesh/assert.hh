#pragma once

#include "macros.hh"

// least overhead assertion macros
// see https://godbolt.org/z/mWdaj3
// [[unlikely]] produces more code in O0 so it is only used outside of debug
// decltype(...) is an unevaluated context, thus eliminating any potential side effect
// assertion handler is customizable

// POLYMESH_ASSERT(cond) aborts if `cond` is false
// NOTE: neither macro must contain side effects!

// compile flags
// POLYMESH_ENABLE_ASSERTIONS enables assertions

#if !defined(POLYMESH_ENABLE_ASSERTIONS)
#define POLYMESH_ASSERT(condition) POLYMESH_UNUSED(condition)
#else
#define POLYMESH_ASSERT(condition) \
    (POLYMESH_UNLIKELY(!(condition)) ? ::polymesh::detail::assertion_failed({#condition, POLYMESH_PRETTY_FUNC, __FILE__, __LINE__}) : void(0)) // force ;
#endif

namespace polymesh
{
namespace detail
{
struct assertion_info
{
    char const* expr;
    char const* func;
    char const* file;
    int line;
};

[[noreturn]] POLYMESH_COLD_FUNC POLYMESH_DONT_INLINE void assertion_failed(assertion_info const& info);
} // namespace detail

/// handler that is called whenever an assertion is violated
/// pass nullptr to reset to default handler
/// this is a thread_local handler
/// the handler must be replaced before it is deleted (non-owning view)
void set_assertion_handler(void (*handler)(detail::assertion_info const& info));
}
