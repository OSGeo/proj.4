.. _tpers:

********************************************************************************
Tilted perspective
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | tpers                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+



.. figure:: ./images/tpers.png
   :width: 500 px
   :align: center
   :alt:   Tilted perspective

   proj-string: ``+proj=tpers +h=5500000 +lat_0=40``

Tilted Perspective is similar to :ref:`nsper` (``nsper``) in that it simulates a
perspective view from a height. Where ``nsper`` projects onto a plane tangent to
the surface, Tilted Perspective orients the plane towards the direction of the
view. Thus, extra parameters specifying azimuth and tilt are required beyond
`nsper``'s ``h``. As with ``nsper``, ``lat_0`` & ``lon_0`` are
also required for satellite position.

Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. include:: ../options/h.rst

Optional
-------------------------------------------------------------------------------

.. option:: +azi=<value>

    Bearing in degrees away from north.

    *Defaults to 0.0.*

.. option:: +tilt=<value>

    Angle in degrees away from nadir.

    *Defaults to 0.0.*

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

