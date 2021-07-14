.. _development:

================================================================================
Development
================================================================================

These pages are primarily focused towards developers either contributing to the
PROJ project or using the library in their own software.


.. toctree::
   :maxdepth: 1

   quickstart
   transformations
   errorhandling
   threads
   reference/index
   cmake
   bindings
   migration

The source code for PROJ is maintained in a
`git repository on GitHub <https://github.com/OSGeo/PROJ>`_.
Additionally, a collection of PROJ-compatible transformation grids
are maintained in a `separate git repository <https://github.com/OSGeo/proj-datumgrid>`_.

.. attention::

    The :file:`projects.h` header and the functions related to it is considered
    deprecated from version 5.0.0 and onwards. The header has been removed
    PROJ in version 6.0.0 released February 1st 2019.

.. attention::

    The nmake build system on Windows is on longer supported in
    version 6.0.0 on onwards. Use CMake instead.

.. attention::

    The :file:`proj_api.h` header and the functions related to it is
    considered deprecated from version 5.0.0 and onwards. The header has been
    removed in version 8.0.0 released March 1st 2021.

.. attention::

    With the introduction of PROJ 5, behavioural changes has been made to
    existing functionality. Consult :ref:`differences` for the details.
