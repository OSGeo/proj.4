.. _ocea:

********************************************************************************
Oblique Cylindrical Equal Area
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical                                              |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | ocea                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/ocea.png
   :width: 500 px
   :align: center
   :alt:   Oblique Cylindrical Equal Area

   proj-string: ``+proj=ocea``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

For the Oblique Cylindrical Equal Area projection a pole of rotation is needed.
The pole can be defined in two ways: By a point and azimuth or by providing to
points that make up the pole.

Point & azimuth
................................................................................

.. option:: +lonc=<value>

    Longitude of rotational pole point.

.. option:: +alpha=<value>

    Angle of rotational pole.

Two points
................................................................................

.. option:: +lon_1=<value>

    Longitude of first point.

.. option:: +lat_1=<value>

    Latitude of first point.

.. option:: +lon_2=<value>

    Longitude of second point.

.. option:: +lat_2=<value>

    Latitude of second point.

Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/k_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
