.. _gstmerc:

********************************************************************************
Gauss-Schreiber Transverse Mercator (aka Gauss-Laborde Reunion)
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | gstmerc                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/gstmerc.png
   :width: 500 px
   :align: center
   :alt:   Gauss-Schreiber Transverse Mercator (aka Gauss-Laborde Reunion)

   proj-string: ``+proj=gstmerc``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. include:: ../options/k_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
