.. _differences:

================================================================================
Known differences between versions
================================================================================

Once in a while, a new version of PROJ causes changes in the existing behavior.
In this section we track deliberate changes to PROJ that break from previous
behavior. Most times that will be caused by a bug fix. Unfortunately, some bugs
have existed for so long that their faulty behavior is relied upon by software
that uses PROJ.

Behavioural changes caused by new bugs are not tracked here, as they should be
fixed in later versions of PROJ.

Version 4.6.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The default datum application behavior changed with the 4.6.0 release. PROJ
will now only apply a datum shift if both the source and destination coordinate
system have valid datum shift information.

The PROJ 4.6.0 Release Notes states

    MAJOR: Rework :c:func:`pj_transform()` to avoid applying ellipsoid to ellipsoid
    transformations as a datum shift when no datum info is available.


Version 5.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Longitude wrapping when using custom central meridian
-------------------------------------------------------------------------------

By default PROJ wraps output longitudes in the range -180 to 180. Previous to
PROJ 5, this was handled incorrectly when a custom central meridian was set with
:option:`+lon_0`. This caused a change in sign on the resulting easting as seen
below::

    $ proj +proj=merc +lon_0=110
    -70 0
    -20037508.34    0.00
    290 0
    20037508.34     0.00

From PROJ 5 on onwards, the same input now results in same coordinates, as seen
from the example below where PROJ 5 is used::

    $ proj +proj=merc +lon_0=110
    -70 0
    -20037508.34    0.00
    290 0
    -20037508.34    0.00

The change is made on the basis that :math:`\lambda=290^{\circ}` is a full
rotation of the circle larger than :math:`\lambda=-70^{\circ}` and hence
should return the same output for both.

Adding the ``+over`` flag to the projection definition provides
the old behavior.

Version 6.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Removal of :file:`proj_def.dat`
-------------------------------

Before PROJ 6, the :file:`proj_def.dat` was used to provide general and per-projection
parameters, when ``+no_defs`` was not specified. It has now been removed. In case,
no ellipsoid or datum specification is provided in the PROJ string, the
default ellipsoid is GRS80 (was WGS84 in previous PROJ versions).

Changes to :ref:`deformation<deformation>`
------------------------------------------------------------------
.. _differences_deformation:


Reversed order of operation
...........................

In the initial version of the of :ref:`deformation<deformation>` operation
the time span between :math:`t_{obs}` and :math:`t_c` was determined by the
expression

.. math::

    dt = t_c - t_{obs}

With version 6.0.0 this has been reversed in order to behave similarly to
the :ref:`Helmert operation<helmert>`, which determines time differences as

.. math::

    dt = t_{obs} - t_c

Effectively this means that the direction of the operation has been reversed,
so that what in PROJ 5 was a forward operation is now an inverse operation and
vice versa.

Pipelines written for PROJ 5 can be migrated to PROJ 6 by adding :option:`+inv`
to forward steps involving the deformation operation. Similarly
:option:`+inv` should be removed from inverse steps to be compatible with
PROJ 6.

Removed ``+t_obs``  parameter
.............................

The ``+t_obs`` parameter was confusing for users since it effectively
overwrote the observation time in input coordinates. To make it more clear
what is the operation is doing, users are now required to directly specify
the time span for which they wish to apply a given deformation. The parameter
:option:`+dt` has been added for that purpose. The new parameter is mutually
exclusive with :option:`+t_epoch`. :option:`+dt` is used when deformation
for a set amount of time is needed and :option:`+t_epoch` is used (in
conjunction with the observation time of the input coordinate) when
deformation from a specific epoch to the observation time is needed.

Version 6.3.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

projinfo
--------

Before PROJ 6.3.0, WKT1:GDAL was implicitly calling ``--boundcrs-to-wgs84``, to
add a ``TOWGS84[]`` node in some cases. This is no longer the case.


Version 7.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

proj
--------

Removed ``-ld`` option from application, since it promoted use of deprecated
parameters like ``+towgs`` and ``+datum``.

cs2cs
--------

Removed ``-ld`` option from application, since it promoted use of deprecated
parameters like ``+towgs`` and ``+datum``.

UTF-8 adoption
--------------

The value of all path, filenames passed to PROJ through function calls, PROJ
strings or environment variables should be encoded in UTF-8.
