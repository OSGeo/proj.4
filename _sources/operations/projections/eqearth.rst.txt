.. _eqearth:

********************************************************************************
Equal Earth
********************************************************************************

.. versionadded:: 5.2.0

+---------------------+-----------------------------------------------------------+
| **Classification**  | Pseudo cylindrical                                        |
+---------------------+-----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal projection |
+---------------------+-----------------------------------------------------------+
| **Defined area**    | Global                                                    |
+---------------------+-----------------------------------------------------------+
| **Alias**           | eqearth                                                   |
+---------------------+-----------------------------------------------------------+
| **Domain**          | 2D                                                        |
+---------------------+-----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                      |
+---------------------+-----------------------------------------------------------+
| **Output type**     | Projected coordinates                                     |
+---------------------+-----------------------------------------------------------+



.. figure:: ./images/eqearth.png
   :width: 500 px
   :align: center
   :alt:   Equal Earth

   proj-string: ``+proj=eqearth``


The Equal Earth projection is intended for making world maps. Equal Earth is a
projection inspired by the Robinson projection, but unlike
the Robinson projection retains the relative size of areas. The projection
was designed in 2018 by Bojan Savric, Tom Patterson and Bernhard Jenny :cite:`Savric2018`.


Usage
###############################################################################

The Equal Earth projection has no special options. Here is
an example of an forward projection on a sphere with a radius of 1 m::

    $ echo 122 47 | src/proj +proj=eqearth +R=1
    1.55    0.89

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
################################################################################

#. Bojan Savric, Tom Patterson & Bernhard Jenny (2018). `The Equal Earth map projection <https://www.researchgate.net/profile/Bojan_Savric2/publication/326879978_The_Equal_Earth_map_projection/links/5b69d0ae299bf14c6d951b77/The-Equal-Earth-map-projection.pdf>`_, International Journal of Geographical Information Science
