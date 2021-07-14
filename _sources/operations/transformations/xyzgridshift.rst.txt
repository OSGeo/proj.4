.. _xyzgridshift:

================================================================================
Geocentric grid shift
================================================================================

.. versionadded:: 7.0.0

Geocentric translation using a grid shift

+-----------------+-------------------------------------------------------------------+
| **Alias**       | xyzgridshift                                                      |
+-----------------+-------------------------------------------------------------------+
| **Domain**      | 3D                                                                |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Cartesian coordinates                                             |
+-----------------+-------------------------------------------------------------------+

Perform a geocentric translation by bilinear interpolation of dx, dy, dz
translation values from a grid. The grid is referenced against either the
2D geographic CRS corresponding to the input (or sometimes output) CRS.

This method is described (in French) in :cite:`NTF_88`
and as EPSG operation method code 9655 in :cite:`IOGP2018` (§2.4.4.1.1
France geocentric interpolation).

The translation in the grids are added to the input coordinates in the forward direction,
and subtracted in the reverse direction.
By default (if ``grid_ref=input_crs``), in the forward direction, the input coordinates
are converted to their geographic equivalent to directly read and interpolate from
the grid. In the reverse direction, an iterative method is used to be able to find
the grid locations to read.
If ``grid_ref=output_crs`` is used, then the reverse strategy is applied: iterative
method in the forward direction, and direct read in the reverse direction.

Example
-------------------------------------------------------------------------------

NTF to RGF93 transformation using :file:`gr3df97a.tif` grid

::

    +proj=pipeline
        +step +proj=push +v_3
        +step +proj=cart +ellps=clrk80ign
        +step +proj=xyzgridshift +grids=gr3df97a.tif +grid_ref=output_crs +ellps=GRS80
        +step +proj=cart +ellps=GRS80 +inv
        +step +proj=pop +v_3

Parameters
################################################################################

The ellipsoid parameters should be the ones consistent with ``grid_ref``.
They are used to perform a geocentric to geographic conversion to find the
translation parameters.


Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. include:: ../options/ellps.rst

.. option:: +grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an ``@`` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids are expected to be in GeoTIFF format. If no metadata is provided,
    the first, second and third samples are assumed to be the geocentric
    translation along X, Y and Z axis respectively, in metres.

Optional
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +grid_ref=input_crs/output_crs

    Specify in which CRS the grid is referenced to. The default value is
    input_crs. That is the grid is referenced in the geographic CRS corresponding
    to the input geocentric CRS.

    If output_crs is specified, the grid is referenced in the geographic CRS corresponding
    to the output geocentric CRS. This is for example the case for the French
    :file:`gr3df97a.tif` grid converting from NTF to RGF93, but referenced against RGF93.
    Thus in the forward direction (NTF->RGF93), an iterative method is used to find
    the appropriate shift.

.. option:: +multiplier=<value>

    Specify the multiplier to apply to the grid values. Defaults to 1.0

