.. _ob_tran:

********************************************************************************
General Oblique Transformation
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical                                              |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | ob_tran                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/ob_tran.png
   :width: 500 px
   :align: center
   :alt:   General Oblique Transformation

   proj-string: ``+proj=ob_tran +o_proj=mill +o_lon_p=40 +o_lat_p=50 +lon_0=60``

Usage
################################################################################

All of the projections of spherical library can be used as an
oblique projection by means of the General Oblique Transformation. The user
performs the oblique transformation by selecting the oblique projection
``+proj=ob_tran``, specifying the translation factors, :option:`+o_lat_p`, and
:option:`+o_lon_p`, and the projection to be used, :option:`+o_proj`. In the
example of the Fairgrieve projection, the latitude and longitude of the North pole
of the unrotated geographic CRS, :math:`\alpha` and :math:`\beta` respectively,
expressed in the rotated geographic CRS, are to be placed
at 45°N and 90°W  and the :ref:`moll` projection is used. Because the central meridian
of the translated coordinates will follow the :math:`\beta` meridian it is
necessary to translate the translated system so that the Greenwich meridian
will pass through the center of the projection by offsetting the central meridian.

The final control for this projection is::

    +proj=ob_tran +o_proj=moll +o_lat_p=45 +o_lon_p=-90 +lon_0=-90

Parameters
################################################################################

Required
--------------------------------------------------------------------------------

.. option:: +o_proj=<projection>

    Oblique projection.

In addition to specifying an oblique projection, *how* to rotate the projection
should be specified. This is done in one of three ways: Define a new pole,
rotate the projection about a given point or define a new "equator" spanned by
two points on the sphere. See the details below.

New pole
................................................................................

.. option:: +o_lat_p=<latitude>

    Latitude of the North pole of the unrotated source CRS, expressed in the rotated geographic CRS.

.. option:: +o_lon_p=<longitude>

    Longitude of the North pole of the unrotated source CRS, expressed in the rotated geographic CRS.

Rotate about point
................................................................................

.. option:: +o_alpha=<value>

    Angle to rotate the projection with.

.. option:: +o_lon_c=<value>

    Longitude of the point the projection will be rotated about.

.. option:: +o_lat_c=<value>

    Latitude of the point the projection will be rotated about.

New "equator" points
................................................................................

.. option:: +lon_1=<value>

    Longitude of first point.

.. option:: +lat_1=<value>

    Latitude of first point.

.. option:: +lon_2=<value>

    Longitude of second point.

.. option:: +lat_2=<value>

    Latitude of second point.

Optional
--------------------------------------------------------------------------------

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
