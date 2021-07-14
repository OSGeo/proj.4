.. _nsper:

********************************************************************************
Near-sided perspective
********************************************************************************

The near-sided perspective projection simulates a view from a height
:math:`h` similar to how a satellite in orbit would see it.

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal. Neither conformal nor equal area.             |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse spherical projection                 |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, although for one hemisphere at a time.           |
+---------------------+----------------------------------------------------------+
| **Alias**           | nsper                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/nsper.png
   :width: 500 px
   :align: center
   :alt:   Near-sided perspective

   proj-string: ``+proj=nsper +h=3000000 +lat_0=-20 +lon_0=145``

Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. include:: ../options/h.rst

Optional
-------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
