.. _geoc:

================================================================================
Geocentric Latitude
================================================================================

.. versionadded:: 5.0.0

Convert from Geodetic Latitude to Geocentric Latitude (in the forward path).

+---------------------+--------------------------------------------------------+
| **Alias**           | geoc                                                   |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                   |
+---------------------+--------------------------------------------------------+
| **Output type**     | Geocentric angular coordinates                         |
+---------------------+--------------------------------------------------------+

The geodetic (or geographic) latitude (also called planetographic latitude
in the context of non-Earth bodies) is the angle between the equatorial plane
and the normal (vertical) to the ellipsoid surface at the considered point.
The geodetic latitude is what is normally used everywhere in PROJ when angular
coordinates are expected or produced.

The geocentric latitude (also called planetocentric latitude in the context of
non-Earth bodies) is the angle between the equatorial plane and a line joining
the body centre to the considered point.

.. image:: ./images/geocentric_latitude.png
   :align: center
   :scale: 100%
   :alt:   Geocentric latitude


.. note::

    This conversion must be distinguished from the :ref:`cart`
    which converts geodetic coordinates to geocentric coordinates in the cartesian
    domain.

Mathematical definition
#######################

The formulas describing the conversion are taken from :cite:`Snyder1987`
(equation 3-28)

Let :math:`\phi'` to be the geocentric latitude and :math:`\phi`  the
geodetic latitude, then

.. math::

  \phi' = \arctan \left[ (1 - e^2) \tan \left( \phi \right) \right]

The geocentric latitude is consequently lesser (in absolute value) than the geodetic
latitude, except at the equator and the poles where they are equal.

On a sphere, they are always equal.

Usage
################################################################################

Converting from geodetic latitude to geocentric latitude::

    +proj=geoc +ellps=GRS80

Converting from geocentric latitude to geodetic latitude::

    +proj=pipeline +step +proj=geoc +inv +ellps=GRS80

Parameters
################################################################################

.. include:: ../options/ellps.rst
