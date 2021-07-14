.. _gall:

********************************************************************************
Gall (Gall Stereographic)
********************************************************************************

The Gall stereographic projection, presented by James Gall in 1855, is a cylindrical projection.
It is neither equal-area nor conformal but instead tries to balance the distortion inherent in any projection.

+---------------------+----------------------------------------------------------+
| **Classification**  | Transverse and oblique cylindrical                       |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, Spherical                           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | gall                                                     |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/gall.png
   :width: 500 px
   :align: center
   :alt:   Gall (Gall Stereographic)

   proj-string: ``+proj=gall``

Usage
#####

The need for a world map which avoids some of the scale exaggeration of the Mercator projection has led to some commonly used cylindrical modifications, as well as to other modifications which are not cylindrical.
The earliest common cylindrical example was developed by James Gall of Edinburgh about 1855 (Gall, 1885, p. 119-123).
His meridians are equally spaced, but the parallels are spaced at increasing intervals away from the Equator.
The parallels of latitude are actually projected onto a cylinder wrapped about the sphere, but cutting it at lats. 45° N. and S., the point of perspective being a point on the Equator opposite the meridian being projected.
It is used in several British atlases, but seldom in the United States.
The Gall projection is neither conformal nor equal-area, but has a blend of various features.
Unlike the Mercator, the Gall shows the poles as lines running across the top and bottom of the map.


Example using Gall Stereographic  ::

    $ echo 9 51 | proj +proj=gall +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +units=m
    708432.90   5193386.36

Example using Gall Stereographic (Central meridian 90°W) ::

    $ echo 9 51 | proj +proj=gall +lon_0=90w +x_0=0 +y_0=0 +ellps=WGS84 +units=m
    7792761.91  5193386.36

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst


Mathematical definition
#######################

The formulas describing the Gall Stereographic are all taken from :cite:`Snyder1993`.

Spherical form
**************

Forward projection
==================

.. math::

   x = \frac{\lambda}{\sqrt{2}}

.. math::

   y = (1+\frac{\sqrt{2}}{2}) \tan(\phi/2)

Inverse projection
==================

.. math::

  \phi = 2 \arctan( \frac{y}{1+\frac{\sqrt{2}}{2}} )

.. math::

  \lambda = \sqrt{2} x


Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Gall_stereographic_projection>`_
#. `Cartographic Projection Procedures for the UNIX Environment-A User's Manual <http://sites.lsa.umich.edu/zhukov/wp-content/uploads/sites/140/2014/08/projection-procedures.pdf>`_
