.. _healpix:

********************************************************************************
HEALPix
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | healpix                                                  |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. image:: ../../../images/healpix.png
   :scale: 75%
   :alt:   HEALPix

The HEALPix projection is area preserving and can be used with a spherical and
ellipsoidal model. It was initially developed for mapping cosmic background
microwave radiation. The image below is the graphical representation of the
mapping and consists of eight isomorphic triangular interrupted map graticules.
The north and south contains four in which straight meridians converge polewards
to a point and unequally spaced horizontal parallels. HEALPix provides a mapping
in which points of equal latitude and equally spaced longitude are mapped to points
of equal latitude and equally spaced longitude with the module of the polar
interruptions.


Usage
###############################################################################

To run a forward HEALPix projection on a unit sphere model, use the following command::

    proj +proj=healpix +lon_0=0 +a=1 -E <<EOF
    0 0
    EOF
    # output
    0 0 0.00 0.00

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. option:: +rot_xy

    .. versionadded:: 6.3.0

    Rotation of the HEALPix map in degrees. A positive value results in a clockwise
    rotation around (x_0, y_0) in the cartesian / projected coordinate space.

    *Defaults to 0.0.*

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

Further reading
################################################################################

#. `NASA <http://healpix.jpl.nasa.gov/>`_
#. `Wikipedia <https://en.wikipedia.org/wiki/HEALPix>`_
