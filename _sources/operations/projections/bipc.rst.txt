.. _bipc:

********************************************************************************
Bipolar conic of western hemisphere
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse spherical projection                 |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | bipc                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/bipc.png
   :width: 500 px
   :align: center
   :alt:   Bipolar conic of western hemisphere

   proj-string: ``+proj=bipc +ns``

Parameters
################################################################################

.. note:: All options are optional for the Bipolar Conic projection.

.. option:: +ns

    Return non-skewed cartesian coordinates.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
