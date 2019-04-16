#include "debug.hh"

#include <fstream>
#include <iostream>

void polymesh::print_debug(const polymesh::Mesh& m)
{
    using namespace std;

    cout << "[Mesh: " << m.vertices().size() << " vertices, " << m.faces().size() << " faces, " << m.edges().size() << " edges]" << endl;

    cout << " vertices:" << endl;
    for (auto v : m.vertices())
        cout << "  [" << v.idx.value << "] -> half-edge #" << v.any_outgoing_halfedge().idx.value << endl;
    cout << endl;

    cout << " faces:" << endl;
    for (auto f : m.faces())
        cout << "  [" << f.idx.value << "] -> half-edge #" << f.any_halfedge().idx.value << endl;
    cout << endl;

    cout << " half-edges:" << endl;
    for (auto h : m.halfedges())
    {
        cout << "  [" << h.idx.value << "] -> (";
        cout << "opp #" << h.opposite().idx.value;
        cout << ", next #" << h.next().idx.value;
        cout << ", prev #" << h.prev().idx.value;
        cout << ", f #" << h.face().idx.value;
        cout << ", v-to #" << h.vertex_to().idx.value;
        cout << ")" << endl;
    }
    cout << endl;
}

void polymesh::write_dot_graph(const polymesh::Mesh& m, std::ostream& out)
{
    out << "strict digraph {\n";
    for (auto v : m.vertices())
        out << "  v" << v.idx.value << ";\n";
    for (auto h : m.halfedges())
    {
        auto v0 = h.vertex_from().idx.value;
        auto v1 = h.vertex_to().idx.value;
        out << "  v" << v0 << " -> v" << v1 << " [label=\"h" << h.idx.value << "\"];\n";
    }
    out << "}\n";
}

void polymesh::write_dot_graph(const polymesh::Mesh& m, const std::string& filename)
{
    std::ofstream file(filename);
    write_dot_graph(m, file);
}
