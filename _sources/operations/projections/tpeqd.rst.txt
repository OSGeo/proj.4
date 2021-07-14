.. _tpeqd:

********************************************************************************
Two Point Equidistant
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | tpeqd                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/tpeqd.png
   :width: 500 px
   :align: center
   :alt:   Two Point Equidistant

   proj-string: ``+proj=tpeqd +lat_1=60 +lat_2=65``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. option:: +lon_1=<value>

    Longitude of first point.

.. option:: +lat_1=<value>

    Latitude of first point.

.. option:: +lon_2=<value>

    Longitude of second point.

.. option:: +lat_2=<value>

    Latitude of second point.

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
