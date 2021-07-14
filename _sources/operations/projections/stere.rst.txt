.. _stere:

********************************************************************************
Stereographic
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | stere                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/stere.png
   :width: 500 px
   :align: center
   :alt:   Stereographic

   proj-string: ``+proj=stere +lat_0=90 +lat_ts=75``

Parameters
################################################################################

.. note:: All parameters are optional for the projection.

.. include:: ../options/lat_0.rst

.. option:: +lat_ts=<value>

    Defines the latitude where scale is not distorted. It is only taken into
    account for Polar Stereographic formulations (``+lat_0`` = +/- 90 ), and
    then defaults to the ``+lat_0`` value.
    If set to a value different from +/- 90, it takes precedence over ``+k_0``
    if both options are used together.

.. include:: ../options/k_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
