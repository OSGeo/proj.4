.. _krovak:

********************************************************************************
Krovak
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Conical                                                  |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, but more accurate around Czechoslovakia          |
+---------------------+----------------------------------------------------------+
| **Alias**           | krovak                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/krovak.png
   :width: 500 px
   :align: center
   :alt:   Krovak

   proj-string: ``+proj=krovak``

Parameters
################################################################################

.. note:: All parameters are optional for the Krovak projection.

          The latitude of pseudo standard parallel is hardcoded to 78.5° and
          the ellipsoid to Bessel.

.. option:: +czech

    Reverse the sign of the output coordinates, as is tradition in the
    Czech Republic.

.. option:: +lon_0=<value>

    Longitude of projection center.

    *Defaults to 24°50' (24.8333333333333)*

.. option:: +lat_0=<value>

    Latitude of projection center.

    *Defaults to 49.5*

.. option:: +k_0=<value>

    Scale factor. Determines scale factor used in the projection.

    *Defaults to 0.9999*

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
