# polymesh

A C++17 easy-to-use high-performance half-edge data structure with strong functional features.

```cpp
#include <polymesh/pm.hh>
#include <typed-geometry/tg.hh> // some math library

// declare mesh with single attribute
pm::Mesh m;
auto pos = pm::vertex_attribute<tg::pos3>(m);

// load from file
pm::load("/path/to/mesh.ext", m, pos);

// single iteration smoothing
auto smoothed_pos = pm::vertex_attribute<tg::pos3>(m);
for (auto v : m.vertices())
    smoothed_pos[v] = v.adjacent_vertices().avg(pos);
```

## Documentation

for now the sphinx documentation must be built manually:

```
cd docs
pip3 install -r requirements.txt
doxygen
make html
open _build/html/index.html
```

TODO: link to hosted version


## Features

- high-performance half-edge data structure
- external `vector<T>`-like attributes
- highly readable code via smart handles
- encourages functional programming via smart ranges
- many built-in geometry processing algorithms


## Installation

Install `polymesh` by running:

    install project


## Requirements and Dependencies

- C++17
- CMake 3.8+
- msvc, gcc, or clang

The following math libraries are supported as optional dependencies:

- `typed-geometry` (recommended)
- `glm`
- `Eigen`

TODO: links


## Contribute

* Issue Tracker: github.com/TODO/issues
* Source Code: github.com/TODO

If you are having issues, please let us know.

TODO: link to contribution guide


## License

The project is licensed under the MIT license.
