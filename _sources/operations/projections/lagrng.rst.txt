.. _lagrng:

********************************************************************************
Lagrange
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | lagrng                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/lagrng.png
   :width: 500 px
   :align: center
   :alt:   Lagrange

   proj-string: ``+proj=lagrng``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. option:: +W=<value>

    The factor :option:`+W` is the ratio of the difference in longitude from the
    central meridian to the a circular meridian to 90. :option:`+W` must be a
    positive value.

    *Defaults to 2.0*

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_1.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
