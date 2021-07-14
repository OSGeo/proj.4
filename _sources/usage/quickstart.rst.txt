.. _quickstart:

================================================================================
Quick start
================================================================================

Coordinate transformations are defined by, what in PROJ terminology is
known as, "proj-strings". A proj-string describes any transformation regardless of
how simple or complicated it might be. The simplest case is projection of geodetic
coordinates. This section focuses on the simpler cases and introduces the basic
anatomy of the proj-string. The complex cases are discussed in
:doc:`transformation`.

A proj-strings holds the parameters of a given coordinate transformation, e.g.

::

    +proj=merc +lat_ts=56.5 +ellps=GRS80

I.e. a proj-string consists of a projection specifier, ``+proj``, a number of
parameters that applies to the projection and, if needed, a description of a
datum shift. In the example above geodetic coordinates are transformed to
projected space with the :doc:`Mercator projection<../operations/projections/merc>` with
the latitude of true scale at 56.5 degrees north on the GRS80 ellipsoid. Every
projection in PROJ is identified by a shorthand such as ``merc`` in the above
example.

By using the  above projection definition as parameters for the command line
utility :program:`proj` we can convert the geodetic coordinates to projected space:

::

    $ proj +proj=merc +lat_ts=56.5 +ellps=GRS80

If called as above :program:`proj` will be in interactive mode, letting you
type the input data manually and getting a response presented on screen.
:program:`proj` works as any UNIX filter though, which means that you can also
pipe data to the utility, for instance by using the :program:`echo` command:

::

    $ echo 55.2 12.2 | proj +proj=merc +lat_ts=56.5 +ellps=GRS80
    3399483.80      752085.60


PROJ also comes bundled with the :program:`cs2cs` utility which is used to
transform from one coordinate reference system to another. Say we want to
convert the above Mercator coordinates to UTM, we can do that with
:program:`cs2cs`:

::

    $ echo 3399483.80 752085.60 | cs2cs +proj=merc +lat_ts=56.5 +ellps=GRS80 +to +proj=utm +zone=32
    6103992.36      1924052.47 0.00

Notice the ``+to`` parameter that separates the source and destination
projection definitions.

If you happen to know the EPSG identifiers for the two coordinates reference
systems you are transforming between you can use those with :program:`cs2cs`:

::

   $ echo 56 12 | cs2cs +init=epsg:4326 +to +init=epsg:25832
   231950.54      1920310.71 0.00

In the above example we transform geodetic coordinates in the WGS84 reference
frame to UTM zone 32N coordinates in the ETRS89 reference frame.
UTM coordinates 
