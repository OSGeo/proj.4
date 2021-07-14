.. _tobmerc:

********************************************************************************
Tobler-Mercator
********************************************************************************

.. versionadded:: 6.0.0

Equal area cylindrical projection with the same latitudinal spacing as
Mercator projection.

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical equal area                                   |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical only                      |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, conventionally truncated at about 80 degrees     |
|                     | north and south                                          |
+---------------------+----------------------------------------------------------+
| **Alias**           | tobmerc                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+

.. figure:: ./images/tobmerc.png
   :width: 500 px
   :align: center
   :alt:   Tobler-Mercator

   proj-string: ``+proj=tobmerc``

Usage
#####

The inappropriate use of the Mercator projection has declined but still
occasionally occurs. One method of contrasting the Mercator projection is to
present an alternative in the form of an equal area projection. The map
projection derived here is thus not simply a pretty Christmas tree ornament:
it is instead a complement to Mercator's conformal navigation anamorphosis
and can be displayed as an alternative. The equations for the new map
projection preserve the latitudinal stretching of the Mercator while
adjusting the longitudinal spacing. This allows placement of the new map
adjacent to that of Mercator. The surface area, while drastically warped,
maintains the correct magnitude.

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/k_0.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/R.rst

Mathematical definition
#######################

The formulas describing the Tobler-Mercator are taken from Waldo Tobler's
article :cite:`Tobler2018`

Spherical form
**************
For the spherical form of the projection we introduce the scaling factor:

.. math::

    k_0 = \cos^2 \phi_{ts}

Forward projection
==================

.. math::

    x = k_0 \lambda

.. math::

    y = k_0 \ln \left[ \tan \left(\frac{\pi}{4} + \frac{\phi}{2} \right) \right]


Inverse projection
==================

.. math::

    \lambda = \frac{x}{k_0}

.. math::

    \phi = \frac{\pi}{2} - 2 \arctan \left[ e^{-y/k_0} \right]
