.. _faq:

******************************************************************************
FAQ
******************************************************************************

.. only:: not latex

    .. contents::
       :depth: 3
       :backlinks: none

Which file formats does PROJ support?
--------------------------------------------------------------------------------

The :ref:`command line applications <apps>` that come with PROJ only support text
input and output (apart from :program:`proj` which accepts a simple binary data
stream as well). :program:`proj`, :program:`cs2cs` and :program:`cct` expects
text files with one coordinate per line with each coordinate dimension in a
separate column.

.. note::

    If your data is stored in a common geodata file format chances are that
    you can use `GDAL <https://gdal.org/>`_ as a frontend to PROJ and transform your data with the
    :program:`ogr2ogr` application.

Can I transform from *abc* to *xyz*?
--------------------------------------------------------------------------------

Probably. PROJ supports transformations between most coordinate reference systems
registered in the EPSG registry, as well as a number of other coordinate reference
systems. The best way to find out is to test it with the :program:`projinfo`
application. Here's an example checking if there's a transformation between
ETRS89/UTM32N (EPSG:25832) and ETRS89/DKTM1 (EPSG:4093):

.. code-block:: console

    $ ./projinfo -s EPSG:25832 -t EPSG:4093 -o PROJ
    Candidate operations found: 1
    -------------------------------------
    Operation No. 1:

    unknown id, Inverse of UTM zone 32N + DKTM1, 0 m, World

    PROJ string:
    +proj=pipeline
      +step +inv +proj=utm +zone=32 +ellps=GRS80
      +step +proj=tmerc +lat_0=0 +lon_0=9 +k=0.99998 +x_0=200000 +y_0=-5000000
            +ellps=GRS80

See the :program:`projinfo` :ref:`documentation <projinfo>` for more info on
how to use it.

Coordinate reference system *xyz* is not in the EPSG registry, what do I do?
--------------------------------------------------------------------------------

Generally PROJ will accept coordinate reference system descriptions in the form
of WKT, WKT2 and PROJ strings. If you are able to describe your desired CRS
in either of those formats there's a good chance that PROJ will be able to make
sense of it.

If it is important to you that a given CRS is added to the EPSG registry, you
should contact your local geodetic authority and ask them to submit the CRS for
inclusion in the registry.

I found a bug in PROJ, how do I get it fixed?
--------------------------------------------------------------------------------

Please report bugs that you find to the issue tracker on GitHub. :ref:`Here's how
<add_bug_report>`.

If you know how to program you can also try to fix it yourself. You are welcome
to ask for guidance on one of the :ref:`communication channels <channels>` used
by the project.

How do I contribute to PROJ?
--------------------------------------------------------------------------------

Any contributions from the PROJ community is welcome. See :ref:`contributing` for
more details.

How do I calculate distances/directions on the surface of the earth?
--------------------------------------------------------------------------------

These are called geodesic calculations. There is a page about it here:
:ref:`geodesic`.

What is the best format for describing coordinate reference systems?
--------------------------------------------------------------------------------

A coordinate reference system (CRS) can in PROJ be described in several ways:
As PROJ strings, Well-Known Text (WKT) and as spatial reference ID's (such as EPSG
codes). Generally, WKT or SRID's are preferred over PROJ strings as they can
contain more information about a given CRS. Conversions between WKT and PROJ
strings will in most cases cause a loss of information, potentially leading to
erroneous transformations.

For compatibility reasons PROJ supports several WKT dialects
(see :option:`projinfo -o`). If possible WKT2 should be used.

Why is the axis ordering in PROJ not consistent?
--------------------------------------------------------------------------------

PROJ respects the axis ordering as it was defined by the authority in charge of
a given coordinate reference system. This is in accordance to the ISO19111 standard
:cite:`ISO19111`. Unfortunately most GIS software on the market doesn't follow this
standard. Before version 6, PROJ did not respect the standard either. This causes
some problems while the rest of the industry conforms to the standard. PROJ intends
to spearhead this effort, hopefully setting a good example for the rest of the
geospatial industry.

Customarily in GIS the first component in a coordinate tuple has been aligned with
the east/west direction and the second component with the north/south direction.
For many coordinate reference systems this is also what is defined by the authority.
There are however exceptions, especially when dealing with coordinate systems that
don't align with the cardinal directions of a compass. For example it is not
obvious which coordinate component aligns to which axis in a skewed coordinate
system with a 45 degrees angle against the north direction. Similarly, a geocentric
cartesian coordinate system usually has the z-component aligned with the rotational
axis of the earth and hence the axis points towards north. Both cases are
incompatible with the convention of always having the x-component be the east/west
axis, the y-component the north/south axis and the z-component the up/down axis.

In most cases coordinate reference systems with geodetic coordinates expect the
input ordered as latitude/longitude  (typically with the EPSG dataset), however,
internally PROJ expects an longitude/latitude ordering for all projections. This
is generally hidden for users but in a few cases it is exposed at the surface
level of PROJ, most prominently in the :program:`proj` utility which expects
longitude/latitude ordering of input date (unless :option:`proj -r` is used).

In case of doubt about the axis order of a specific CRS :program:`projinfo` is
able to provide an answer. Simply look up the CRS and examine the axis specification
of the Well-Known Text output:

.. code-block:: console

    projinfo EPSG:4326
    PROJ.4 string:
    +proj=longlat +datum=WGS84 +no_defs +type=crs

    WKT2:2019 string:
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

Why am I getting the error "Cannot find proj.db"?
--------------------------------------------------------------------------------
The file :ref:`proj.db<proj-db>` must be readable for the library to properly
function.  Like other :doc:`resource files<../resource_files>`,
it is located using a set of search
paths.  In most cases, the following paths are checked in order:

    - A path provided by the environment variable :envvar:`PROJ_LIB`.
    - A path built into PROJ as its resource installation directory
      (typically ../share/proj relative to the PROJ library).
    - The current directory.

Note that if you're using conda, activating an environment sets
:envvar:`PROJ_LIB` to a resource directory located in that environment.


What happened to PROJ.4?
--------------------------------------------------------------------------------

The first incarnation of PROJ saw the light of day in 1983. Back then it
was simply known as PROJ. Eventually a new version was released, known
as PROJ.2 in order to distinguish between the two versions. Later on both
PROJ.3 and PROJ.4 was released. By the time PROJ.4 was released the
software had matured enough that a new major version release wasn't an
immediate necessity. PROJ.4 was around for more than 25 years before it
again became time for an update. This left the project in a bit of a
conundrum regarding the name. For the majority of the life-time of the product it was known as PROJ.4, but with the release of version 5 the name
was no longer aligned with the version number. As a consequence, it was
decided to decouple the name from the version number and once again simply
call the software PROJ.

Use of name PROJ.4 is now strictly reserved for describing legacy behavior
of the software, e.g. "PROJ.4 strings" as seen in :program:`projinfo`
output.
