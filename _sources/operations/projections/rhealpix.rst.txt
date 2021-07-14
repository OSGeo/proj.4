.. _rhealpix:

********************************************************************************
rHEALPix
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | rhealpix                                                 |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. image:: ../../../images/rhealpix.png
   :scale: 75%
   :alt:   rHEALPix

rHEALPix is a projection based on the HEALPix projection. The implementation of
rHEALPix uses the HEALPix projection. The rHEALPix combines the peaks of the
HEALPix into a square. The square's position can be translated and rotated across
the x-axis which is a novel approach for the rHEALPix projection. The initial
intention of using rHEALPix in the Spatial Computation Engine Science Collaboration
Environment (SCENZGrid).

Usage
###############################################################################

To run a rHEALPix projection on a WGS84 ellipsoidal model, use the following
command::

    proj +proj=rhealpix -f '%.2f' +ellps=WGS84 +south_square=0 +north_square=2  -E << EOF
    > 55 12
    > EOF
    55 12   6115727.86  1553840.13

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. option:: +north_square

    Position of the north polar square. Valid inputs are 0--3.

    *Defaults to 0.0.*

.. option:: +south_square

    Position of the south polar square. Valid inputs are 0--3.

    *Defaults to 0.0.*


.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
################################################################################

#. `NASA <http://healpix.jpl.nasa.gov/>`_
#. `Wikipedia <https://en.wikipedia.org/wiki/HEALPix>`_
