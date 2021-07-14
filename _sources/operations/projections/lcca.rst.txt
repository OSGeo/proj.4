.. _lcca:

********************************************************************************
Lambert Conformal Conic Alternative
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Conical                                                  |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | lcca                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/lcca.png
   :width: 500 px
   :align: center
   :alt:   Lambert Conformal Conic Alternative

   proj-string: ``+proj=lcca +lat_0=35``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
