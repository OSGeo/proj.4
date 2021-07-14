.. _natearth2:

********************************************************************************
Natural Earth II
********************************************************************************
+---------------------+--------------------------------------------------------+
| **Classification**  | Pseudo cylindrical                                     |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection              |
+---------------------+--------------------------------------------------------+
| **Defined area**    | Global                                                 |
+---------------------+--------------------------------------------------------+
| **Alias**           | natearth2                                              |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+
| **Output type**     | Projected coordinates                                  |
+---------------------+--------------------------------------------------------+



.. figure:: ./images/natearth2.png
   :width: 500 px
   :align: center
   :alt:   Natural Earth II

   proj-string: ``+proj=natearth2``

The Natural Earth II projection is intended for making world maps. At high
latitudes, meridians bend steeply toward short pole lines resulting in a map
with highly rounded corners that resembles an elongated globe.

See :cite:`Savric2015`


Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
