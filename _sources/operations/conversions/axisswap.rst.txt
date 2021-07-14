.. _axisswap:

================================================================================
Axis swap
================================================================================

.. versionadded:: 5.0.0

Change the order and sign of 2,3 or 4 axes.

+---------------------+--------------------------------------------------------+
| **Alias**           | axisswap                                               |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D, 3D or 4D                                           |
+---------------------+--------------------------------------------------------+
| **Input type**      | Any                                                    |
+---------------------+--------------------------------------------------------+
| **Output type**     | Any                                                    |
+---------------------+--------------------------------------------------------+


Each of the possible four axes are numbered with 1--4, such that the first input axis
is 1, the second is 2 and so on. The output ordering is controlled by a list of the
input axes re-ordered to the new mapping.

Usage
################################################################################

Reversing the order of the axes::

    +proj=axisswap +order=4,3,2,1

Swapping the first two axes (x and y)::

    +proj=axisswap +order=2,1,3,4

The direction, or sign, of an axis can be changed by adding a minus in
front of the axis-number::

    +proj=axisswap +order=1,-2,3,4

It is only necessary to specify the axes that are affected by the swap
operation::

    +proj=axisswap +order=2,1

Parameters
################################################################################

.. option:: +order=<list>

    Ordered comma-separated list of axis, e.g. *+order=2,1,3,4*.
    Adding a minus in front of an axis number results in a change of direction
    for that axis, e.g. southward instead of northward.

    *Required.*
