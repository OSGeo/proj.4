.. _helmert:

================================================================================
Helmert transform
================================================================================

.. versionadded:: 5.0.0

The Helmert transformation changes coordinates from one reference frame to
another by means of 3-, 4-and 7-parameter shifts, or one of their 6-, 8- and
14-parameter kinematic counterparts.


+-----------------+-------------------------------------------------------------------+
| **Alias**       | helmert                                                           |
+-----------------+-------------------------------------------------------------------+
| **Domain**      | 2D, 3D and 4D                                                     |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates (spatial), decimalyears (temporal).         |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Cartesian coordinates (spatial), decimalyears (temporal).         |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+

The Helmert transform, in all its various incarnations, is used to perform reference
frame shifts. The transformation operates in cartesian space. It can be used to transform
planar coordinates from one datum to another, transform 3D cartesian
coordinates from one static reference frame to another or it can be used to do fully
kinematic transformations from global reference frames to local static frames.

All of the parameters described in the table above are marked as optional. This is true
as long as at least one parameter is defined in the setup of the transformation.
The behavior of the transformation depends on which parameters are used in the setup.
For instance, if a rate of change parameter is specified a kinematic version of the
transformation is used.

The kinematic transformations require an observation time of the coordinate, as well
as a central epoch for the transformation. The latter is usually documented
alongside the rest of the transformation parameters for a given transformation.
The central epoch is controlled with the parameter `t_epoch`. The observation
time is given as part of the coordinate when using PROJ's 4D-functionality.

Examples
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Transforming coordinates from NAD72 to NAD83 using the 4 parameter 2D Helmert:

::

    proj=helmert convention=coordinate_frame x=-9597.3572 y=.6112 s=0.304794780637 theta=-1.244048

Simplified transformations from ITRF2008/IGS08 to ETRS89 using 7 parameters:

::

    proj=helmert convention=coordinate_frame x=0.67678    y=0.65495   z=-0.52827
                rx=-0.022742 ry=0.012667 rz=0.022704  s=-0.01070

Transformation from `ITRF2000`  to `ITRF93` using 15 parameters:

::

    proj=helmert convention=position_vector
         x=0.0127     y=0.0065     z=-0.0209  s=0.00195
         dx=-0.0029   dy=-0.0002   dz=-0.0006 ds=0.00001
         rx=-0.00039  ry=0.00080   rz=-0.00114
         drx=-0.00011 dry=-0.00019 drz=0.00007
         t_epoch=1988.0

Parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. note::

    All parameters are optional but at least one should be used, otherwise the
    operation will return the coordinates unchanged.

.. option:: +convention=coordinate_frame/position_vector

    .. versionadded:: 5.2.0

    Indicates the convention to express the rotational terms when a 3D-Helmert /
    7-parameter more transform is involved. As soon as a rotational parameter
    is specified (one of ``rx``, ``ry``, ``rz``, ``drx``, ``dry``, ``drz``),
    ``convention`` is required.

    The two conventions are equally popular and a frequent source of confusion.
    The coordinate frame convention is also described as an clockwise
    rotation of the coordinate frame. It corresponds to EPSG method code
    1032 (in the geocentric domain) or 9607 (in the geographic domain)
    The position vector convention is also described as an anticlockwise
    (counter-clockwise) rotation of the coordinate frame.
    It corresponds to as EPSG method code 1033 (in the geocentric domain) or
    9606 (in the geographic domain).

    This parameter is ignored when only a 3-parameter
    (translation terms only: ``x``, ``y``, ``z``) , 4-parameter (3-parameter
    and ``theta``) or 6-parameter (3-parameter and their derivative terms)
    is used.

    The result obtained with parameters specified in a given convention
    can be obtained in the other convention by negating the rotational parameters
    (``rx``, ``ry``, ``rz``, ``drx``, ``dry``, ``drz``)

    .. note:: This parameter obsoletes ``transpose`` which was present in
              PROJ 5.0 and 5.1, and is forbidden starting with PROJ 5.2

.. option:: +x=<value>

    Translation of the x-axis given in meters.

.. option:: +y=<value>

    Translation of the y-axis given in meters.

.. option:: +z=<value>

    Translation of the z-axis given in meters.

.. option:: +s=<value>

    Scale factor given in ppm.

.. option:: +rx=<value>

    X-axis rotation in the 3D Helmert given arc seconds.


.. option:: +ry=<value>

    Y-axis rotation in the 3D Helmert given in arc seconds.

.. option:: +rz=<value>

    Z-axis rotation in the 3D Helmert given in arc seconds.


.. option:: +theta=<value>

    Rotation angle in the 2D Helmert given in arc seconds.

.. option:: +dx=<value>

    Translation rate of the x-axis given in m/year.

.. option:: +dy=<value>

    Translation rate of the y-axis given in m/year.

.. option:: +dz=<value>

    Translation rate of the z-axis given in m/year.

.. option:: +ds=<value>

    Scale rate factor given in ppm/year.

.. option:: +drx=<value>

    Rotation rate of the x-axis given in arc seconds/year.

.. option:: +dry=<value>

    Rotation rate of the y-axis given in arc seconds/year.

.. option:: +drz=<value>

    Rotation rate of the y-axis given in arc seconds/year.

.. option:: +t_epoch=<value>

    Central epoch of transformation given in decimalyear. Only used
    spatiotemporal transformations.

.. option:: +exact

    Use exact transformation equations.

    See :eq:`rot_exact`

.. option:: +transpose

    .. deprecated:: 5.2.0 (removed)

    Transpose rotation matrix and follow the **Position Vector** rotation
    convention. If :option:`+transpose` is not added the **Coordinate Frame**
    rotation convention is used.



Mathematical description
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

In the notation used below, :math:`\hat{P}` is the rate of change of a given transformation
parameter :math:`P`. :math:`\dot{P}` is the kinematically adjusted version of :math:`P`,
described by

.. math::
    :label: propagation

    \dot{P}= P + \hat{P}\left(t - t_{central}\right)

where :math:`t` is the observation time of the coordinate and :math:`t_{central}` is
the central epoch of the transformation. Equation :eq:`propagation` can be used to
propagate all transformation parameters in time.

Superscripts of vectors denote the reference frame the coordinates in the vector belong to.


2D Helmert
-------------------------------------------------------------------------------

The simplest version of the Helmert transform is the 2D case. In the 2-dimensional
case only the horizontal coordinates are changed. The coordinates can be
translated, rotated and scale. Translation is controlled with the `x` and `y`
parameters. The rotation is determined by `theta` and the scale is controlled with
the `s` parameters.

.. note::

    The scaling parameter `s` is unitless for the 2D Helmert, as opposed to the
    3D version where the scaling parameter is given in units of ppm.

Mathematically the 2D Helmert is described as:

.. math::
    :label: 4param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            T_x \\
            T_y \\
        \end{bmatrix} +
        s
        \begin{bmatrix}
            \hphantom{-}\cos \theta & \sin \theta \\
            -\sin \theta & \cos \theta \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
        \end{bmatrix}^A
    \end{align}


:eq:`4param` can be extended to a time-varying kinematic version by
adjusting the parameters with :eq:`propagation` to :eq:`4param`, which yields
the kinematic 2D Helmert transform:

.. math::
    :label: 8param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            \dot{T_x} \\
            \dot{T_y} \\
        \end{bmatrix} +
        s(t)
        \begin{bmatrix}
             \hphantom{-}\cos \dot{\theta} & \sin \dot{\theta}  \\
                        -\sin\ \dot{\theta} & \cos \dot{\theta} \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
        \end{bmatrix}^A
    \end{align}

All parameters in :eq:`8param` are determined by the use of :eq:`propagation`,
which applies the rate of change to each individual parameter for a given
timespan between :math:`t` and :math:`t_{central}`.


3D Helmert
-------------------------------------------------------------------------------

The general form of the 3D Helmert is

.. math::
    :label: general-helmert


    \begin{align}
        V^B = T + \left(1 + s \times 10^{-6}\right) \mathbf{R} V^A
    \end{align}

Where :math:`T` is a vector consisting of the three translation parameters, :math:`s`
is the scaling factor and :math:`\mathbf{R}` is a rotation matrix. :math:`V^A` and
:math:`V^B` are coordinate vectors, with :math:`V^A` being the input coordinate and
:math:`V^B` is the output coordinate.

In the *Position Vector* convention, we define :math:`R_x = radians \left( rx \right)`,
:math:`R_z = radians \left( ry \right)` and :math:`R_z = radians \left( rz \right)`

In the *Coordinate Frame* convention, :math:`R_x = - radians \left( rx \right)`,
:math:`R_z = - radians \left( ry \right)` and :math:`R_z = - radians \left( rz \right)`

The rotation matrix is composed of three rotation matrices, one for each axis.

.. math::

    \begin{align}
        \mathbf{R}_X &= \begin{bmatrix} 1 & 0 & 0\\ 0 & \cos R_x  & -\sin R_x \\ 0 & \sin R_x  & \cos R_x  \end{bmatrix}
    \end{align}

.. math::

    \begin{align}
        \mathbf{R}_Y &= \begin{bmatrix} \cos R_y & 0 & \sin R_y\\ 0 & 1 & 0\\ -\sin R_y & 0 & \cos R_y \end{bmatrix}
    \end{align}

.. math::

    \begin{align}
        \mathbf{R}_Z &= \begin{bmatrix} \cos R_z  & -\sin R_z  & 0\\ \sin R_z  & \cos R_z  & 0\\ 0 & 0 & 1 \end{bmatrix}
    \end{align}

The three rotation matrices can be combined in one:

.. math::

    \begin{align}
        \mathbf{R} = \mathbf{R_X} \mathbf{R_Y} \mathbf{R_Y}
    \end{align}


For :math:`\mathbf{R}`, this yields:

.. math::
    :label: rot_exact

    \begin{bmatrix}
      \cos R_y \cos R_z  &  -\cos R_x  \sin R_z  +       &   \sin R_x  \sin R_z  +   \\
                         &  \sin R_x  \sin R_y \cos R_z  &   \cos R_x  \sin R_y \cos R_z  \\
      \cos R_y\sin R_z   &  \cos R_x  \cos R_z  +        &  - \sin R_x  \cos R_z +  \\
                         &  \sin R_x  \sin R_y \sin R_z  &   \cos R_x  \sin R_y \sin R_z  \\
      -\sin R_y          &  \sin R_x  \cos R_y            &   \cos R_x  \cos R_y \\
     \end{bmatrix}


Using the small angle approximation the rotation matrix can be simplified to

.. math::
    :label: rot_approx

    \begin{align} \mathbf{R} =
        \begin{bmatrix}
             1  & -R_z  &  R_y \\
             Rz &  1    & -R_x \\
            -Ry &  R_x  &  1   \\
        \end{bmatrix}
    \end{align}

Which allow us to express the most common version of the Helmert transform,
using the approximated rotation matrix:


.. math::
    :label: 7param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            T_x \\
            T_y \\
            T_z \\
        \end{bmatrix} +
        \left(1 + s \times 10^{-6}\right)
        \begin{bmatrix}
             1  & -R_z  &  R_y \\
             Rz &  1    & -R_x \\
            -Ry &  R_x  &  1   \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^A
    \end{align}

If the rotation matrix is transposed, or the sign of the rotation terms negated,
the rotational part of the transformation is effectively reversed.
This is what happens when switching between the 2 conventions ``position_vector``
and ``coordinate_frame``

Applying :eq:`propagation` we get the kinematic version of the approximated
3D Helmert:

.. math::
    :label: 14param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            \dot{T_x} \\
            \dot{T_y} \\
            \dot{T_z} \\
        \end{bmatrix} +
        \left(1 + \dot{s} \times 10^{-6}\right)
        \begin{bmatrix}
             1         & -\dot{R_z}  &  \dot{R_y} \\
             \dot{R_z} &  1          & -\dot{R_x} \\
            -\dot{R_y} &  \dot{R_x}  &  1      \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^A
    \end{align}




The Helmert transformation can be applied without using the rotation parameters,
in which case it becomes a simple translation of the origin of the coordinate
system. When using the Helmert in this version equation :eq:`general-helmert`
simplifies to:

.. math::
    :label: 3param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            T_x \\
            T_y \\
            T_z \\
        \end{bmatrix} +
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^A
    \end{align}

That after application of :eq:`propagation` has the following kinematic
counterpart:

.. math::
    :label: 6param

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^B =
        \begin{bmatrix}
            \dot{T_x} \\
            \dot{T_y} \\
            \dot{T_z} \\
        \end{bmatrix} +
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
        \end{bmatrix}^A
    \end{align}
