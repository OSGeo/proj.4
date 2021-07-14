.. _molobadekas:

================================================================================
Molodensky-Badekas transform
================================================================================

.. versionadded:: 6.0.0

The Molodensky-Badekas transformation changes coordinates from one reference frame to
another by means of a 10-parameter shift.

.. note::

    It should not be confused with the :ref:`Molodensky` transform which
    operates directly in the geodetic coordinates. Molodensky-Badekas can rather
    be seen as a variation of :ref:`Helmert`

+-----------------+-------------------------------------------------------------------+
| **Alias**       | molobadekas                                                       |
+-----------------+-------------------------------------------------------------------+
| **Domain**      | 3D                                                                |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+

The Molodensky-Badekas transformation is a variation of the :ref:`Helmert` where
the rotational terms are not directly applied to the ECEF coordinates, but on
cartesian coordinates relative to a reference point (usually close to Earth surface,
and to the area of use of the transformation). When ``px`` = ``py`` = ``pz`` = 0,
this is equivalent to a 7-parameter Helmert transformation.

Example
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Transforming coordinates from La Canoa to REGVEN:

::

    proj=molobadekas convention=coordinate_frame
           x=-270.933 y=115.599 z=-360.226 rx=-5.266 ry=-1.238 rz=2.381
           s=-5.109 px=2464351.59 py=-5783466.61 pz=974809.81


Parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. note::

    All parameters (except convention) are optional but at least one should be
    used, otherwise the operation will return the coordinates unchanged.

.. option:: +convention=coordinate_frame/position_vector

    Indicates the convention to express the rotational terms when a 3D-Helmert /
    7-parameter more transform is involved.

    The two conventions are equally popular and a frequent source of confusion.
    The coordinate frame convention is also described as an clockwise
    rotation of the coordinate frame. It corresponds to EPSG method code
    1034 (in the geocentric domain) or 9636 (in the geographic domain)
    The position vector convention is also described as an anticlockwise
    (counter-clockwise) rotation of the coordinate frame.
    It corresponds to as EPSG method code 1061 (in the geocentric domain) or
    1063 (in the geographic domain).

    The result obtained with parameters specified in a given convention
    can be obtained in the other convention by negating the rotational parameters
    (``rx``, ``ry``, ``rz``)

.. option:: +x=<value>

    Translation of the x-axis given in meters.

.. option:: +y=<value>

    Translation of the y-axis given in meters.

.. option:: +z=<value>

    Translation of the z-axis given in meters.

.. option:: +s=<value>

    Scale factor given in ppm.

.. option:: +rx=<value>

    X-axis rotation given arc seconds.

.. option:: +ry=<value>

    Y-axis rotation given in arc seconds.

.. option:: +rz=<value>

    Z-axis rotation given in arc seconds.

.. option:: +px=<value>

    Coordinate along the x-axis of the reference point given in meters.

.. option:: +py=<value>

    Coordinate along the y-axis of the reference point given in meters.

.. option:: +pz=<value>

    Coordinate along the z-axis of the reference point given in meters.

Mathematical description
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


In the *Position Vector* convention, we define :math:`R_x = radians \left( rx \right)`,
:math:`R_z = radians \left( ry \right)` and :math:`R_z = radians \left( rz \right)`

In the *Coordinate Frame* convention, :math:`R_x = - radians \left( rx \right)`,
:math:`R_z = - radians \left( ry \right)` and :math:`R_z = - radians \left( rz \right)`

.. math::
    :label: 10param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^{output} =
        \begin{bmatrix}
            T_x + P_x\\
            T_y + P_y\\
            T_z + P_z\\
        \end{bmatrix} +
        \left(1 + s \times 10^{-6}\right)
        \begin{bmatrix}
             1  & -R_z  &  R_y \\
             Rz &  1    & -R_x \\
            -Ry &  R_x  &  1   \\
        \end{bmatrix}
        \begin{bmatrix}
            X^{input} - P_x\\
            Y^{input} - P_y\\
            Z^{input} - P_z\\
        \end{bmatrix}
    \end{align}
