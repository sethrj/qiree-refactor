#----------------------------------*-CMake-*----------------------------------#
# Copyright 2024 UT-Battelle, LLC, and otrher QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#[=======================================================================[.rst:

QIREEUtils
----------

CMake utilities for managing QIR-EE.

.. command:: qiree_add_library

  Add a library that defines
  the ``QIREE::`` aliases, is generated into the ``lib/`` build
  directory, and installs with the target export.

.. command:: qiree_add_object_library

  Add an OBJECT library to reduce dependencies (e.g. includes) from other
  libraries.

.. command:: qiree_add_executable

  Create an executable and install it::

    qiree_add_executable(<target> [<source> ...])

  The ``<target>`` is a unique identifier for the executable target. The actual
  executable name may end up with an .exe suffix (e.g. if it's windows). The
  executable will be built into the top-level ``bin`` directory, so all
  executables will sit side-by-side before installing.

  The ``<source>`` arguments are passed to CMake's builtin ``add_executable``
  command.

.. command:: qiree_configure_file

  Configure to the build "include" directory for later installation::

    qiree_configure_file(<input> <output> [ARGS...])

  The ``<input>`` must be a relative path to the current source directory, and
  the ``<output>` path is configured to the project build "include" directory.


#]=======================================================================]
#-----------------------------------------------------------------------------#

function(qiree_add_library target)
  add_library(${target} ${ARGN})

  # Add QIREE:: namespace alias
  add_library(QIREE::${target} ALIAS ${target})

  # Build all targets in lib/
  set_target_properties(${target} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${QIREE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${QIREE_LIBRARY_OUTPUT_DIRECTORY}"
  )

  # Install all targets to lib/
  install(TARGETS ${target}
    EXPORT qiree-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT runtime
  )
endfunction()

#-----------------------------------------------------------------------------#
# Add an object library to limit the propagation of includes to the rest of the
# library.
function(qiree_add_object_library target)
  add_library(${target} OBJECT ${ARGN})
  install(TARGETS ${target}
    EXPORT qiree-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT runtime
  )
endfunction()

#-----------------------------------------------------------------------------#

function(qiree_add_executable target)
  add_executable("${target}" ${ARGN})
  install(TARGETS "${target}"
    EXPORT qiree-targets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT runtime
  )
  set_target_properties("${target}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${QIREE_RUNTIME_OUTPUT_DIRECTORY}"
  )
endfunction()

#-----------------------------------------------------------------------------#

function(qiree_configure_file input output)
  if(NOT IS_ABSOLUTE "${input}")
    set(input "${CMAKE_CURRENT_SOURCE_DIR}/${input}")
  endif()
  configure_file("${input}"
    "${QIREE_HEADER_CONFIG_DIRECTORY}/${output}"
    ${ARGN})
endfunction()

#-----------------------------------------------------------------------------#
