.. _resource_files:

================================================================================
Resource files
================================================================================

A number of files containing preconfigured transformations and default parameters
for certain projections are bundled with the PROJ distribution. Init files
contain preconfigured proj-strings for various coordinate reference systems
and the `defaults` file contains default values for parameters of select
projections.

In addition to the bundled init files the PROJ project also distributes a number
of packages containing transformation grids and additional init files not included
in the main PROJ package.

.. _resource_file_paths:

Where are PROJ resource files looked for ?
-------------------------------------------------------------------------------

PROJ will attempt to locate its resource files - database, transformation grids
or init files - from several directories.
The following paths are checked in order:

- For resource files that have an explicit relative or absolute path,
  the directory specified in the filename.

- Path resolved by the callback function set with
  the :c:func:`proj_context_set_file_finder`. If it is set, the next tests
  will not be run.

- Path(s) set with the :c:func:`proj_context_set_search_paths`. If set, the
  next tests will not be run.

.. _user_writable_directory:

- .. versionadded:: 7.0

  The PROJ user writable directory, which is :

    * on Windows, ``${LOCALAPPDATA}/proj``
    * on macOS, ``${HOME}/Library/Application Support/proj``
    * on other platforms (Linux), ``${XDG_DATA_HOME}/proj`` if
      :envvar:`XDG_DATA_HOME` is defined. Else ``${HOME}/.local/share/proj``

- Path(s) set with by the environment variable :envvar:`PROJ_LIB`.
  On Linux/macOS/Unix, use ``:`` to separate paths. On Windows, ``;``

- .. versionadded:: 7.0

  The :file:`../share/proj/` and its contents are found automatically
  at run-time if the installation respects the build structure. That is, the
  binaries and :file:`proj.dll`/:file:`libproj.so` are installed under
  :file:`../bin/` or :file:`../lib/`, and resource files are in
  :file:`../share/proj/`.

- A path built into PROJ as its resource installation directory (whose value is
  ``$(pkgdatadir)`` for builds using the Makefile build system or
  ``${CMAKE_INSTALL_PREFIX}/${DATADIR}`` for CMake builds). Note, however,
  that since this is a hard-wired path setting, it only works if the whole
  PROJ installation is not moved somewhere else.

  .. note:: if PROJ is built with the ``PROJ_LIB_ENV_VAR_TRIED_LAST`` CMake option /
            ``--enable-proj-lib-env-var-tried-last`` configure switch, then this
            hard-wired path will be tried before looking at the environment
            variable :envvar:`PROJ_LIB`.

- The current directory

When networking capabilities are enabled, either by API with the
:c:func:`proj_context_set_enable_network` function or when the
:envvar:`PROJ_NETWORK` environment variable is set to ``ON``, PROJ will
attempt to use remote grids stored on CDN (Content Delivery Network) storage.

.. _proj-db:

:file:`proj.db`
-------------------------------------------------------------------------------

A proj installation includes a SQLite database of transformation information
that must be accessible for the library to work properly.  The library will
print an error if the database can't be found.

.. _proj-ini:

:file:`proj.ini`
-------------------------------------------------------------------------------

.. versionadded:: 7.0

:file:`proj.ini` is a text configuration file, mostly dedicated at setting up
network related parameters.

Its default content is:

::

    [general]
    ; Lines starting by ; are commented lines.
    ;

    ; Network capabilities disabled by default.
    ; Can be overridden with the PROJ_NETWORK=ON environment variable.
    ; network = on

    ; Can be overridden with the PROJ_NETWORK_ENDPOINT environment variable.
    cdn_endpoint = https://cdn.proj.org

    cache_enabled = on

    cache_size_MB = 300

    cache_ttl_sec = 86400

    ; Transverse Mercator (and UTM) default algorithm: auto, evenden_snyder or poder_engsager
    ; * evenden_snyder is the fastest, but less accurate far from central meridian
    ; * poder_engsager is slower, but more accurate far from central meridian
    ; * default will auto-select between the two above depending on the coordinate
    ;   to transform and will use evenden_snyder if the error in doing so is below
    ;   0.1 mm (for an ellipsoid of the size of Earth)
    tmerc_default_algo = poder_engsager


Transformation grids
-------------------------------------------------------------------------------

Grid files are important for shifting and transforming between datums.

PROJ supports CTable2, NTv1 and NTv2 files for horizontal grid corrections and
the GTX file format for vertical corrections. Details about the formats can be
found in the `GDAL documentation <http://gdal.org/>`_. GDAL reads and writes
all formats. Using GDAL for construction of new grids is recommended.

.. _datumgrid:

External resources and packaged grids
-------------------------------------------------------------------------------

proj-data
+++++++++

The ``proj-data`` package is a collection of all the resource files that are
freely available for use with PROJ. The package is maintained on
`GitHub <https://github.com/OSGeo/PROJ-data>`_ and the contents of the package
are show-cased on the `PROJ CDN <https://cdn.proj.org/>`_. The contents of the
package can be installed using the :program:`projsync` package or by downloading
the zip archive of the package and unpacking in the :envvar:`PROJ_LIB` directory.

proj-datumgrid
++++++++++++++

.. note::

    The packages described below can be used with PROJ 7 and later but are
    primarily meant to be used with PROJ 6 and earlier versions.
    The ``proj-datumgrid`` series of packages are not maintained anymore and
    are only kept available for legacy purposes.

For a functioning build of PROJ prior to version 7, installation of the
`proj-datumgrid <https://github.com/OSGeo/proj-datumgrid>`_ is needed. If you
have installed PROJ from a package system chances are that this will already be
done for you. The *proj-datumgrid* package provides transformation grids that
are essential for many of the predefined transformations in PROJ. Which grids
are included in the package can be seen on the
`proj-datumgrid repository <https://github.com/OSGeo/proj-datumgrid>`_ as well
as descriptions of those grids. This is the main grid package and the only one
that is required. It includes various older grids that is mostly needed for
legacy reasons. Without this package, the test suite fails miserably.

Regional packages
+++++++++++++++++

In addition to the default ``proj-datumgrid`` package regional packages are also
distributed. These include grids and init files that are valid within the given
region. The packages are divided into geographical regions in order to keep the
needed disk space by PROJ at a minimum. Some users may have a use for resource
files covering several regions in which case they can download more than one.

At the moment three regional resource file packages are distributed:

* `Europe <https://github.com/OSGeo/proj-datumgrid/tree/master/europe#proj-datumgrid-europe>`_
* `Oceania <https://github.com/OSGeo/proj-datumgrid/tree/master/oceania#proj-datumgrid-oceania>`_
* `North America <https://github.com/OSGeo/proj-datumgrid/tree/master/north-america#proj-datumgrid-north-america>`_

If someone supplies grids relevant for Africa, South-America, Asia or Antarctica
we will create new regional packages.

Click the links to jump to the relevant README files for each package. Details
on the content of the packages maintained there.

.. tip::
   To download the various datumgrid packages head to the :ref:`download section <download>`.

World package
+++++++++++++

The `world package <https://github.com/OSGeo/proj-datumgrid/tree/master/world#proj-datumgrid-world>`_
includes grids that have global extent, e.g. the global geoid model EGM08.

-latest packages
++++++++++++++++

All packages above come in different versions, e.g., ``proj-datumgrid-1.8`` or
``proj-datumgrid-europe-1.4``. The ``-latest`` packages are symbolic links to the
latest version of a given package. That means that the link
https://download.osgeo.org/proj/proj-datumgrid-north-america-latest.zip is
equivalent to https://download.osgeo.org/proj/proj-datumgrid-north-america-1.2.zip
(as of the time of writing this).

.. _transformation_grids:

Other transformation grids
-------------------------------------------------------------------------------

Below is a list of grid resources for various countries which are not
included in the grid distributions mentioned above.

Free grids
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The following is a list of grids distributed under a free and open license.

Hungary
................................................................................

`Hungarian grid <https://github.com/OSGeoLabBp/eov2etrs/>`__ ETRS89 - HD72/EOV (epsg:23700), both horizontal and elevation grids


.. _nonfreegrids:

Non-Free Grids
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Not all grid shift files have licensing that allows them to be freely
distributed, but can be obtained by users through free and legal methods.

Austria
................................................................................

Overview of `Austrian grids <http://www.bev.gv.at/portal/page?_pageid=713,1569828&_dad=portal&_schema=PORTAL>`__ and other resources
related to the local geodetic reference.

Brazil
................................................................................

`Brazilian grids <http://www.ibge.gov.br/home/geociencias/geodesia/param_transf/default_param_transf.shtm>`__ for datums Corrego Alegre 1961, Corrego Alegre 1970-72, SAD69 and SAD69(96)

Netherlands
................................................................................

`Dutch grid <https://www.nsgi.nl/geodetische-infrastructuur/coordinatentransformatie>`__ (Registration required before download)

Portugal
................................................................................

`Portuguese grids <http://www.fc.up.pt/pessoas/jagoncal/coordenadas/index.htm>`__ for ED50, Lisbon 1890, Lisbon 1937 and Datum 73

South Africa
................................................................................

`South African grid <http://eepublishers.co.za/article/datum-transformations-using-the-ntv2-grid.html>`__ (Cape to Hartebeesthoek94 or WGS84)

Spain
................................................................................

`Spanish grids <http://www.ign.es/ign/layoutIn/herramientas.do#DATUM>`__ for ED50.


HTDP
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This section describes the use of the :file:`crs2crs2grid.py` script and the HTDP
(Horizontal Time Dependent Positioning) grid shift modelling program from
NGS/NOAA to produce PROJ compatible grid shift files for fine grade
conversions between various NAD83 epochs and WGS84.  Traditionally PROJ has
treated NAD83 and WGS84 as equivalent and failed to distinguish between
different epochs or realizations of those datums.  At the scales of much
mapping this is adequate but as interest grows in high resolution imagery and
other high resolution mapping this is inadequate.  Also, as the North American
crust drifts over time the displacement between NAD83 and WGS84 grows (more
than one foot over the last two decades).

Getting and building HTDP
................................................................................

The HTDP modelling program is written in FORTRAN.  The source and documentation
can be found on the HTDP page at http://www.ngs.noaa.gov/TOOLS/Htdp/Htdp.shtml

On Linux systems it will be necessary to install GFortran or some Fortran
compiler.  For Ubuntu something like the following should work.

::

    apt-get install gfortran

To compile the program do something like the following to produce the binary
:program:`htdp` from the source code.

::

    gfortran htdp.for -o htdp

Getting :file:`crs2crs2grid.py`
................................................................................

The :file:`crs2crs2grid.py` script can be found at
https://github.com/OSGeo/gdal/tree/master/gdal/swig/python/samples/crs2crs2grid.py

The script depends on having the GDAL Python bindings operational; if they are not you
will get an error such as:


::

    Traceback (most recent call last):
      File "./crs2crs2grid.py", line 37, in <module>
        from osgeo import gdal, gdal_array, osr
    ImportError: No module named osgeo

Usage
................................................................................

::

    crs2crs2grid.py
            <src_crs_id> <src_crs_date> <dst_crs_id> <dst_crs_year>
            [-griddef <ul_lon> <ul_lat> <ll_lon> <ll_lat> <lon_count> <lat_count>]
            [-htdp <path_to_exe>] [-wrkdir <dirpath>] [-kwf]
            -o <output_grid_name>

     -griddef: by default the following values for roughly the continental USA
               at a six minute step size are used:
               -127 50 -66 25 251 611
     -kwf: keep working files in the working directory for review.

::

    crs2crs2grid.py 29 2002.0 8 2002.0 -o nad83_2002.ct2

The goal of :file:`crs2crs2grid.py` is to produce a grid shift file for a designated
region.  The region is defined using the ``-griddef`` switch.  When missing a
continental US region is used.  The script creates a set of sample points for
the grid definition, runs :program:`htdp` against it and then parses the
resulting points and computes a point by point shift to encode into the final
grid shift file.  By default it is assumed that :program:`htdp` is in the
executable path.  If not, please provide the path to the executable using the
``-htdp`` switch.

The :program:`htdp` program supports transformations between many CRSes and for each (or
most?) of them you need to provide a date at which the CRS is fixed.  The full
set of CRS Ids available in the HTDP program are:

::

  1...NAD_83(2011) (North America tectonic plate fixed)
  29...NAD_83(CORS96)  (NAD_83(2011) will be used)
  30...NAD_83(2007)    (NAD_83(2011) will be used)
  2...NAD_83(PA11) (Pacific tectonic plate fixed)
  31...NAD_83(PACP00)  (NAD 83(PA11) will be used)
  3...NAD_83(MA11) (Mariana tectonic plate fixed)
  32...NAD_83(MARP00)  (NAD_83(MA11) will be used)

  4...WGS_72                             16...ITRF92
  5...WGS_84(transit) = NAD_83(2011)     17...ITRF93
  6...WGS_84(G730) = ITRF92              18...ITRF94 = ITRF96
  7...WGS_84(G873) = ITRF96              19...ITRF96
  8...WGS_84(G1150) = ITRF2000           20...ITRF97
  9...PNEOS_90 = ITRF90                  21...IGS97 = ITRF97
 10...NEOS_90 = ITRF90                   22...ITRF2000
 11...SIO/MIT_92 = ITRF91                23...IGS00 = ITRF2000
 12...ITRF88                             24...IGb00 = ITRF2000
 13...ITRF89                             25...ITRF2005
 14...ITRF90                             26...IGS05 = ITRF2005
 15...ITRF91                             27...ITRF2008
                                         28...IGS08 = ITRF2008

The typical use case is mapping from NAD83 on a particular date to WGS84 on
some date.  In this case the source CRS Id "29" (NAD_83(CORS96)) and the
destination CRS Id is "8 (WGS_84(G1150)).  It is also necessary to select the
source and destination date (epoch).  For example:

::

    crs2crs2grid.py 29 2002.0 8 2002.0 -o nad83_2002.ct2

The output is a CTable2 format grid shift file suitable for use with PROJ
(4.8.0 or newer).  It might be utilized something like:


::

    cs2cs +proj=latlong +ellps=GRS80 +nadgrids=./nad83_2002.ct2 +to +proj=latlong +datum=WGS84

See Also
................................................................................

* http://www.ngs.noaa.gov/TOOLS/Htdp/Htdp.shtml - NGS/NOAA page about the HTDP
  model and program.  Source for the HTDP program can be downloaded from here.


.. _init_files:

Init files
-------------------------------------------------------------------------------

Init files are used for preconfiguring proj-strings for often used
transformations, such as those found in the EPSG database. Most init files contain
transformations from a given coordinate reference system to WGS84. This makes
it easy to transform between any two coordinate reference systems with
:program:`cs2cs`. Init files can however contain any proj-string and don't necessarily
have to follow the *cs2cs* paradigm where WGS84 is used as a pivot datum. The
ITRF init file is a good example of that.

A number of init files come pre-bundled with PROJ but it is also possible to
add your own custom init files. PROJ looks for the init files in the directory
listed in the :envvar:`PROJ_LIB` environment variable.

The format of init files is an identifier in angled brackets and a
proj-string:

::

    <3819> +proj=longlat +ellps=bessel
           +towgs84=595.48,121.69,515.35,4.115,-2.9383,0.853,-3.408 +no_defs <>

The above example is the first entry from the ``epsg`` init file. So, this is the
coordinate reference system with ID 3819 in the EPSG database. Comments can be
inserted by prefixing them with a "#". With version 4.10.0 a new special metadata
entry is now accepted in init files. It can be parsed with a function from the public
API. The metadata entry in the epsg init file looks like this at the time of writing:

::

<metadata> +version=9.0.0 +origin=EPSG +lastupdate=2017-01-10

Pre-configured proj-strings from init files are used in the following way:

::

    $ cs2cs -v +proj=latlong +to +init=epsg:3819
    # ---- From Coordinate System ----
    #Lat/long (Geodetic alias)
    #
    # +proj=latlong +ellps=WGS84
    # ---- To Coordinate System ----
    #Lat/long (Geodetic alias)
    #
    # +init=epsg:3819 +proj=longlat +ellps=bessel
    # +towgs84=595.48,121.69,515.35,4.115,-2.9383,0.853,-3.408 +no_defs

It is possible to override parameters when using ``+init``. Just add the parameter
to the proj-string alongside the ``+init`` parameter. For instance by overriding
the ellipsoid as in the following example

::

    +init=epsg:25832 +ellps=intl

where the Hayford ellipsoid is used instead of the predefined GRS80 ellipsoid.
It is also possible to add additional parameters not specified in the init file,
for instance by adding an observation epoch when transforming from ITRF2000 to
ITRF2005:

::

    +init=ITRF2000:ITRF2005 +t_obs=2010.5

which then expands to

::

    +proj=helmert +x=-0.0001 +y=0.0008 +z=0.0058 +s=-0.0004
    +dx=0.0002 +dy=-0.0001 +dz=0.0018 +ds=-0.000008
    +t_epoch=2000.0 +convention=position_vector
    +t_obs=2010.5

Below is a list of the init files that are packaged with PROJ.

    ========    ================================================================
    Name        Description
    ========    ================================================================
    GL27        Great Lakes Grids
    ITRF2000    Full set of transformation parameters between ITRF2000 and other
                ITRF's
    ITRF2008    Full set of transformation parameters between ITRF2008 and other
                ITRF's
    ITRF2014    Full set of transformation parameters between ITRF2014 and other
                ITRF's
    nad27       State plane coordinate systems, North American Datum 1927
    nad83       State plane coordinate systems, North American Datum 1983
    ========    ================================================================
