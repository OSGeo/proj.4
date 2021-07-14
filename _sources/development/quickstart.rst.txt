.. _dev_quickstart:

================================================================================
Quick start
================================================================================

This is a short introduction to the PROJ API. In the following section we
create a simple program that transforms a geodetic coordinate to UTM and back
again. The program is explained a few lines at a time. The complete program can
be seen at the end of the section.

See the following sections for more in-depth descriptions of different parts of
the PROJ API or consult the :doc:`API reference <reference/index>` for specifics.

Before the PROJ API can be used it is necessary to include the :file:`proj.h` header
file. Here :file:`stdio.h` is also included so we can print some text to the screen:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 39-40

Let's declare a few variables that'll be used later in the program. Each variable
will be discussed below.
See the :doc:`reference for more info on data types <reference/datatypes>`.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 43-46
  :dedent: 4

For use in multi-threaded programs the :c:type:`PJ_CONTEXT` threading-context is used.
In this particular example it is not needed, but for the sake of completeness
it created here. The section on :doc:`threads <threads>` discusses
this in detail.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 50
  :dedent: 4

Next we create the :c:type:`PJ` transformation object ``P`` with the function
:c:func:`proj_create_crs_to_crs`. :c:func:`proj_create_crs_to_crs` takes the threading context ``C``
created above, a string that describes the source coordinate reference system (CRS),
a string that describes the target CRS and an optional description of the area of
use.
The strings for the source or target CRS may be PROJ strings (``+proj=longlat +datum=WGS84``),
CRS identified by their code (``EPSG:4326`` or ``urn:ogc:def:crs:EPSG::4326``) or
by a well-known text (WKT) string:
::

    GEOGCRS["WGS 84",
        DATUM["World Geodetic System 1984",
            ELLIPSOID["WGS 84",6378137,298.257223563,
                LENGTHUNIT["metre",1]]],
        PRIMEM["Greenwich",0,
            ANGLEUNIT["degree",0.0174532925199433]],
        CS[ellipsoidal,2],
            AXIS["geodetic latitude (Lat)",north,
                ORDER[1],
                ANGLEUNIT["degree",0.0174532925199433]],
            AXIS["geodetic longitude (Lon)",east,
                ORDER[2],
                ANGLEUNIT["degree",0.0174532925199433]],
        USAGE[
            SCOPE["unknown"],
            AREA["World"],
            BBOX[-90,-180,90,180]],
        ID["EPSG",4326]]

The use of PROJ strings to describe a CRS is considered as legacy (one of the
main weakness of PROJ strings is their inability to describe a geodetic datum,
other than the few ones hardcoded in the ``+datum`` parameter).
Here we transform from geographic coordinates to UTM zone 32N.
It is recommended to create one threading-context per thread used by the program.
This ensures that all :c:type:`PJ` objects created in the same context will be
sharing resources such as error-numbers and loaded grids.
In case the creation of the :c:type:`PJ` object fails an error message is
displayed and the program returns. See :doc:`errorhandling` for further
details.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 52-60
  :dedent: 4

:c:func:`proj_create_crs_to_crs` creates a transformation object, which accepts
coordinates expressed in the units and axis order of the definition of the
source CRS, and return transformed coordinates in the units and axis order of
the definition of the target CRS.
For almost most geographic CRS, the units will be in most cases degrees (in
rare cases, such as EPSG:4807 / NTF (Paris), this can be grads). For geographic
CRS defined by the EPSG authority, the order of coordinates is latitude first,
longitude second. When using a PROJ string, on contrary the order will be
longitude first, latitude second.
For projected CRS, the units may vary (metre, us-foot, etc..). For projected
CRS defined by the EPSG authority, and with EAST / NORTH directions, the order
might be easting first, northing second, or the reverse.  When using a PROJ string,
the order will be easting first, northing second, except if the ``+axis``
parameter modifies it.

If for the needs of your software, you want
a uniform axis order (and thus do not care about axis order mandated by the
authority defining the CRS), the :c:func:`proj_normalize_for_visualization`
function can be used to modify the PJ* object returned by
:c:func:`proj_create_crs_to_crs` so that it accepts as input and returns as
output coordinates using the traditional GIS order, that is longitude, latitude
(followed by elevation, time) for geographic CRS and easting, northing for most
projected CRS.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 65-71
  :dedent: 4

PROJ uses its own data structures for handling coordinates. Here we use a
:c:type:`PJ_COORD` which is easily assigned with the function :c:func:`proj_coord`.
When using ``+proj=longlat``, the order of coordinates is longitude, latitude,
and values are expressed in degrees. If you used instead a EPSG geographic CRS,
like EPSG:4326 (WGS84), it would be latitude, longitude.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 76
  :dedent: 4

The coordinate defined above is transformed with :c:func:`proj_trans`. For this
a :c:type:`PJ` object, a transformation direction (either forward or inverse)
and the coordinate is needed. The transformed coordinate is returned in ``b``.
Here the forward (:c:type:`PJ_FWD`) transformation from geographic to UTM is made.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 79-80
  :dedent: 4

The inverse transformation (UTM to geographic) is done similar to above,
this time using :c:type:`PJ_INV` as the direction.

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 81-82
  :dedent: 4

Before ending the program the allocated memory needs to be released again:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :lines: 85-86
  :dedent: 4


A complete compilable version of the above can be seen here:

.. literalinclude:: ../../../examples/pj_obs_api_mini_demo.c
  :language: c
  :linenos:
  :lines: 39-

