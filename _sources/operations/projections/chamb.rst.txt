.. _chamb:

********************************************************************************
Chamberlin Trimetric
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward spherical projection                             |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | chamb                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/chamb.png
   :width: 500 px
   :align: center
   :alt:   Chamberlin Trimetric

   proj-string: ``+proj=chamb +lat_1=10 +lon_1=30 +lon_2=40``

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. note:: Control points should be oriented clockwise.

.. option:: +lat_1=<value>

    Latitude of the first control point.

.. option:: +lon_1=<value>

    Longitude of the first control point.

.. option:: +lat_2=<value>

    Latitude of the second control point.

.. option:: +lon_2=<value>

    Longitude of the second control point.

.. option:: +lat_3=<value>

    Latitude of the third control point.

.. option:: +lon_3=<value>

    Longitude of the third control point.

Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
