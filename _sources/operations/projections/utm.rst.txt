.. _utm:

********************************************************************************
Universal Transverse Mercator (UTM)
********************************************************************************

The Universal Transverse Mercator is a system of map projections divided into
sixty zones across the globe, with each zone corresponding to 6 degrees of
longitude.

+---------------------+----------------------------------------------------------+
| **Classification**  | Transverse cylindrical, conformal                        |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, ellipsoidal only                    |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Within the used zone, but transformations of coordinates |
|                     | in adjacent zones can be expected to be accurate as well |
+---------------------+----------------------------------------------------------+
| **Alias**           | utm                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+

.. figure:: ../../../images/utm_zones.png
   :alt:   Universal Transverse Mercator (UTM) zones
   :figwidth: 100%
   :align: center

   UTM zones.

UTM projections are really the :ref:`Transverse Mercator<tmerc>`
to which specific parameters, such as central meridians, have been applied.
The Earth is divided into 60 zones each generally 6° wide in longitude.
Bounding meridians are evenly divisible by 6°, and zones are
numbered from 1 to 60 proceeding east from the 180th meridian from Greenwich
with minor exceptions :cite:`Snyder1987`.

Usage
#####

Convert geodetic coordinate to UTM Zone 32 on the northern hemisphere::

    $ echo 12 56 | proj +proj=utm +zone=32
    687071.44       6210141.33

Convert geodetic coordinate to UTM Zone 59 on the southern hemisphere::

    $ echo 174 -44 | proj +proj=utm +zone=59 +south
    740526.32       5123750.87

Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. option:: +zone=<value>

    Select which UTM zone to use. Can be a value between 1-60.


Optional
-------------------------------------------------------------------------------

.. option:: +south

    Add this flag when using the UTM on the southern hemisphere.

.. option:: +approx

    .. versionadded:: 6.0.0

    Use faster, less accurate algorithm for the Transverse Mercator.

.. option:: +algo=auto/evenden_snyder/poder_engsager

    .. versionadded:: 7.1

    Selects the algorithm to use. The hardcoded value and the one defined in
    :ref:`proj-ini` default to ``poder_engsager``, that is the most precise
    one.

    When using auto, a heuristics based on the input coordinate to transform
    is used to determine if the faster Evenden-Snyder method can be used, for
    faster computation, without causing an error greater than 0.1 mm (for an
    ellipsoid of the size of Earth)

    Note that :option:`+approx` and :option:`+algo` are mutually exclusive.

.. include:: ../options/ellps.rst

Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system>`_
