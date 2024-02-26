# QIR-EE

The QIR Execution Engine library provides interfaces for easily processing
[Quantum Intermediate
Representation](https://github.com/qir-alliance/qir-spec/) code using the LLVM
execution engine.

# Documentation

Most of the QIR-EE documentation is readable through the codebase through a
combination of [static RST documentation](doc/index.rst) and Doxygen-markup
comments in the source code itself. The full [QIR-EE user
documentation][user-docs] (including selected code documentation incorporated
by Breathe) and the [QIR-EE code documentation][dev-docs] are (TODO: will
be) mirrored on
our GitHub pages site. You can generate these yourself (if the necessary
prerequisites are installed) by
setting the `QIREE_BUILD_DOCS=ON` configuration option and running `make
doc` (user) or `make doxygen` (developer).

[user-docs]: https://ornl-qci.github.io/qir-ee/user/index.html
[dev-docs]: https://ornl-qci.github.io/qir-ee/dev/index.html


# Directory structure

| **Directory** | **Description**                                       |
| ------------- | ---------------                                       |
| **app**       | Source code for installed executable applications     |
| **cmake**     | Implementation code for CMake build configuration     |
| **doc**       | Code documentation and manual                         |
| **scripts**   | Development and continuous integration helper scripts |
| **src**       | Library source code                                   |
| **test**      | Unit tests                                            |

# Installation for developers

The `scripts/build.sh` uses CMake user presets to set up your local build
directories.

TODO

