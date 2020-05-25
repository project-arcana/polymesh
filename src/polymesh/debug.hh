#pragma once

#include <iosfwd>
#include <string>

#include "Mesh.hh"

namespace polymesh
{
void print_debug(Mesh const& m);

void write_dot_graph(Mesh const& m, std::ostream& out);

void write_dot_graph(Mesh const& m, std::string const& filename);
}
