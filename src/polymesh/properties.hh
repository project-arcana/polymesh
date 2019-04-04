#pragma once
#include "primitives.hh"
#include "tmp.hh"

namespace polymesh
{
template <class CollectionT, class FuncT>
struct readonly_property
{
    readonly_property(CollectionT collection, FuncT func) : mCollection(collection), mFunc(func) {}

    using index_t = typename std::decay<CollectionT>::type::index_t;
    using handle_t = typename std::decay<CollectionT>::type::handle_t;
    using input_t = decltype(std::declval<CollectionT>()[index_t()]);
    using output_t = typename tmp::decayed_result_type_of<FuncT, input_t>;

    output_t operator[](handle_t h) const { return mFunc(mCollection(h)); }
    output_t operator[](index_t h) const { return mFunc(mCollection(h)); }

    output_t operator()(handle_t h) const { return mFunc(mCollection(h)); }
    output_t operator()(index_t h) const { return mFunc(mCollection(h)); }

    int size() const { return mCollection.size(); }

    template <class Func2T>
    auto view(Func2T&& f) const -> readonly_property<readonly_property<CollectionT, FuncT>, Func2T>;

private:
    CollectionT mCollection;
    FuncT mFunc;
};
}
