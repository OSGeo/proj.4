.. _transformation:

================================================================================
Geodetic transformation
================================================================================

PROJ can do everything from the most simple projection to very complex
transformations across many reference frames. While originally developed as a
tool for cartographic projections, PROJ has over time evolved into a powerful
generic coordinate transformation engine that makes it possible to do both
large scale cartographic projections as well as coordinate transformation at a
geodetic high precision level. This chapter delves into the details of how
geodetic transformations of varying complexity can be performed.

In PROJ, two frameworks for geodetic transformations exists, the
*PROJ 4.x/5.x* / :program:`cs2cs` / :c:func:`pj_transform`
framework and the *transformation pipelines* framework. The first is the original,
and limited, framework for doing geodetic transforms in PROJ The latter is a
newer addition that aims to be a more complete transformation framework. Both are
described in the sections below. Large portions of the text are based on
:cite:`EversKnudsen2017`.

Before describing the details of the two frameworks, let us first remark that
most cases of geodetic transformations can be expressed as a series of elementary
operations, the output of one operation being the input of the next. E.g. when
going from UTM zone 32, datum ED50, to UTM zone 32, datum ETRS89, one must, in the
simplest case, go through 5 steps:

1. Back-project the UTM coordinates to geographic coordinates
2. Convert the geographic coordinates to 3D cartesian geocentric coordinates
3. Apply a Helmert transformation from ED50 to ETRS89
4. Convert back from cartesian to geographic coordinates
5. Finally project the geographic coordinates to UTM zone 32 planar coordinates.


Transformation pipelines
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The homology between the above steps and a Unix shell style pipeline is evident.
It is there the main architectural inspiration behind the transformation pipeline
framework. The pipeline framework is realized by utilizing a special "projection",
that takes as its user supplied arguments, a series of elementary operations,
which it strings together in order to implement the full transformation needed.
Additionally, a number of elementary geodetic operations, including Helmert
transformations, general high order polynomial shifts and the Molodensky
transformation are available as part of the pipeline framework.
In anticipation of upcoming support for full time-varying transformations, we
also introduce a 4D spatiotemporal data type, and a programming interface
(API) for handling this.

The Molodensky transformation converts directly from geodetic coordinates
in one datum, to geodetic coordinates in another datum, while the (typically more
accurate) Helmert transformation converts from 3D cartesian to 3D cartesian
coordinates. So when using the Helmert transformation one typically needs to do an
initial conversion from geodetic to cartesian coordinates, and a final conversion
the other way round, to arrive at the desired result. Fortunately, this three-step
compound transformation has the attractive characteristic that each step depends
only on the output of the immediately preceding step. Hence, we can build a
geodetic-to-geodetic Helmert transformation by tying together the outputs and inputs
of 3 steps (geodetic-to-cartesian → Helmert → cartesian-to-geodetic), pipeline style.
The pipeline driver, makes this kind of chained transformations possible.
The implementation is compact, consisting of just one pseudo-projection, called
``pipeline``, which takes as its arguments strings of elementary projections
(note: "projection" is the, slightly misleading, PROJ term used for any kind of
transformation).
The pipeline pseudo projection is supplemented by a number of elementary
transformations, all in all providing a framework for building high accuracy
solutions for a wide spectrum of geodetic tasks.


As a first example, let us take a look at the iconic
*geodetic → Cartesian → Helmert → geodetic* case (steps 2 to 4 in the example in
the introduction). In PROJ it can be implemented as

::

    proj=pipeline
    step proj=cart ellps=intl
    step proj=helmert convention=coordinate_frame
         x=-81.0703  y=-89.3603  z=-115.7526
        rx=-0.48488 ry=-0.02436 rz=-0.41321  s=-0.540645
    step proj=cart inv ellps=GRS80

The pipeline can be expanded at both ends to accommodate whatever coordinate type
is needed for input and output: In the example below, we transform from the
deprecated Danish System 45, a 2D system with some tension in the original defining
network, to UTM zone 33, ETRS89. The tension is reduced using a polynomial
transformation (the init=./s45b... step, s45b.pol is a file containing the
polynomial coefficients), taking the S45 coordinates to a technical coordinate
system (TC32), defined to represent "UTM zone 32 coordinates, as they would look if
the Helmert transformation between ED50 and ETRS89 was perfect". The TC32
coordinates are then converted back to geodetic(ED50) coordinates, using an
inverse UTM projection, further to cartesian(ED50), then to cartesian(ETRS89),
using the relevant Helmert transformation, and back to geodetic(ETRS89), before
finally being projected onto the UTM zone 33, ETRS89 system. All in all a 6 step
pipeline, implementing a transformation with centimeter level accuracy from a
deprecated system with decimeter level tensions.

::

    proj=pipeline
    step init=./s45b.pol:s45b_tc32
    step proj=utm inv ellps=intl zone=32
    step proj=cart ellps=intl
    step proj=helmert convention=coordinate_frame
          x=-81.0703  y=-89.3603  z=-115.7526
         rx=-0.48488 ry=-0.02436 rz=-0.41321 s=-0.540645
    step proj=cart inv ellps=GRS80
    step proj=utm ellps=GRS80 zone=33

With the pipeline framework spatiotemporal transformation is possible. This is
possible by leveraging the time dimension in PROJ that enables 4D coordinates
(three spatial components and one temporal component) to be passed through a
transformation pipeline. In the example below a transformation from ITRF93 to
ITRF2000 is defined. The temporal component is given as GPS weeks in the input
data, but the 14-parameter Helmert transform expects temporal units in decimalyears.
Hence the first step in the pipeline is the unitconvert pseudo-projection that makes
sure the correct units are passed along to the Helmert transform.
Most parameters of the Helmert transform are taken from :cite:`Altamimi2002`,
except the epoch which is the epoch of the transformation.
The last step in the pipeline is converting the
coordinate timestamps back to GPS weeks.

::

    proj=pipeline
    step proj=unitconvert t_in=gps_week t_out=decimalyear
    step proj=helmert convention=coordinate_frame
         x=0.0127 y=0.0065 z=-0.0209 s=0.00195
         rx=0.00039 ry=-0.00080 rz=0.00114
         dx=-0.0029 dy=-0.0002 dz=-0.0006 ds=0.00001
         drx=0.00011 dry=0.00019 drz=-0.00007
         t_epoch=1988.0
    step proj=unitconvert t_in=decimalyear t_out=gps_week


PROJ 4.x/5.x paradigm
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _cs2cs_specific_options:

    ============   ==============================================================
    Parameter      Description
    ============   ==============================================================
    +datum         Datum name (see ``proj -ld``)
    +geoidgrids    Filename of GTX grid file to use for vertical datum transforms
    +nadgrids      Filename of NTv2 grid file to use for datum transforms
    +towgs84       3 or 7 term datum transform parameters
    +to_meter      Multiplier to convert map units to 1.0m
    +vto_meter     Vertical conversion to meters
    ============   ==============================================================

.. warning::
    This section documents the behavior of PROJ 4.x and 5.x. In PROJ 6.x,
    :program:`cs2cs` has been reworked to use :c:func:`proj_create_crs_to_crs` internally,
    with *late binding* capabilities, and thus is no longer constrained to using
    WGS84 as a pivot (also called as *early binding* method).
    When :program:`cs2cs` of PROJ 6 is used with PROJ.4 expanded strings to describe the CRS,
    including ``+towgs84``, ``+nadgrids`` and ``+geoidgrids``, it will generally give
    the same results as earlier PROJ versions. When used with AUTHORITY:CODE
    CRS descriptions, it may return different results.

The *cs2cs* framework in PROJ 4 and 5 delivers a subset of the geodetic transformations available
with the *pipeline* framework. Coordinate transformations done in this framework
were transformed in a two-step process with WGS84 as a pivot datum. That is, the
input coordinates are transformed to WGS84 geodetic coordinates and then transformed
from WGS84 coordinates to the specified output coordinate reference system, by
utilizing either the Helmert transform, datum shift grids or a combination of both.
Datum shifts can be described in a proj-string with the parameters ``+towgs84``,
``+nadgrids`` and ``+geoidgrids``.
An inverse transform exists for all three and is applied if
specified in the input proj-string. The most common is ``+towgs84``, which is used to
define a 3- or 7-parameter Helmert shift from the input reference frame to WGS84.
Exactly which realization of WGS84 is not specified, hence a fair amount of
uncertainty is introduced in this step of the transformation. With the +nadgrids
parameter a non-linear planar correction derived from interpolation in a
correction grid can be applied. Originally this was implemented as a means to
transform coordinates between the North American datums NAD27 and NAD83, but
corrections can be applied for any datum for which a correction grid exists. The
inverse transform for the horizontal grid shift is "dumb", in the sense that the
correction grid is applied verbatim without taking into account that the inverse
operation is non-linear. Similar to the horizontal grid correction, ``+geoidgrids``
can be used to perform grid corrections in the vertical component.
Both grid correction methods allow inclusion of more than one grid in the same
transformation

In contrast to the *transformation pipeline* framework, transformations with the
*cs2cs* framework in PROJ 4 and 5 were expressed as two separate proj-strings. One proj-string *to*
WGS84 and one *from* WGS84. Together they form the mapping from the source
coordinate reference system to the destination coordinate reference system.
When used with the :program:`cs2cs` the source and destination CRS's are separated by the
special ``+to`` parameter.

The following example demonstrates converting from the Greek GGRS87 datum
to WGS84 with the ``+towgs84`` parameter.

::

    cs2cs +proj=latlong +ellps=GRS80 +towgs84=-199.87,74.79,246.62
        +to +proj=latlong +datum=WGS84
    20 35
    20d0'5.467"E    35d0'9.575"N 0.000

With PROJ 6, you can simply use the following:

.. note:: With PROJ 6, the order of coordinates for EPSG geographic coordinate
          reference systems is latitude first, longitude second.

::

    cs2cs "GGRS87" "WGS 84"
    35 20
    35d0'9.575"N    20d0'5.467"E 0.000

    cs2cs EPSG:4121 EPSG:4326
    35 20
    35d0'9.575"N    20d0'5.467"E 0.000

The EPSG database provides this example for transforming from WGS72 to WGS84
using an approximated 7 parameter transformation.

::

    cs2cs +proj=latlong +ellps=WGS72 +towgs84=0,0,4.5,0,0,0.554,0.219 \
        +to +proj=latlong +datum=WGS84
    4 55
    4d0'0.554"E     55d0'0.09"N 0.000

With PROJ 6, you can simply use the following (note the reversed order for
latitude and longitude)

::

    cs2cs "WGS 72" "WGS 84"
    55 4
    55d0'0.09"N 4d0'0.554"E 0.000

    cs2cs EPSG:4322 EPSG:4326
    55 4
    55d0'0.09"N 4d0'0.554"E 0.000


Grid Based Datum Adjustments
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

In many places (notably North America and Australia) national geodetic
organizations provide grid shift files for converting between different datums,
such as NAD27 to NAD83.  These grid shift files include a shift to be applied
at each grid location. Actually grid shifts are normally computed based on an
interpolation between the containing four grid points.

PROJ supports use of grid files for shifting between various reference frames.
The grid shift table formats are CTable, NTv1 (the old Canadian format), and NTv2
(:file:`.gsb` - the new Canadian and Australian format).

The text in this section is based on the *cs2cs* framework. Gridshifting is off
course also possible with the *pipeline* framework. The major difference between the
two is that the *cs2cs* framework is limited to grid mappings to WGS84, whereas with
*transformation pipelines* it is possible to perform grid shifts between any two
reference frames, as long as a grid exists.

Use of grid shifts with :program:`cs2cs` is specified using the ``+nadgrids``
keyword in a coordinate system definition. For example:

::

    % cs2cs +proj=latlong +ellps=clrk66 +nadgrids=ntv1_can.dat \
        +to +proj=latlong +ellps=GRS80 +datum=NAD83 << EOF
    -111 50
    EOF
    111d0'2.952"W   50d0'0.111"N 0.000

In this case the :file:`/usr/local/share/proj/ntv1_can.dat` grid shift file was
loaded, and used to get a grid shift value for the selected point.

It is possible to list multiple grid shift files, in which case each will be
tried in turn till one is found that contains the point being transformed.

::

    cs2cs +proj=latlong +ellps=clrk66 \
              +nadgrids=conus,alaska,hawaii,stgeorge,stlrnc,stpaul \
        +to +proj=latlong +ellps=GRS80 +datum=NAD83 << EOF
    -111 44
    EOF
    111d0'2.788"W   43d59'59.725"N 0.000


Skipping Missing Grids
................................................................................

The special prefix ``@`` may be prefixed to a grid to make it optional.  If it
not found, the search will continue to the next grid.  Normally any grid not
found will cause an error.  For instance, the following would use the
:file:`ntv2_0.gsb` file if available, otherwise it would
fallback to using the :file:`ntv1_can.dat` file.

::

    cs2cs +proj=latlong +ellps=clrk66 +nadgrids=@ntv2_0.gsb,ntv1_can.dat \
        +to +proj=latlong +ellps=GRS80 +datum=NAD83 << EOF
    -111 50
    EOF
    111d0'3.006"W   50d0'0.103"N 0.000

The null Grid
................................................................................

A special :file:`null` grid shift file is distributed with PROJ.
This file provides a zero shift for the whole world.  It may be
listed at the end of a nadgrids file list if you want a zero shift to be
applied to points outside the valid region of all the other grids.  Normally if
no grid is found that contains the point to be transformed an error will occur.

::

    cs2cs +proj=latlong +ellps=clrk66 +nadgrids=conus,null \
        +to +proj=latlong +ellps=GRS80 +datum=NAD83 << EOF
    -111 45
    EOF
    111d0'3.006"W   50d0'0.103"N 0.000

    cs2cs +proj=latlong +ellps=clrk66 +nadgrids=conus,null \
        +to +proj=latlong +ellps=GRS80 +datum=NAD83 << EOF
    -111 44
    -111 55
    EOF
    111d0'2.788"W   43d59'59.725"N 0.000
    111dW   55dN 0.000

For more information see the chapter on :ref:`transformation_grids`.

Caveats
................................................................................

* Where grids overlap (such as conus and :file:`ntv1_can.dat` for instance) the first
  found for a point will be used regardless of whether it is appropriate or
  not.  So, for instance, ``+nadgrids=ntv1_can.dat``,conus would result in
  the Canadian data being used for some areas in the northern United States
  even though the conus data is the approved data to use for the area.
  Careful selection of files and file order is necessary.  In some cases
  border spanning datasets may need to be pre-segmented into Canadian and
  American points so they can be properly grid shifted
* Additional detail on the grid shift being applied can be found by setting
  the PROJ_DEBUG environment variable to a value.  This will result in output
  to stderr on what grid is used to shift points, the bounds of the various
  grids loaded and so forth
