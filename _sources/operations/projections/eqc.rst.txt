.. _eqc:

********************************************************************************
Equidistant Cylindrical (Plate Carrée)
********************************************************************************

The simplest of all projections. Standard parallels (0° when omitted) may be specified that determine latitude of true scale (k=h=1).

+---------------------+----------------------------------------------------------+
| **Classification**  | Conformal cylindrical                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse                                      |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, but best used near the equator                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | eqc                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/eqc.png
   :width: 500 px
   :align: center
   :alt:   Equidistant Cylindrical (Plate Carrée)

   proj-string: ``+proj=eqc``

Usage
########

Because of the distortions introduced by this projection, it has little use in navigation or cadastral mapping and finds its main use in thematic mapping.
In particular, the plate carrée has become a standard for global raster datasets, such as Celestia and NASA World Wind, because of the particularly simple relationship between the position of an image pixel on the map and its corresponding geographic location on Earth.

The following table gives special cases of the cylindrical equidistant projection.

+---------------------------------------------------------+--------------------------+
| Projection Name                                         | (lat ts=) :math:`\phi_0` |
+=========================================================+==========================+
| Plain/Plane Chart                                       | 0°                       |
+---------------------------------------------------------+--------------------------+
| Simple Cylindrical                                      | 0°                       |
+---------------------------------------------------------+--------------------------+
| Plate Carrée                                            | 0°                       |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum overall scale distortion          | 37°30′                   |
+---------------------------------------------------------+--------------------------+
| E.Grafarend and A.Niermann                              | 42°                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller—minimum continental scale distortion      | 43°30′                   |
+---------------------------------------------------------+--------------------------+
| Gall Isographic                                         | 45°                      |
+---------------------------------------------------------+--------------------------+
| Ronald Miller Equirectangular                           | 50°30′                   |
+---------------------------------------------------------+--------------------------+
| E.Grafarend and A.Niermann minimum linear distortion    | 61°7′                    |
+---------------------------------------------------------+--------------------------+


Example using EPSG 32662 (WGS84 Plate Carrée)::

    $ echo 2 47 | proj +proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +units=m
    222638.98       5232016.07

Example using Plate Carrée projection with true scale at latitude 30° and central meridian 90°W::

    $ echo -88 30 | proj +proj=eqc +lat_ts=30 +lon_0=90w
    192811.01       3339584.72

Parameters
################################################################################


.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/lat_ts.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

Mathematical definition
#######################

The formulas describing the Equidistant Cylindrical projection are all taken from :cite:`Snyder1987`.

:math:`\phi_{ts}` is the latitude of true scale, that mean the standard parallels where the scale of the projection is true. It can be set with ``+lat_ts``.

:math:`\phi_0` is the latitude of origin that match the center of the map. It can be set with ``+lat_0``.


Forward projection
==================

.. math::

   x = \lambda \cos \phi_{ts}

.. math::

   y = \phi - \phi_0

Inverse projection
==================

.. math::

   \lambda = x / cos \phi_{ts}

.. math::

   \phi = y + \phi_0


Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Equirectangular_projection>`_
#. `Wolfram Mathworld <http://mathworld.wolfram.com/CylindricalEquidistantProjection.html>`_


