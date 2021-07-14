.. _leac:

********************************************************************************
Lambert Equal Area Conic
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Conical                                                  |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | leac                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/leac.png
   :width: 500 px
   :align: center
   :alt:   Lambert Equal Area Conic

   proj-string: ``+proj=leac``

Parameters
################################################################################

.. note:: All parameters are optional for the Lambert Equal Area Conic
          projection.

.. include:: ../options/lat_1.rst

.. option:: +south

    Sets the second standard parallel to 90°S. When the flag is off the second
    standard parallel is set to 90°N.

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

