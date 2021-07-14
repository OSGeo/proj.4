.. _deformation:

================================================================================
Kinematic datum shifting utilizing a deformation model
================================================================================

.. versionadded:: 5.0.0

Perform datum shifts means of a deformation/velocity model.

+-----------------+--------------------------------------------------------------------+
| **Alias**       | deformation                                                        |
+-----------------+--------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates (spatial), decimalyears (temporal).          |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Cartesian coordinates (spatial), decimalyears (temporal).          |
+-----------------+--------------------------------------------------------------------+
| **Domain**      | 4D                                                                 |
+-----------------+--------------------------------------------------------------------+
| **Input type**  | Geodetic coordinates                                               |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Geodetic coordinates                                               |
+-----------------+--------------------------------------------------------------------+


The deformation operation is used to adjust coordinates for intraplate deformations.
Usually the transformation parameters for regional plate-fixed reference frames such as
the ETRS89 does not take intraplate deformation into account. It is assumed that
tectonic plate of the region is rigid. Often times this is true, but near the plate
boundary and in areas with post-glacial uplift the assumption breaks. Intraplate
deformations can be modelled and then applied to the coordinates so that
they represent the physical world better. In PROJ this is done with the deformation
operation.

The horizontal grid is stored in CTable2 format and the vertical grid is stored in the
GTX format. Both grids are expected to contain grid-values in units of
mm/year. GDAL both reads and writes both file formats. Using GDAL for
construction of new grids is recommended.

Starting with PROJ 7.0, use of a GeoTIFF format is recommended to store both
the horizontal and vertical velocities.

More complex deformations can be done with the :ref:`Multi-component time-based deformation model <defmodel>` transformation.

Example
-------------------------------------------------------------------------------

In :cite:`Hakli2016` coordinate transformation including a deformation model is described.
The paper describes how coordinates from the global ITRFxx frames are transformed to the
local Nordic realisations of ETRS89. Scandinavia is an area with significant post-glacial
rebound. The deformations from the post-glacial uplift is not accounted for in the
official ETRS89 transformations so in order to get accurate transformations in the Nordic
countries it is necessary to apply the deformation model. The transformation from ITRF2008
to the Danish realisation of ETRS89 is in PROJ described as::


    proj =  pipeline ellps = GRS80
            # ITRF2008@t_obs -> ITRF2000@t_obs
    step    init = ITRF2008:ITRF2000
            # ITRF2000@t_obs -> ETRF2000@t_obs
    step    proj=helmert t_epoch = 2000.0 convention=position_vector
            x =  0.054  rx =  0.000891 drx =  8.1e-05
            y =  0.051  ry =  0.00539  dry =  0.00049
            z = -0.048  rz = -0.008712 drz = -0.000792
            # ETRF2000@t_obs -> NKG_ETRF00@2000.0
    step    proj = deformation t_epoch = 2000.0
            grids = ./eur_nkg_nkgrf03vel_realigned.tif
            inv
            # NKG_ETRF@2000.0 -> ETRF92@2000.0
    step    proj=helmert convention=position_vector s = -0.009420e
            x = 0.03863 rx = 0.00617753
            y = 0.147   ry = 5.064e-05
            z = 0.02776 rz = 4.729e-05
            # ETRF92@2000.0 -> ETRF92@1994.704
    step    proj = deformation dt = -5.296
            grids = ./eur_nkg_nkgrf03vel_realigned.tif

From this we can see that the transformation from ITRF2008 to the Danish realisation of
ETRS89 is a combination of Helmert transformations and adjustments with a deformation
model. The first use of the deformation operation is::

    proj = deformation t_epoch = 2000.0 grids = ./eur_nkg_nkgrf03vel_realigned.tif

Here we set the central epoch of the transformation, 2000.0. The observation epoch
is expected as part of the input coordinate tuple. The deformation model is
described by two grids, specified with :option:`+xy_grids` and :option:`+z_grids`.
The first is the horizontal part of the model and the second is the vertical
component.

Parameters
-------------------------------------------------------------------------------

.. option:: +xy_grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an ``@`` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids for the horizontal component of a deformation model is expected to be
    in CTable2 format.

    .. note:: :option:`+xy_grids` is mutually exclusive with :option:`+grids`

.. option:: +z_grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an `@` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids for the vertical component of a deformation model is expected to be
    in either GTX format.

    .. note:: :option:`+z_grids` is mutually exclusive with :option:`+grids`

.. option:: +grids=<list>

    .. versionadded:: 7.0.0

    Comma-separated list of grids to load. If a grid is prefixed by an `@` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids should be in GeoTIFF format with the first 3 components being
    respectively the easting, northing and up velocities in mm/year.
    Setting the Description and Unit Type GDAL band metadata items is strongly
    recommended, so that gdalinfo reports:

    ::

        Band 1 Block=... Type=Float32, ColorInterp=Gray
            Description = east_velocity
            Unit Type: mm/year
        Band 2 Block=... Type=Float32, ColorInterp=Undefined
            Description = north_velocity
            Unit Type: mm/year
        Band 3 Block=... Type=Float32, ColorInterp=Undefined
            Description = up_velocity
            Unit Type: mm/year

    .. note:: :option:`+grids` is mutually exclusive with :option:`+xy_grids`
              and :option:`+z_grids`

.. option:: +t_epoch=<value>

    Central epoch of transformation given in decimalyears. Will be used in
    conjunction with the observation time from the input coordinate to
    determine :math:`dt` as used in eq. :eq:`apply_velocity` below.

    .. note:: :option:`+t_epoch` is mutually exclusive with :option:`+dt`

.. option:: +dt=<value>

    .. versionadded:: 6.0.0

    :math:`dt` as used in eq. :eq:`apply_velocity` below. Is useful when
    no observation time is available in the input coordinate or when
    a deformation for a specific timespan needs to be applied in a
    transformation. :math:`dt` is given in units of decimalyears.

    .. note:: :option:`+dt` is mutually exclusive with :option:`+t_epoch`

Mathematical description
-------------------------------------------------------------------------------

Mathematically speaking, application of a deformation model is simple. The deformation model is
represented as a grid of velocities in three dimensions. Coordinate corrections are
applied in cartesian space. For a given coordinate, :math:`(X, Y, Z)`, velocities
:math:`(V_X, V_Y, V_Z)` can be interpolated from the gridded model. The time span
between :math:`t_{obs}` and :math:`t_c` determine the magnitude of the coordinate
correction as seen in eq. :eq:`apply_velocity` below.

.. math::
    :label: apply_velocity

    \begin{align}
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix}_B =
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix}_A +
        (t_{obs} - t_c)
        \begin{pmatrix}
            V_X \\
            V_Y \\
            V_Z \\
        \end{pmatrix}
    \end{align}

Corrections are done in cartesian space.

Coordinates of the gridded model are in ENU (east, north, up) space because it
would otherwise require an enormous 3 dimensional grid to handle the corrections
in cartesian space. Keeping the correction in lat/long space reduces the
complexity of the grid significantly. Consequently though, the input coordinates
needs to be converted to lat/long space when searching for corrections in the
grid. This is done with the :ref:`cart<cart>` operation. The converted grid
corrections can then be applied to the input coordinates in cartesian space. The
conversion from ENU space to cartesian space is done in the following way:

.. math::
    :label: enu2xyz

    \begin{align}
        \begin{pmatrix}
            X \\
            Y \\
            Z \\
        \end{pmatrix} =
        \begin{pmatrix}
            -\sin\phi \cos\lambda N - \sin\lambda E + \cos\phi \cos\lambda U \\
            -\sin\phi \sin\lambda N + \sin\lambda E + \cos\phi \sin\lambda U \\
                         \cos\phi N +                 \sin\phi U \\
        \end{pmatrix}
    \end{align}

where :math:`\phi` and :math:`\lambda` are the latitude and longitude of the coordinate
that is searched for in the grid. :math:`(E, N, U)` are the grid values in ENU-space and
:math:`(X, Y, Z)` are the corrections converted to cartesian space.


See also
-----------------------------------------------------------------------------

#. :ref:`Behavioural changes from version 5 to 6<differences_deformation>`
