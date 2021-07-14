.. _geogoffset:

================================================================================
Geographic offsets
================================================================================

.. versionadded:: 6.0.0

The Geographic offsets transformation adds an offset to the geographic longitude,
latitude coordinates, and an offset to the ellipsoidal height.

+---------------------+----------------------------------------------------------+
| **Alias**           | geogoffset                                               |
+---------------------+----------------------------------------------------------+
| **Domain**          | 3D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates (horizontal), meters (vertical)     |
+---------------------+----------------------------------------------------------+
| **output type**     | Geodetic coordinates (horizontal), meters (vertical)     |
+---------------------+----------------------------------------------------------+

This method is normally only used when low accuracy is tolerated. It is documented
as coordinate operation method code 9619 (for geographic 2D) and 9660 (for
geographic 3D) in the EPSG dataset (:cite:`IOGP2018`)

It can also be used to implement the method Geographic2D with Height Offsets
(code 9618) by noting that the input vertical component is a gravity-related
height and the output vertical component is the ellipsoid height (dh being
the geoid undulation).

It can also be used to implement the method Vertical offset (code 9616)

The reverse transformation simply consists in subtracting the offsets.

This method is a conveniency wrapper for the more general :ref:`affine`.

Examples
###############################################################################

Geographic offset from the old Greek geographic 2D CRS to the newer GGRS87 CRS::

    proj=geogoffset dlon=0.28 dlat=-5.86

Conversion from Tokyo + JSLD69 height to WGS 84::

    proj=geogoffset dlon=-13.97 dlat=7.94 dh=26.9

Conversion from Baltic 1977 height to Black Sea height::

    proj=geogoffset dh=0.4


Parameters
################################################################################

Optional
-------------------------------------------------------------------------------

.. option:: +dlon=<value>

    Offset in longitude, expressed in arc-second, to add.

.. option:: +dlat=<value>

    Offset in latitude, expressed in arc-second, to add.

.. option:: +dh=<value>

    Offset in height, expressed in meter, to add.

