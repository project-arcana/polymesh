#pragma once

#include <map>
#include <string>

#include "attributes.hh"

namespace polymesh
{
struct attribute_collection
{
private:
    std::map<std::string, vertex_attribute_base> mVertexAttrs;
    // TODO
};
}
