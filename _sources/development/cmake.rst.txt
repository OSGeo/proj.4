.. _cmake:

********************************************************************************
Using PROJ in CMake projects
********************************************************************************

The recommended way to use the PROJ library in a CMake project is to
link to the imported library target ``${PROJ_LIBRARIES}`` provided by
the CMake configuration which comes with the library. Typical usage is:

.. code::

    find_package(PROJ)

    target_link_libraries(MyApp PRIVATE ${PROJ_LIBRARIES})

By adding the imported library target ``${PROJ_LIBRARIES}`` to the
target link libraries, CMake will also pass the include directories to
the compiler.  This requires that you use CMake version 2.8.11 or later.
If you are using an older version of CMake, then add

.. code::

    include_directories(${PROJ_INCLUDE_DIRS})

The CMake command ``find_package`` will look for the configuration in a
number of places. The lookup can be adjusted for all packages by setting
the cache variable or environment variable ``CMAKE_PREFIX_PATH``. In
particular, CMake will consult (and set) the cache variable
``PROJ_DIR``.

The old CMake name for the PROJ project was "PROJ4" and the switch to
the name "PROJ" was made with version 7.0.  So if you expect your
package to work with pre-7.0 versions of PROJ, you will need to use

.. code::

    find_package(PROJ4)
    target_link_libraries(MyApp PRIVATE ${PROJ4_LIBRARIES})
    include_directories(${PROJ4_INCLUDE_DIRS})

This will also find version 7.0.  This use of the PROJ4 name will be
phased out at some point.
