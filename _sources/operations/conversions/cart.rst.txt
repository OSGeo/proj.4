.. _cart:

================================================================================
Geodetic to cartesian conversion
================================================================================

.. versionadded:: 5.0.0

Convert geodetic coordinates to cartesian coordinates (in the forward path).

+---------------------+--------------------------------------------------------+
| **Alias**           | cart                                                   |
+---------------------+--------------------------------------------------------+
| **Domain**          | 3D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+
| **Output type**     | Geocentric cartesian coordinates                       |
+---------------------+--------------------------------------------------------+

This conversion converts geodetic coordinate values (longitude, latitude,
elevation above ellipsoid) to their geocentric (X, Y, Z) representation, where
the first axis (X) points from the Earth centre to the point of longitude=0,
latitude=0, the second axis (Y) points from the Earth centre to the point of
longitude=90, latitude=0 and the third axis (Z) points to the North pole.

Usage
################################################################################

Convert geodetic coordinates to GRS80 cartesian coordinates::

    echo 17.7562015132 45.3935192042 133.12 2017.8 | cct +proj=cart +ellps=GRS80
    4272922.1553   1368283.0597  4518261.3501     2017.8000

Parameters
################################################################################

.. include:: ../options/ellps.rst
