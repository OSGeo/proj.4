.. _affine:

================================================================================
Affine transformation
================================================================================

.. versionadded:: 6.0.0

The affine transformation applies translation and scaling/rotation terms on the
x,y,z coordinates, and translation and scaling on the temporal coordinate.

+---------------------+----------------------------------------------------------+
| **Alias**           | affine                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 4D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | XYZT                                                     |
+---------------------+----------------------------------------------------------+
| **output type**     | XYZT                                                     |
+---------------------+----------------------------------------------------------+

By default, the parameters are set for an identity transforms. The transformation
is reversible unless the determinant of the sji matrix is 0, or `tscale` is 0


Parameters
################################################################################

Optional
-------------------------------------------------------------------------------

.. option:: +xoff=<value>

    Offset in X. Default value: 0

.. option:: +yoff=<value>

    Offset in Y. Default value: 0

.. option:: +zoff=<value>

    Offset in Z. Default value: 0

.. option:: +toff=<value>

    Offset in T. Default value: 0

.. option:: +s11=<value>

    Rotation/scaling term. Default value: 1

.. option:: +s12=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s13=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s21=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s22=<value>

    Rotation/scaling term. Default value: 1

.. option:: +s23=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s31=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s32=<value>

    Rotation/scaling term. Default value: 0

.. option:: +s33=<value>

    Rotation/scaling term. Default value: 1

.. option:: +tscale=<value>

    Time scaling term. Default value: 1



Mathematical description
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. math::
    :label: formula

    \begin{align}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
            T \\
        \end{bmatrix}^{dest} =
        \begin{bmatrix}
            xoff \\
            yoff \\
            zoff \\
            toff \\
        \end{bmatrix} +
        \begin{bmatrix}
             s11 & s12 & s13 & 0 \\
             s21 & s22 & s23 & 0 \\
             s31 & s32 & s33 & 0 \\
             0   & 0   & 0   & tscale \\
        \end{bmatrix}
        \begin{bmatrix}
            X \\
            Y \\
            Z \\
            T \\
        \end{bmatrix}^{source}
    \end{align}


