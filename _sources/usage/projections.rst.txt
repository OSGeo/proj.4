.. _projections_intro:

================================================================================
Cartographic projection
================================================================================

The foundation of PROJ is the large number of
:doc:`projections<../operations/projections/index>` available in the library. This section
is devoted to the generic parameters that can be used on any projection in the
PROJ library.

Below is a list of PROJ parameters which can be applied to most coordinate
system definitions. This table does not attempt to describe the parameters
particular to particular projection types. These can be found on the pages
documenting the individual :doc:`projections<../operations/projections/index>`.

    ==========   ================================================================
    Parameter    Description
    ==========   ================================================================
    +a           Semimajor radius of the ellipsoid axis
    +axis        Axis orientation
    +b           Semiminor radius of the ellipsoid axis
    +ellps       Ellipsoid name (see ``proj -le``)
    +k           Scaling factor (deprecated)
    +k_0         Scaling factor
    +lat_0       Latitude of origin
    +lon_0       Central meridian
    +lon_wrap    Center longitude to use for wrapping (see below)
    +over        Allow longitude output outside -180 to 180 range, disables
                 wrapping (see below)
    +pm          Alternate prime meridian (typically a city name, see below)
    +proj        Projection name (see ``proj -l``)
    +units       meters, US survey feet, etc.
    +vunits      vertical units.
    +x_0         False easting
    +y_0         False northing
    ==========   ================================================================

In the sections below most of the parameters are explained in details.

Units
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Horizontal units can be specified using the ``+units`` keyword with a symbolic
name for a unit (i.e. ``us-ft``).  Alternatively the translation to meters can be
specified with the ``+to_meter`` keyword (i.e. 0.304800609601219 for US feet).  The
``-lu`` argument to :program:`cs2cs` or :program:`proj` can be used to list
symbolic unit names. The default unit for projected coordinates is the meter.
A few special projections deviate from this behavior, most notably the
latlong pseudo-projection that returns degrees.

Vertical (Z) units can be specified using the ``+vunits`` keyword with a
symbolic name for a unit (i.e. ``us-ft``).  Alternatively the translation to
meters can be specified with the ``+vto_meter`` keyword (i.e. 0.304800609601219
for US feet).  The ``-lu`` argument to :program:`cs2cs` or :program:`proj` can
be used to list symbolic unit names.  If no vertical units are specified, the
vertical units will default to be the same as the horizontal coordinates.

.. note::
    :program:`proj` does not handle vertical units at all and hence the
    ``+vto_meter`` argument will be ignored.

Scaling of output units can be done by applying the ``+k_0`` argument. The
returned coordinates are scaled by the value assigned with the ``+k_0``
parameter.

False Easting/Northing
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Virtually all coordinate systems allow for the presence of a false easting
(``+x_0``) and northing (``+y_0``).  Note that these values are always expressed in
meters even if the coordinate system is some other units.  Some coordinate
systems (such as UTM) have implicit false easting and northing values.

Longitude Wrapping
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

By default PROJ wraps output longitudes in the range -180 to 180.  The ``+over``
switch can be used to disable the default wrapping which is done at a low level
in ``pj_inv()``.  This is particularly useful with projections like the
:doc:`equidistant cylindrical<../operations/projections/eqc>`
where it would be desirable for X values past -20000000 (roughly) to continue
past -180 instead of wrapping to +180.

The ``+lon_wrap`` option can be used to provide an alternative means of doing
longitude wrapping within ``pj_transform()``.  The argument to this option is a
center longitude.  So ``+lon_wrap=180`` means wrap longitudes in the range 0 to
360.  Note that ``+over`` does **not** disable ``+lon_wrap``.

Prime Meridian
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

A prime meridian may be declared indicating the offset between the prime
meridian of the declared coordinate system and that of greenwich.  A prime
meridian is declared using the "pm" parameter, and may be assigned a symbolic
name, or the longitude of the alternative prime meridian relative to greenwich.

Currently prime meridian declarations are only utilized by the
``pj_transform()`` API call, not the ``pj_inv()`` and ``pj_fwd()`` calls.
Consequently the user utility :program:`cs2cs` does honour prime meridians but
the :program:`proj` user utility ignores them.

The following predeclared prime meridian names are supported.  These can be
listed using with ``cs2cs -lm``.

 ===========     ================
 Meridian        Longitude
 ===========     ================
   greenwich     0dE
      lisbon     9d07'54.862"W
       paris     2d20'14.025"E
      bogota     74d04'51.3"E
      madrid     3d41'16.48"W
        rome     12d27'8.4"E
        bern     7d26'22.5"E
     jakarta     106d48'27.79"E
       ferro     17d40'W
    brussels     4d22'4.71"E
   stockholm     18d3'29.8"E
      athens     23d42'58.815"E
        oslo     10d43'22.5"E
 ===========     ================

Example of use.  The location ``long=0``, ``lat=0`` in the greenwich based lat/long
coordinates is translated to lat/long coordinates with Madrid as the prime
meridian.

::

    cs2cs +proj=latlong +datum=WGS84 +to +proj=latlong +datum=WGS84 +pm=madrid
    0 0
    3d41'16.48"E    0dN 0.000


Axis orientation
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Starting in PROJ 4.8.0, the +axis argument can be used to control the axis
orientation of the coordinate system.  The default orientation is "easting,
northing, up" but directions can be flipped, or axes flipped using combinations
of the axes in the +axis switch.  The values are:

* "e" - Easting
* "w" - Westing
* "n" - Northing
* "s" - Southing
* "u" - Up
* "d" - Down

They can be combined in +axis in forms like:

* ``+axis=enu`` - the default easting, northing, elevation.
* ``+axis=neu`` - northing, easting, up - useful for "lat/long" geographic
  coordinates, or south orientated transverse mercator.
* ``+axis=wnu`` - westing, northing, up - some planetary coordinate systems
  have "west positive" coordinate systems

.. note::

    The ``+axis`` argument does not work with the :program:`proj` command line
    utility.


