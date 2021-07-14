.. _sinu:

********************************************************************************
Sinusoidal (Sanson-Flamsteed)
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Pseudocylindrical                                        |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | sinu                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/sinu.png
   :width: 500 px
   :align: center
   :alt:   Sinusoidal (Sanson-Flamsteed)

   proj-string: ``+proj=sinu``

MacBryde and Thomas developed generalized formulas for several of the
pseudocylindricals with sinusoidal meridians:

.. math::

    x = C\lambda(m+cos\theta) / ( m + 1)

.. math::
    y = C\theta

.. math::

    C = \sqrt { (m + 1 ) / n }

Parameters
################################################################################

.. note:: All parameters are optional for the Sinusoidal projection.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
