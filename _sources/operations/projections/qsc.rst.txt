.. _qsc:

********************************************************************************
Quadrilateralized Spherical Cube
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Azimuthal                                                |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, ellipsoidal                         |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | qsc                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


The purpose of the Quadrilateralized Spherical Cube (QSC) projection is to project
a sphere surface onto the six sides of a cube:

.. image:: ../../../images/qsc_concept.jpg
   :width: 500 px
   :align: center
   :alt:   Quadrilateralized Spherical Cube

For this purpose, other alternatives can be used, notably :ref:`gnom` or
:ref:`healpix`. However, QSC projection has the following favorable properties:

It is an equal-area projection, and at the same time introduces only limited angular
distortions. It treats all cube sides equally, i.e. it does not use different
projections for polar areas and equatorial areas. These properties make QSC
projection a good choice for planetary-scale terrain rendering. Map data can be
organized in quadtree structures for each cube side. See :cite:`LambersKolb2012` for an example.

The QSC projection was introduced by :cite:`ONeilLaubscher1976`,
building on previous work by :cite:`ChanONeil1975`. For clarity: The
earlier QSC variant described in :cite:`ChanONeil1975` became known as the COBE QSC since it
was used by the NASA Cosmic Background Explorer (COBE) project; it is an approximately
equal-area projection and is not the same as the QSC projection.

See also :cite:`CalabrettaGreisen2002` Sec. 5.6.2 and 5.6.3 for a description of both and
some analysis.

In this implementation, the QSC projection projects onto one side of a circumscribed
cube. The cube side is selected by choosing one of the following six projection centers:

+-------------------------+--------------------+
| ``+lat_0=0 +lon_0=0``   | front cube side    |
+-------------------------+--------------------+
| ``+lat_0=0 +lon_0=90``  | right cube side    |
+-------------------------+--------------------+
| ``+lat_0=0 +lon_0=180`` | back cube side     |
+-------------------------+--------------------+
| ``+lat_0=0 +lon_0=-90`` | left cube side     |
+-------------------------+--------------------+
| ``+lat_0=90``           | top cube side      |
+-------------------------+--------------------+
| ``+lat_0=-90``          | bottom cube side   |
+-------------------------+--------------------+

Furthermore, this implementation allows the projection to be applied to ellipsoids.
A preceding shift to a sphere is performed automatically; see :cite:`LambersKolb2012` for details.


Usage
###############################################################################

The following example uses QSC projection via GDAL to create the six cube side
maps from a world map for the WGS84 ellipsoid::

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84  +lat_0=0 +lon_0=0"        \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff frontside.tiff

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84 +lat_0=0 +lon_0=90"        \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff rightside.tiff

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84 +lat_0=0 +lon_0=180"       \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff backside.tiff

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84 +lat_0=0 +lon_0=-90"       \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff leftside.tiff

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84 +lat_0=90 +lon_0=0"        \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff topside.tiff

    gdalwarp -t_srs "+wktext +proj=qsc +units=m +ellps=WGS84 +lat_0=-90 +lon_0=0"       \
        -wo SOURCE_EXTRA=100 -wo SAMPLE_GRID=YES -te -6378137 -6378137 6378137 6378137  \
        worldmap.tiff bottomside.tiff


Explanation:

* QSC projection is selected with ``+wktext +proj=qsc``.
* The WGS84 ellipsoid is specified with ``+ellps=WGS84``.
* The cube side is selected with ``+lat_0=... +lon_0=...``.
* The ``-wo`` options are necessary for GDAL to avoid holes in the output maps.
* The ``-te`` option limits the extends of the output map to the major axis diameter
  (from -radius to +radius in both x and y direction). These are the dimensions of one side
  of the circumscribing cube.


The resulting images can be laid out in a grid like below.


.. |topside| image:: ../../../images/qsc_topside.jpg
   :scale: 50 %
   :align: middle
   :alt:   Top side

.. |leftside| image:: ../../../images/qsc_leftside.jpg
   :scale: 50%
   :align: middle
   :alt:   Left side

.. |frontside| image:: ../../../images/qsc_frontside.jpg
   :scale: 50%
   :align: middle
   :alt:   Front side

.. |rightside| image:: ../../../images/qsc_rightside.jpg
   :scale: 50%
   :align: middle
   :alt:   Right side

.. |backside| image:: ../../../images/qsc_backside.jpg
   :scale: 50%
   :align: middle
   :alt:   Back side

.. |bottomside| image:: ../../../images/qsc_bottomside.jpg
   :scale: 50%
   :align: middle
   :alt:   Bottom side


+------------+--------------+-------------+------------+
|            | |topside|    |             |            |
+------------+--------------+-------------+------------+
| |leftside| | |frontside|  | |rightside| | |backside| |
+------------+--------------+-------------+------------+
|            | |bottomside| |             |            |
+------------+--------------+-------------+------------+

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
################################################################################

#. `Wikipedia <https://en.wikipedia.org/wiki/Quadrilateralized_spherical_cube>`_
#. `NASA <https://lambda.gsfc.nasa.gov/product/cobe/skymap_info_new.cfm>`_
