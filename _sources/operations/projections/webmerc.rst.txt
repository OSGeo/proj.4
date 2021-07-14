.. _webmerc:

********************************************************************************
Web Mercator / Pseudo Mercator
********************************************************************************

.. versionadded:: 5.1.0

The Web Mercator / Pseudo Mercator projection is a cylindrical map projection.
This is a variant of the regular :ref:`merc` projection, except that the computation
is done on a sphere, using the semi-major axis of the ellipsoid.

From `Wikipedia <https://en.wikipedia.org/wiki/Web_Mercator>`_:

    This projection is widely used by the Web Mercator, Google Web Mercator,
    Spherical Mercator, WGS 84 Web Mercator[1] or WGS 84/Pseudo-Mercator is a
    variant of the Mercator projection and is the de facto standard for Web
    mapping applications. [...]
    It is used by virtually all major online map providers [...]
    Its official EPSG identifier is EPSG:3857, although others have been used
    historically.


+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical (non conformant if used with ellipsoid)      |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse                                      |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | webmerc                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


Usage
########

Example::

    $ echo 2 49 | proj +proj=webmerc +datum=WGS84
    222638.98       6274861.39

Parameters
################################################################################

.. note:: All parameters for the projection are optional, except the ellipsoid
          definition, which is WGS84 for the typical use case of EPSG:3857.
          In which case, the other parameters are set to their default 0 value.

.. include:: ../options/ellps.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Mathematical definition
#######################

The formulas describing the Mercator projection are all taken from G. Evenden's libproj manuals :cite:`Evenden2005`.

Forward projection
==================

.. math::

    x = \lambda

.. math::

    y = \ln \left[ \tan \left(\frac{\pi}{4} + \frac{\phi}{2} \right) \right]


Inverse projection
==================

.. math::

    \lambda = {x}

.. math::

    \phi = \frac{\pi}{2} - 2 \arctan \left[ e^{-y} \right]



Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Web_Mercator>`_



