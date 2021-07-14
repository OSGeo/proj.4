.. _comill:

********************************************************************************
Compact Miller
********************************************************************************

The Compact Miller projection is a cylindrical map projection with a
height-to-width ratio of 0.6:1.

See :cite:`Jenny2015`

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical                                              |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | comill                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+

.. figure:: ./images/comill.png
   :width: 500 px
   :align: center
   :alt:   Compact Miller  

   proj-string: ``+proj=comill``

Parameters
################################################################################

.. note:: All parameters are optional for projection.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
