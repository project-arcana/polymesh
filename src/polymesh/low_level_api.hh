#pragma once

namespace polymesh
{
class Mesh;

struct low_level_api
{
    Mesh const& m;

    low_level_api(Mesh const& m) : m(m) { }
    low_level_api(Mesh const* m) : m(*m) { }
};
}
