.. _hammer:

********************************************************************************
Hammer & Eckert-Greifendorff
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | hammer                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/hammer.png
   :width: 500 px
   :align: center
   :alt:   Hammer & Eckert-Greifendorff

   proj-string: ``+proj=hammer``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. option:: +W=<value>

    Set to 0.5 for the Hammer projection and 0.25 for the Eckert-Greifendorff
    projection. :option:`+W` has to be larger than zero.

    *Defaults to 0.5.*

.. option:: +M=<value>

    :option:`+M` has to be larger than zero.

    *Defaults to 1.0.*

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
