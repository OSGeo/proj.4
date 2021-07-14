.. _vgridshift:

================================================================================
Vertical grid shift
================================================================================

.. versionadded:: 5.0.0

Change Vertical datum change by grid shift

+-----------------+--------------------------------------------------------------------+
| **Alias**       | vgridshift                                                         |
+-----------------+--------------------------------------------------------------------+
| **Domain**      | 3D and 4D                                                          |
+-----------------+--------------------------------------------------------------------+
| **Input type**  | Geodetic coordinates (horizontal), meters (vertical),              |
|                 | decimalyear (temporal)                                             |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Geodetic coordinates (horizontal), meters (vertical),              |
|                 | decimalyear (temporal)                                             |
+-----------------+--------------------------------------------------------------------+

The vertical grid shift is done by offsetting the vertical input coordinates by
a specific amount determined by the loaded grids. The simplest use case of the
horizontal grid shift is applying a single grid. Here we change the vertical
reference from the ellipsoid to the global geoid model, EGM96::

    +proj=vgridshift +grids=egm96_15.gtx


More than one grid can be loaded at the same time, for instance in the case where
a better geoid model than the global is available for a certain area. Here the
gridshift is set up so that the local DVR90 geoid model takes precedence over
the global model::

    +proj=vgridshift +grids=@dvr90.gtx,egm96_15.gtx

The ``@`` in the above example states that the grid is optional, in case the grid
is not found in the PROJ search path. The list of grids is prioritized so that
grids in the start of the list takes precedence over the grids in the back of the
list.

PROJ supports the GTX file format for vertical grid corrections. Details
about all the format can be found in the GDAL documentation. GDAL both reads and
writes the format. Using GDAL for construction of new grids is recommended.

Temporal gridshifting
################################################################################
.. versionadded:: 5.1.0

By initializing the vertical gridshift operation with a central epoch, it can be
used as a step function applying the grid offsets only if a coordinate is
transformed from an epoch before grids central epoch to an epoch after. This is
handy in transformations where it is necessary to handle deformations caused by
seismic activity.

The central epoch of the grid is controlled with :option:`+t_epoch` and the final
epoch of the coordinate is set with :option:`+t_final`. The observation epoch of
the coordinate is part of the coordinate tuple.

Suppose we want to model the deformation of the 2008 earthquake in Iceland in
a transformation of data from 2005 to 2009::

    echo 63.992 -21.014 10.0 2005.0 | cct +proj=vgridshift +grids=iceland2008.gtx +t_epoch=2008.4071 +t_final=2009.0
    63.992 -21.014 10.11 2005.0

.. note::
    The timestamp of the resulting coordinate is still 2005.0. The observation
    time is always kept unchanged as it would otherwise be impossible to do the
    inverse transformation.

Temporal gridshifting is especially powerful in transformation pipelines where
several gridshifts can be chained together, effectively acting as a series of
step functions that can be applied to a coordinate that is propagated through
time. In the following example we establish a pipeline that allows transformation
of coordinates from any given epoch up until the current date, applying only
those gridshifts that have central epochs between the observation epoch and
the final epoch::

    +proj=pipeline +t_final=now
    +step +proj=vgridshift +grids=earthquake_1.gtx +t_epoch=2010.421
    +step +proj=vgridshift +grids=earthquake_2.gtx +t_epoch=2013.853
    +step +proj=vgridshift +grids=earthquake_3.gtx +t_epoch=2017.713

.. note::
    The special epoch *now* is used when specifying the final epoch with
    :option:`+t_final`. This results in coordinates being transformed to the
    current date. Additionally, :option:`+t_final` is used as a
    :ref:`global pipeline parameter<global-pipeline-parameter>`, which means
    that it is applied to all the steps in the pipeline.

In the above transformation, a coordinate with observation epoch 2009.32 would
be subject to all three gridshift steps in the pipeline. A coordinate with
observation epoch 2014.12 would only by offset by the last step in the pipeline.

Parameters
################################################################################

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +grids=<list>

    Comma-separated list of grids to load. If a grid is prefixed by an `@` the
    grid is considered optional and PROJ will the not complain if the grid is
    not available.

    Grids are expected to be in GTX format.

Optional
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. include:: ../options/t_epoch.rst
.. versionadded:: 5.1.0
.. include:: ../options/t_final.rst
.. versionadded:: 5.1.0

.. option:: +multiplier=<value>

    Specify the multiplier to apply to the grid value in the forward transformation
    direction, such that:

    .. math::
        :label: multiplier_formula

        Z_{target} = Z_{source} + multiplier \times gridvalue

    The multiplier can be used to control whether the gridvalue should be added
    or subtracted, and if unit conversion must be done (the multiplied gridvalue
    must be expressed in metre).

    Note that the default is `-1.0` for historical reasons.
.. versionadded:: 5.2.0
