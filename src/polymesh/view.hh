#pragma once

#include <polymesh/primitives.hh>
#include <polymesh/tmp.hh>

namespace polymesh
{
template <class CollectionT, class FuncT>
struct attribute_view
{
    attribute_view(CollectionT& collection, FuncT func) : mCollection(collection), mFunc(std::move(func)) {}

    using index_t = typename std::decay<CollectionT>::type::index_t;
    using handle_t = typename std::decay<CollectionT>::type::handle_t;
    using input_t = decltype(std::declval<CollectionT>()[index_t()]);
    using output_t = typename tmp::decayed_result_type_of<FuncT, input_t>;

    decltype(auto) operator[](handle_t h) const { return mFunc(mCollection(h)); }
    decltype(auto) operator[](index_t h) const { return mFunc(mCollection(h)); }

    decltype(auto) operator()(handle_t h) const { return mFunc(mCollection(h)); }
    decltype(auto) operator()(index_t h) const { return mFunc(mCollection(h)); }

    decltype(auto) operator[](handle_t h) { return mFunc(mCollection(h)); }
    decltype(auto) operator[](index_t h) { return mFunc(mCollection(h)); }

    decltype(auto) operator()(handle_t h) { return mFunc(mCollection(h)); }
    decltype(auto) operator()(index_t h) { return mFunc(mCollection(h)); }

    int size() const { return mCollection.size(); }

    template <class F>
    auto map(F&& f) const
    {
        auto new_f = [f0 = mFunc, f1 = std::forward<F>(f)](auto&& v) -> decltype(auto) { return f1(f0(v)); };
        return attribute_view<CollectionT, decltype(new_f)>(mCollection, std::move(new_f));
    }

private:
    CollectionT& mCollection;
    FuncT mFunc;
};
}
