#pragma once

#include <polymesh/primitives.hh>
#include <polymesh/tmp.hh>

namespace polymesh
{
template <class CollectionT, class FuncT>
struct attribute_view
{
    attribute_view(CollectionT collection, FuncT func) : mCollection(collection), mFunc(std::move(func)) {}

    using index_t = typename std::decay<CollectionT>::type::index_t;
    using handle_t = typename std::decay<CollectionT>::type::handle_t;
    using input_t = decltype(std::declval<CollectionT>()[index_t()]);
    using output_t = typename tmp::decayed_result_type_of<FuncT, input_t>;

    output_t operator[](handle_t h) const { return mFunc(mCollection(h)); }
    output_t operator[](index_t h) const { return mFunc(mCollection(h)); }

    output_t operator()(handle_t h) const { return mFunc(mCollection(h)); }
    output_t operator()(index_t h) const { return mFunc(mCollection(h)); }

    int size() const { return mCollection.size(); }

    template <class F>
    auto map(F&& f) const
    {
        auto f2 = [ff = std::forward<F>(f)](output_t const& v) { return ff(v); };
        return attribute_view<CollectionT, decltype(f2)>(mCollection, std::move(f2));
    }

private:
    CollectionT mCollection;
    FuncT mFunc;
};
}
