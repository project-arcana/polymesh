#pragma once

namespace polymesh::detail
{
/// functional that does nothing
struct noop_function
{
    template <class... Args>
    void operator()(Args&&...) const
    {
    }
};
}
