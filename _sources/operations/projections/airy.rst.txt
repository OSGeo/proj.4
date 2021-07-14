.. _airy:

********************************************************************************
Airy
********************************************************************************

The Airy projection is an azimuthal minimum error projection for the region
within the small or great circle defined by an angular distance,
:math:`\phi_b`, from the tangency point of the plane :math:`( \lambda_0, \phi_0 )`.

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward spherical projection                             |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | airy                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+



.. figure:: ./images/airy.png
   :width: 500 px
   :align: center
   :alt:   Airy

   proj-string: ``+proj=airy``

Options
################################################################################

.. option:: +lat_b

    Angular distance from tangency point of the plane :math:`( \lambda_0, \phi_0 )`
    where the error is kept at minimum.

    *Defaults to 90° (suitable for hemispherical maps).*

.. option:: +no_cut

    Do not cut at hemisphere limit

.. include:: ../options/lat_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/R.rst
