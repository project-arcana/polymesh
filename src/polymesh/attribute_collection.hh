#pragma once

#include <map>
#include <string>

#include "attributes.hh"

namespace polymesh
{
struct attribute_collection
{
private:
    std::map<std::string, primitive_attribute_base<vertex_tag>> mVertexAttrs;
    // TODO
};
}
