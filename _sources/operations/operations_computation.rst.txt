.. _operations_computation:

================================================================================
Computation of coordinate operations between two CRS
================================================================================

:Author: Even Rouault
:Last Updated: 2021-02-10

Introduction
------------

When using :command:`projinfo -s {crs_def} -t {crs_def}`,
:command:`cs2cs {crs_def} {crs_def}` or the underlying
:c:func:`proj_create_crs_to_crs` or :cpp:func:`proj_create_operations` functions,
PROJ applies an algorithm to compute one or several candidate coordinate operations,
that can be expressed as a PROJ :ref:`pipeline <pipeline>` to transform between the source
and the target CRS. This document is about the description of this algorithm that
finds the actual operations to apply to be able later to perform transform coordinates.
So this is mostly about metadata management around coordinate operation methods,
and not about the actual mathematics used to implement those methods.
As a matter of fact with PROJ 6, there are about 60 000
lines of code dealing with "metadata" management (including conversions between PROJ
strings, all CRS WKT variants), to be compared to 30 000 for the purely computation part.

This document is meant as a plain text explanation of the code for developers,
but also as a in-depth examination of what happens under the hood for curious PROJ
users. It is important to keep in mind that it is not meant to be the ultimate
source of truth of how coordinate operations should be computed. There are clearly
implementation choices and compromises that can be questioned.

Let us start with an example to research operations between the NAD27 and NAD83
geographic CRS:

.. code-block:: shell

    $ projinfo -s NAD27 -t NAD83 --summary --spatial-test intersects --grid-check none

    Candidate operations found: 10
    DERIVED_FROM(EPSG):1312, NAD27 to NAD83 (3), 1.0 m, Canada
    DERIVED_FROM(EPSG):1313, NAD27 to NAD83 (4), 1.5 m, Canada - NAD27, at least one grid missing
    DERIVED_FROM(EPSG):1241, NAD27 to NAD83 (1), 0.15 m, USA - CONUS including EEZ
    DERIVED_FROM(EPSG):1243, NAD27 to NAD83 (2), 0.5 m, USA - Alaska including EEZ
    DERIVED_FROM(EPSG):1573, NAD27 to NAD83 (6), 1.5 m, Canada - Quebec, at least one grid missing
    EPSG:1462, NAD27 to NAD83 (5), 1.0 m, Canada - Quebec, at least one grid missing
    EPSG:9111, NAD27 to NAD83 (9), 1.5 m, Canada - Saskatchewan, at least one grid missing
    unknown id, Ballpark geographic offset from NAD27 to NAD83, unknown accuracy, World, has ballpark transformation
    EPSG:8555, NAD27 to NAD83 (7), 0.15 m, USA - CONUS and GoM, at least one grid missing
    EPSG:8549, NAD27 to NAD83 (8), 0.5 m, USA - Alaska, at least one grid missing

The algorithm involves many cases, so we will progress in the explanation from
the most simple case to more complex ones. We document here the working of this
algorithm as implemented in PROJ 8.0.0.
The results of some examples might also be quite sensitive to the content of the
PROJ database and the PROJ version used.

From a code point of view, the entry point of the algorithm is the C++
:cpp:func:`osgeo::proj::operation::CoordinateOperation::createOperations` method.

It combines several strategies:

  - look up in the PROJ database for available operations
  - consider the pair (source CRS, target CRS) to synthetize operations depending
    on the nature of the source and target CRS.

Geographic CRS to Geographic CRS, with known identifiers
--------------------------------------------------------

With the above example of two geographic CRS, that have an identified identifier,
(:program:`projinfo` internally resolves NAD27 to EPSG:4267 and NAD83 to EPSG:4269)
the algorithm will first search
in the coordinate operation related tables of the :file:`proj.db` if there are records
that list direct transformations between the source and the target CRS. The
transformations typically involve :ref:`Helmert <helmert>`-style operations or datum shift based on
grids (more esoteric operations are possible).

A request similar to the following will be emitted:

.. code-block:: shell

    $ sqlite3 proj.db "SELECT auth_name, code, name, method_name, accuracy FROM \
                       coordinate_operation_view WHERE \
                       source_crs_auth_name = 'EPSG' AND \
                       source_crs_code = '4267' AND \
                       target_crs_auth_name = 'EPSG' AND \
                       target_crs_code = '4269'"

    EPSG|1241|NAD27 to NAD83 (1)|NADCON|0.15
    EPSG|1243|NAD27 to NAD83 (2)|NADCON|0.5
    EPSG|1312|NAD27 to NAD83 (3)|NTv1|1.0
    EPSG|1313|NAD27 to NAD83 (4)|NTv2|1.5
    EPSG|1462|NAD27 to NAD83 (5)|NTv1|1.0
    EPSG|1573|NAD27 to NAD83 (6)|NTv2|1.5
    EPSG|8549|NAD27 to NAD83 (8)|NADCON5 (2D)|0.5
    EPSG|8555|NAD27 to NAD83 (7)|NADCON5 (2D)|0.15
    EPSG|9111|NAD27 to NAD83 (9)|NTv2|1.5
    ESRI|108003|NAD_1927_To_NAD_1983_PR_VI|NTv2|0.05

As we have found direct transformations, we will not attempt any more complicated
research.
One can note in the above result set that a ESRI:108003 operation was found,
but as the source and target CRS are in the EPSG registry, and there are
operations between those CRS in the EPSG registry itself, transformations from
other authorities will be ignored (except if they are in the PROJ authority,
which can be used as an override).

As those results all involve operations that does not have a perfect accuracy and that
does not cover the area of use of the 2 CRSs, a
'Ballpark geographic offset from NAD27 to NAD83' operation is synthetized by PROJ
(see :term:`Ballpark transformation`)

Filtering and sorting of coordinate operations
----------------------------------------------

The last step is to filter and sort results in order of relevance.

The filtering takes into account the following criteria to decide which operations
must be retained or discarded:

* a minimum accuracy that the user might have expressed,
* an area of use on which the coordinate operation(s) must apply
* if the absence of grids needed by an operation must result in discarding it.

The sorting algorithm determines the order of relevance of the operations we got.
A comparison function compares pair of operations to determine which of the
two is the most relevant. This is implemented by the :cpp:func:`operator ()`
method of the SortFunction structure.
When comparing two operations, the following criteria are used. The tests are
performed in the order they are listed below:

1. consider as more relevant an operation that can be expressed as a PROJ operation string
   (the database might list operations whose method is not (yet) implemented by PROJ)
2. if both operations evaluate identically with respect to the above criterion,
   consider as more relevant an operation that does not include a synthetic
   ballpark vertical transformation (occurs when there is a geoid model).
3. if both operations evaluate identically with respect to the above criterion,
   consider as more relevant an operation that does not include a synthetic
   ballpark horizontal transformation.
4. consider as more relevant an operation that refers to shift grids that are locally available.
5. consider as more relevant an operation that refers to grids that are available
   in one of the proj-datumgrid packages, but not necessarily locally available
6. consider as more relevant an operation that has a known accuracy.
7. if two operations have unknown accuracy, consider as more relevant an operation
   that uses grid(s) if the other one does not (grid based operations are assumed
   to be more precise than operations relying on a few parameters)
8. consider as more relevant an operation whose area of use is larger
   (note: the computation of the are of use is approximate, based on a bounding box)
9. consider as more relevant an operation that has a better accuracy.
10. in case of same accuracy, consider as more relevant an operation that does
    not use grids (operations that use only parameters will be faster)
11. consider as more relevant an operation that involves less transformation steps
    (transformation steps considered are the ones listed in the WKT output, not PROJ pipeline steps)
12. and for completeness, if two operations are comparable given all the above criteria,
    consider as more relevant the one which has the shorter name, and if they
    have the same length, consider as more relevant the one whose name comes last in
    lexicographic order (e.g. "FOO to BAR (3)" will have higher precedence than
    "FOO to BAR (2)")

.. note::

    :c:func:`proj_trans`, on the results returned by :c:func:`proj_create_crs_to_crs`,
    will not necessarily use the operation that
    is listed in first position due to the above algorithm. :c:func:`proj_trans`
    has more context, since it has the coordinate to transform, so it can compare
    this coordinate to the area of use of operations. Typically, the above criteria
    will favor an operation that has a larger area of use over another one with a
    smaller area, due to it being more generally applicable. But once coordinates are known,
    :c:func:`proj_trans` can select an operation with a smaller
    area of use that applies to the coordinate to transform.

Geodetic/geographic CRS to Geodetic/geographic CRS, without known identifiers
-----------------------------------------------------------------------------

In a number of situations, the source and/or target CRS do not have an identifier
(WKT without identifier, PROJ string, ..)
The first step is to try to find in the :file:`proj.db` a CRS of the same nature of
the CRS to identify and whose name exactly matches the one provided to the
:c:func:`createOperations` method. If there is exactly one match and that the CRS are
"computationally" equivalent, then use the code of the CRS for further computations.

If this search did not succeed, or if the previous case with known CRS identifiers
did not result in matches in the database, the search will be based on the
datums. That is, a list of geographic CRS whose datum matches the datum of the
source and target CRS is searched for in the database (by querying the `geodetic_crs`
database table). If the datum has a known
identifier, we will use it, otherwise we will look for an equivalent datum in the
database based on the datum name.

Let's consider the case where the datum of the source CRS is EPSG:6171 "Reseau
Geodesique Francais 1993" and the datum of the target CRS is EPSG:6258 "European
Terrestrial Reference System 1989".
For EPSG:6171, there are 10 matching (non-deprecated) geodetic CRSs: 

- EPSG:4171, RGF93, geographic 2D
- EPSG:4964, RGF93, geocentric
- EPSG:4965, RGF93, geographic 3D
- EPSG:7042, RGF93 (lon-lat), geographic 3D
- EPSG:7084, RGF93 (lon-lat), geographic 2D
- IGNF:RGF93, RGF93 cartesiennes geocentriques, geocentric
- IGNF:RGF93GDD, RGF93 geographiques (dd),geographic 2D
- IGNF:RGF93GEODD, RGF93 geographiques (dd), geographic 3D
- IGNF:RGF93G, RGF93 geographiques (dms), geographic 2D
- IGNF:RGF93GEO, RGF93 geographiques (dms), geographic 3D

The first three entries from the EPSG dataset are typical: for each datum,
one can define a geographic 2D CRS (latitude, longitude), a geographic 3D crs
(latitude, longitude, ellipsoidal height) and a geocentric one. For that particular
case, the EPSG dataset has also included two extra definitions corresponding to a
longitude, latitude, [ellipsoidal height] coordinate system, as found in the official
French IGNF registry. This IGNF registry has also definitions for a geographic 2D
CRS (with an extra subtlety with an entry using decimal degree as unit and another
one degree-minute-second), geographic 3D and geocentric.

For EPSG:6258, there are 7 matching (non-deprecated) geodetic CRSs: 

- EPSG:4258, ETRS89, geographic 2D
- EPSG:4936, ETRS89, geocentric
- EPSG:4937, ETRS89, geographic 3D
- IGNF:ETRS89, ETRS89 cartesiennes geocentriques, geocentric
- IGNF:ETRS89G, ETRS89 geographiques (dms), geographic 2D
- IGNF:ETRS89GEO, ETRS89 geographiques (dms), geographic 3D
- ESRI:104129, GCS_EUREF_FIN, geographic 2D

So the 3 typical EPSG entries, 3 equivalent (with long, lat ordering for the
geographic CRS) and one from the ESRI registry;

PROJ can now test 10 x 7 different combinations of source x target CRSs, using
the database searching method explained in the previous section. As soon as
one of this combination returns at least one non-ballpark combination, the result
set coming from that combination is used. PROJ will then add before that
transformation a conversion between the source CRS and the first intermediate CRS,
and will add at the end a conversion between the second intermediate CRS and the
target CRS. Those conversions are conversion between geographic 2D and geographic 3D
CRS or geographic 2D/3D and geocentric CRS.

This is done by the :c:func:`createOperationsWithDatumPivot()` method.

So if transforming between EPSG:7042, RGF93 (lon-lat), geographic 3D and
EPSG:4936, ETRS89, geocentric, one get the following concatenated operation,
chaining an axis order change, the null geocentric translation between
RGF93 and ETRS89 (EPSG:1591), and a conversion between geographic and geocentric
coordinates. This concatenated operation is assumed to have a perfect accuracy
as both the initial and final operations are conversions, and the middle transformation
accounts for the fact that the RGF93 datum is one realization of ETRS89, so they
are equivalent for most purposes.

.. code-block:: shell

    $ projinfo -s EPSG:7042 -t EPSG:4936

    Candidate operations found: 1
    -------------------------------------
    Operation No. 1:

    unknown id, axis order change (geographic3D horizontal) + RGF93 to ETRS89 (1) + Conversion from ETRS89 (geog2D) to ETRS89 (geocentric), 0 m, France

    PROJ string:
    +proj=pipeline +step +proj=unitconvert +xy_in=deg +xy_out=rad +step +proj=cart +ellps=GRS80

    WKT2:2019 string:
    CONCATENATEDOPERATION["axis order change (geographic3D horizontal) + RGF93 to ETRS89 (1) + Conversion from ETRS89 (geog2D) to ETRS89 (geocentric)",
        SOURCECRS[
            GEOGCRS["RGF93 (lon-lat)",
                [...]
                ID["EPSG",7042]]],
        TARGETCRS[
            GEODCRS["ETRS89",
                [...]
                ID["EPSG",4936]]],
        STEP[
            CONVERSION["axis order change (geographic3D horizontal)",
                METHOD["Axis Order Reversal (Geographic3D horizontal)",
                    ID["EPSG",9844]],
                ID["EPSG",15499]]],
        STEP[
            COORDINATEOPERATION["RGF93 to ETRS89 (1)",
                [...]
                METHOD["Geocentric translations (geog2D domain)",
                    ID["EPSG",9603]],
                PARAMETER["X-axis translation",0,
                    LENGTHUNIT["metre",1],
                    ID["EPSG",8605]],
                PARAMETER["Y-axis translation",0,
                    LENGTHUNIT["metre",1],
                    ID["EPSG",8606]],
                PARAMETER["Z-axis translation",0,
                    LENGTHUNIT["metre",1],
                    ID["EPSG",8607]],
                OPERATIONACCURACY[0.0],
                ID["EPSG",1591],
                REMARK["May be taken as approximate transformation RGF93 to WGS 84 - see code 1671."]]],
        STEP[
            CONVERSION["Conversion from ETRS89 (geog2D) to ETRS89 (geocentric)",
                METHOD["Geographic/geocentric conversions",
                    ID["EPSG",9602]]]],
        USAGE[
            SCOPE["unknown"],
            AREA["France"],
            BBOX[41.15,-9.86,51.56,10.38]]]

Geodetic/geographic CRS to Geodetic/geographic CRS, without direct transformation
---------------------------------------------------------------------------------

Still considering transformations between geodetic/geographic CRS, but let's
consider that the lookup in the database for a transformation between
the source and target CRS (possibly going through the "equivalent" CRS based on
the same datum as detailed in the previous section) leads to an empty set.

Of course, as most operations are invertible, one first tries to do a lookup switching
the source and target CRS, and inverting the resulting operation(s):

.. code-block:: shell

    $ projinfo -s NAD83 -t NAD27 --spatial-test intersects --summary

    Candidate operations found: 10
    INVERSE(DERIVED_FROM(EPSG)):1312, Inverse of NAD27 to NAD83 (3), 2.0 m, Canada
    INVERSE(DERIVED_FROM(EPSG)):1313, Inverse of NAD27 to NAD83 (4), 1.5 m, Canada - NAD27
    INVERSE(DERIVED_FROM(EPSG)):1241, Inverse of NAD27 to NAD83 (1), 0.15 m, USA - CONUS including EEZ
    INVERSE(DERIVED_FROM(EPSG)):1243, Inverse of NAD27 to NAD83 (2), 0.5 m, USA - Alaska including EEZ
    INVERSE(DERIVED_FROM(EPSG)):1573, Inverse of NAD27 to NAD83 (6), 1.5 m, Canada - Quebec, at least one grid missing
    INVERSE(EPSG):1462, Inverse of NAD27 to NAD83 (5), 2.0 m, Canada - Quebec, at least one grid missing
    INVERSE(EPSG):9111, Inverse of NAD27 to NAD83 (9), 1.5 m, Canada - Saskatchewan, at least one grid missing
    unknown id, Ballpark geographic offset from NAD83 to NAD27, unknown accuracy, World, has ballpark transformation
    INVERSE(EPSG):8555, Inverse of NAD27 to NAD83 (7), 0.15 m, USA - CONUS and GoM, at least one grid missing
    INVERSE(EPSG):8549, Inverse of NAD27 to NAD83 (8), 0.5 m, USA - Alaska, at least one grid missing

That was an easy case. Now let's consider the transformation between the Australian
CRS AGD84 and GDA2020. There is no direct transformation from AGD84 to GDA2020, or
in the reverse direction, even when considering alternative geodetic CRS based on
the underlying datums. PROJ will then do a cross-join in the coordinate_operation_view
table to find the tuples (op1, op2) of coordinate operations such that:

- SOURCE_CRS = op1.source_crs AND op1.target_crs = op2.source_crs AND op2.target_crs = TARGET_CRS or
- SOURCE_CRS = op1.source_crs AND op1.target_crs = op2.target_crs AND op2.source_crs = TARGET_CRS or
- SOURCE_CRS = op1.target_crs AND op1.source_crs = op2.source_crs AND op2.target_crs = TARGET_CRS or
- SOURCE_CRS = op1.target_crs AND op1.source_crs = op2.target_crs AND op2.source_crs = TARGET_CRS

Depending on which case is selected, op1 and op2 should be reversed, before
being concatenated.

This logic is implement by the ``findsOpsInRegistryWithIntermediate()`` method.

Assuming that the proj-datumgrid-oceania package is installed, we get the
following results for the AGD84 to GDA2020 coordinate operations lookup:

.. code-block:: shell

    $ projinfo -s AGD84 -t GDA2020 --spatial-test intersects -o PROJ

    Candidate operations found: 4
    -------------------------------------
    Operation No. 1:

    unknown id, AGD84 to GDA94 (5) + GDA94 to GDA2020 (1), 0.11 m, Australia - AGD84

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=hgridshift +grids=National_84_02_07_01.gsb \
                   +step +proj=push +v_3 \
                   +step +proj=cart +ellps=GRS80 \
                   +step +proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 \
                                       +rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 \
                                       +s=-0.009994 +convention=coordinate_frame \
                   +step +inv +proj=cart +ellps=GRS80 \
                   +step +proj=pop +v_3 \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

    -------------------------------------
    Operation No. 2:

    unknown id, AGD84 to GDA94 (2) + GDA94 to GDA2020 (1), 1.01 m, Australia - AGD84

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=push +v_3 \
                   +step +proj=cart +ellps=aust_SA \
                   +step +proj=helmert +x=-117.763 +y=-51.51 +z=139.061 \
                                       +rx=-0.292 +ry=-0.443 +rz=-0.277 +s=-0.191 \
                                       +convention=coordinate_frame \
                   +step +proj=helmert +x=0.06155 +y=-0.01087 +z=-0.04019 \
                                       +rx=-0.0394924 +ry=-0.0327221 +rz=-0.0328979 \
                                       +s=-0.009994 +convention=coordinate_frame \
                   +step +inv +proj=cart +ellps=GRS80 \
                   +step +proj=pop +v_3 \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

    -------------------------------------
    Operation No. 3:

    unknown id, AGD84 to GDA94 (5) + GDA94 to GDA2020 (2), 0.15 m, unknown domain of validity

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=hgridshift +grids=National_84_02_07_01.gsb \
                   +step +proj=hgridshift +grids=GDA94_GDA2020_conformal_and_distortion.gsb \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

    -------------------------------------
    Operation No. 4:

    unknown id, AGD84 to GDA94 (5) + GDA94 to GDA2020 (3), 0.15 m, unknown domain of validity

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=hgridshift +grids=National_84_02_07_01.gsb \
                   +step +proj=hgridshift +grids=GDA94_GDA2020_conformal.gsb \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

One can see that the selected intermediate CRS that has been used is GDA94.
This is a completely novel behavior of PROJ 6 as opposed to the logic of PROJ.4 
where datum transformations implied using EPSG:4326 / WGS 84 has the mandatory
datum hub. PROJ 6 no longer hardcodes it as the mandatory datum hub, and relies
on the database to find the appropriate hub(s).
Actually, WGS 84 has been considered during the above lookup, because there are
transformations between AGD84 and WGS 84 and WGS 84 and GDA2020. However those
have been discarded in a step which we did not mention previously: just after
the initial filtering of results and their sorting, there is a final filtering
that is done. In the list of sorted results, given two operations A and B that
have the same area of use, if B has an accuracy lower than A, and A does not use
grids, or all the needed grids are available, then B is discarded.

If one forces the datum hub to be considered to be EPSG:4326, ones gets:

.. code-block:: shell

    $ projinfo -s AGD84 -t GDA2020 --spatial-test intersects --pivot-crs EPSG:4326 -o PROJ

    Candidate operations found: 2
    -------------------------------------
    Operation No. 1:

    unknown id, AGD84 to WGS 84 (7) + Inverse of GDA2020 to WGS 84 (2), 4 m, Australia - AGD84

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=push +v_3 \
                   +step +proj=cart +ellps=aust_SA \
                   +step +proj=helmert +x=-117.763 +y=-51.51 +z=139.061 \
                                       +rx=-0.292 +ry=-0.443 +rz=-0.277 \
                                       +s=-0.191 +convention=coordinate_frame \
                   +step +inv +proj=cart +ellps=GRS80 \
                   +step +proj=pop +v_3 \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

    -------------------------------------
    Operation No. 2:

    unknown id, AGD84 to WGS 84 (9) + Inverse of GDA2020 to WGS 84 (2), 4 m, Australia - AGD84

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=2,1 \
                   +step +proj=unitconvert +xy_in=deg +xy_out=rad \
                   +step +proj=hgridshift +grids=National_84_02_07_01.gsb \
                   +step +proj=unitconvert +xy_in=rad +xy_out=deg \
                   +step +proj=axisswap +order=2,1

Those operations are less accurate, since WGS 84 is assumed to be equivalent to
GDA2020 with an accuracy of 4 metre. This is an instance demonstrating that using
WGS 84 as a hub systematically can be sub-optimal.

There are still situations where the attempt to find a hub CRS does not work,
because there is no such hub. This can occur for example when transforming from
GDA94 to the latest realization at time of writing of WGS 84, WGS 84 (G1762).
There are transformations between WGS 84 (G1762). Using the above described
techniques, we would only find one non-ballpark operation taking the route:
1. Conversion from GDA94 (geog2D) to GDA94 (geocentric): synthetized by PROJ
2. Inverse of ITRF2008 to GDA94 (1): from EPSG
3. Inverse of WGS 84 (G1762) to ITRF2008 (1): from EPSG
4. Conversion from WGS 84 (G1762) (geocentric) to WGS 84 (G1762): synthetized by PROJ

This is not bad, but the global validity area of use is "Australia - onshore and EEZ",
whereas GDA94 has a larger area of use.
There is another road that can be taken by going through GDA2020 instead of ITRF2008.
The GDA94 to GDA2020 transformations operate on the respective geographic CRS,
whereas GDA2020 to WGS 84 (G1762) operate on the geocentric CRS. Consequently,
GDA2020 cannot be identifier as a hub by a "simple" self-join SQL request on
the coordinate operation table. This requires to do the join based on the datum
referenced by the source and target CRS of each operation rather than the
source and target CRS themselves. When there is a match, PROJ inserts the required
conversions between geographic and geocentric CRS to have a consistent concatenated
operation, like the following:
1. GDA94 to GDA2020 (1): from EPSG
2. Conversion from GDA2020 (geog2D) to GDA2020 (geocentric): synthetized by PROJ
3. GDA2020 to WGS 84 (G1762) (1): from EPSG
4. Conversion from WGS 84 (G1762) (geocentric) to WGS 84 (G1762) (geog2D): synthetized by PROJ

Projected CRS to any target CRS
---------------------------------------------------------------------------------

This actually extends to any Derived CRS, whose Projected CRS is a well-known
particular case. Such transformations are done in 2 steps:

1. Use the inverse conversion of the derived CRS to its base CRS, typically an
   inverse map projection.
2. Find transformations from this base CRS to the target CRS. If the base CRS
   is the target CRS, this step can be skipped.

.. code-block:: shell

    $ projinfo -s EPSG:32631 -t RGF93

    Candidate operations found: 1
    -------------------------------------
    Operation No. 1:

    unknown id, Inverse of UTM zone 31N + Inverse of RGF93 to WGS 84 (1), 1 m, France

    PROJ string:
    +proj=pipeline +step +inv +proj=utm +zone=31 +ellps=WGS84 +step +proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1

This is implemented by the ``createOperationsDerivedTo`` method

For the symmetric case, source CRS to a derived CRS, the above algorithm is applied
by switching the source and target CRS, and then inverting the resulting operation(s).
This is mostly a matter of avoiding to write very similar code twice. This logic
is also applied to all below cases when considering the transformation between 2 different
types of objects.

.. _verttogeog:

Vertical CRS to a Geographic CRS
---------------------------------------------------------------------------------

Such transformation is normally not meant as being used as standalone by PROJ
users, but as an internal computation step of a Compound CRS to a target CRS.

In cases where we are lucky, the PROJ database will have a transformation registered
between those:

.. code-block:: shell

    $ projinfo -s "NAVD88 height" -t "NAD83(2011)" -o PROJ --spatial-test intersects
    Candidate operations found: 11
    -------------------------------------
    Operation No. 1:

    INVERSE(DERIVED_FROM(EPSG)):9229, Inverse of NAD83(2011) to NAVD88 height (3), 0.015 m, USA - CONUS - onshore

    PROJ string:
    +proj=vgridshift +grids=g2018u0.gtx +multiplier=1


But in cases where there is no match, the ``createOperationsVertToGeog`` method
will be used to synthetize a ballpark vertical transformation, just taking care
of unit changes, and axis reversal in case the vertical CRS was a depth rather than
a height. Of course the results of such an operation are questionable, hence the
ballpark qualifier and a unknown accuracy advertized for such an operation.

Vertical CRS to a Vertical CRS
---------------------------------------------------------------------------------

Overall logic is similar to the above case. There might be direct operations in
the PROJ database, involving grid transformations or simple offsets. The fallback
case is to synthetize a ballpark transformation.

This is implemented by the ``createOperationsVertToVert`` method

.. code-block:: shell

    $ projinfo -s "NGVD29 depth (ftUS)" -t "NAVD88 height" --spatial-test intersects -o PROJ

    Candidate operations found: 3
    -------------------------------------
    Operation No. 1:

    unknown id, Inverse of NGVD29 height (ftUS) to NGVD29 depth (ftUS) + NGVD29 height (ftUS) to NGVD29 height (m) + NGVD29 height (m) to NAVD88 height (3), 0.02 m, USA - CONUS east of 89°W - onshore

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=1,2,-3 +step +proj=unitconvert +z_in=us-ft +z_out=m +step +proj=vgridshift +grids=vertcone.gtx +multiplier=0.001

    -------------------------------------
    Operation No. 2:

    unknown id, Inverse of NGVD29 height (ftUS) to NGVD29 depth (ftUS) + NGVD29 height (ftUS) to NGVD29 height (m) + NGVD29 height (m) to NAVD88 height (2), 0.02 m, USA - CONUS 89°W-107°W - onshore

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=1,2,-3 +step +proj=unitconvert +z_in=us-ft +z_out=m +step +proj=vgridshift +grids=vertconc.gtx +multiplier=0.001

    -------------------------------------
    Operation No. 3:

    unknown id, Inverse of NGVD29 height (ftUS) to NGVD29 depth (ftUS) + NGVD29 height (ftUS) to NGVD29 height (m) + NGVD29 height (m) to NAVD88 height (1), 0.02 m, USA - CONUS west of 107°W - onshore

    PROJ string:
    +proj=pipeline +step +proj=axisswap +order=1,2,-3 +step +proj=unitconvert +z_in=us-ft +z_out=m +step +proj=vgridshift +grids=vertconw.gtx +multiplier=0.001


Compound CRS to a Geographic CRS
---------------------------------------------------------------------------------

A typical example of a Compound CRS is a CRS made of a geographic or projected CRS
as the horizontal component, and a vertical CRS. E.g. "NAD83 + NAVD88 height"

When the horizontal component of the compound source CRS is a projected CRS, we
first look for the operation from this source CRS to another compound CRS made
of the geographic CRS base of the projected CRS,
like "NAD83 / California zone 1 (ftUS) + NAVD88 height" to "NAD83 + NAVD88 height",
which ultimately goes to one of the above described case. Then we can consider
the transformation from a compound CRS made of a geographic CRS to another geographic CRS.

It first starts by the vertical transformations from the vertical CRS of the
source compound CRS to the target geographic CRS, using the strategy detailed
in :ref:`Vertical CRS to a Geographic CRS <verttogeog>`

What we did not mention is that when there is not a transformation registered
between the vertical CRS and the target geographic CRS, PROJ attempts to find
transformations between that vertical CRS and any other geographic CRS. This is
clearly an approximation.
If the research of the vertical CRS to the target geographic CRS resulted in
operations that use grids that are not available, as another approximation, we
research operations from the vertical CRS to the source geographic CRS for the
vertical component.

Once we got those more or less accurate vertical transformations, we must consider
the horizontal transformation(s). The algorithm iterates over all found vertical
transformations and look for their target geographic CRS. This will be used as
the interpolation CRS for horizontal transformations. PROJ will then look for
available transformations from the source geographic CRS to the interpolation CRS
and from the interpolation CRS to the target geographic CRS. There is then a
3-level loop to create the final set of operations chaining together:

- the horizontal transformation from the source geographic CRS to the interpolation CRS
- the vertical transformation from the source vertical CRS to the interpolation CRS
- the horizontal transformation from the interpolation CRS to the target geographic CRS.

This is implemented by the ``createOperationsCompoundToGeog`` method

Example:

.. code-block:: shell

    $ projinfo -s "NAD83(NSRS2007) + NAVD88 height" -t "WGS 84 (G1762)" --spatial-test intersects --summary

    Candidate operations found: 21
    unknown id, Inverse of NAD83(NSRS2007) to NAVD88 height (1) + NAD83(NSRS2007) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.05 m, USA - CONUS - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (7) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, 95°W to 78°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (7) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, 95°W to 78°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (6) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, 112°W to 95°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (6) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, 112°W to 95°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (2) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, 112°W to 95°W
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (2) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, 112°W to 95°W
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (3) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, 95°W to 78°W
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (3) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, 95°W to 78°W
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (5) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, west of 112°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (5) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, west of 112°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (1) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, west of 112°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (1) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, west of 112°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (4) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, east of 78°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (4) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS north of 41°N, east of 78°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (8) + NAD83(HARN) to WGS 84 (1) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, east of 78°W - onshore
    unknown id, Inverse of NAD83(HARN) to NAD83(NSRS2007) (1) + Inverse of NAD83(HARN) to NAVD88 height (8) + NAD83(HARN) to WGS 84 (3) + WGS 84 to WGS 84 (G1762), 3.15 m, USA - CONUS south of 41°N, east of 78°W - onshore
    unknown id, Ballpark geographic offset from NAD83(NSRS2007) to NAD83(FBN) + Inverse of NAD83(FBN) to NAVD88 height (1) + Ballpark geographic offset from NAD83(FBN) to WGS 84 (G1762), unknown accuracy, USA - CONUS - onshore, has ballpark transformation
    unknown id, Ballpark geographic offset from NAD83(NSRS2007) to NAD83(2011) + Inverse of NAD83(2011) to NAVD88 height (3) + Ballpark geographic offset from NAD83(2011) to WGS 84 (G1762), unknown accuracy, USA - CONUS - onshore, has ballpark transformation
    unknown id, Ballpark geographic offset from NAD83(NSRS2007) to NAD83(2011) + Inverse of NAD83(2011) to NAVD88 height (3) + Conversion from NAD83(2011) (geog2D) to NAD83(2011) (geocentric) + Inverse of ITRF2008 to NAD83(2011) (1) + Inverse of WGS 84 (G1762) to ITRF2008 (1) + Conversion from WGS 84 (G1762) (geocentric) to WGS 84 (G1762) (geog2D), unknown accuracy, USA - CONUS - onshore, has ballpark transformation
    unknown id, NAD83(NSRS2007) to WGS 84 (1) + WGS 84 to WGS 84 (G1762) + Transformation from NAVD88 height to WGS 84 (G1762) (ballpark vertical transformation, without ellipsoid height to vertical height correction), unknown accuracy, USA - CONUS and Alaska; PRVI, has ballpark transformation


CompoundCRS to CompoundCRS
---------------------------------------------------------------------------------

There is some similarity with the previous paragraph. We first research the
vertical transformations between the two vertical CRS.

1. If there is such a transformation, be it direct, or if both vertical CRS
   relate to a common intermediate CRS.
   If it has a registered interpolation geographic CRS, then it is used.
   Otherwise we fallback to the geographic CRS of the source CRS.

   Finally, a 3-level loop to create the final set of operations chaining together:

   - the horizontal transformation from the source CRS to the interpolation CRS
   - the vertical transformation
   - the horizontal transformation from the interpolation CRS to the target CRS.

    Example:

    .. code-block:: shell

        $ projinfo -s "NAD27 + NGVD29 height (ftUS)" -t "NAD83 + NAVD88 height" --spatial-test intersects --summary

        Candidate operations found: 20
        unknown id, NGVD29 height (ftUS) to NAVD88 height (3) + NAD27 to NAD83 (1), 0.17 m, USA - CONUS east of 89°W - onshore
        unknown id, NGVD29 height (ftUS) to NAVD88 height (2) + NAD27 to NAD83 (1), 0.17 m, USA - CONUS 89°W-107°W - onshore
        unknown id, NGVD29 height (ftUS) to NAVD88 height (1) + NAD27 to NAD83 (1), 0.17 m, USA - CONUS west of 107°W - onshore
        unknown id, NGVD29 height (ftUS) to NAVD88 height (3) + NAD27 to NAD83 (3), 1.02 m, unknown domain of validity
        unknown id, NGVD29 height (ftUS) to NAVD88 height (2) + NAD27 to NAD83 (3), 1.02 m, unknown domain of validity
        unknown id, NGVD29 height (ftUS) to NAVD88 height (1) + NAD27 to NAD83 (3), 1.02 m, unknown domain of validity
        unknown id, NGVD29 height (ftUS) to NAVD88 height (3) + NAD27 to NAD83 (5), 1.02 m, unknown domain of validity, at least one grid missing
        unknown id, NGVD29 height (ftUS) to NAVD88 height (3) + NAD27 to NAD83 (6), 1.52 m, unknown domain of validity, at least one grid missing
        unknown id, NGVD29 height (ftUS) to NAVD88 height (2) + NAD27 to NAD83 (9), 1.52 m, unknown domain of validity, at least one grid missing
        unknown id, NGVD29 height (ftUS) to NAVD88 height (1) + NAD27 to NAD83 (9), 1.52 m, unknown domain of validity, at least one grid missing
        unknown id, NGVD29 height (ftUS) to NAVD88 height (3) + Ballpark geographic offset from NAD27 to NAD83, unknown accuracy, USA - CONUS east of 89°W - onshore, has ballpark transformation
        unknown id, NGVD29 height (ftUS) to NAVD88 height (2) + Ballpark geographic offset from NAD27 to NAD83, unknown accuracy, USA - CONUS 89°W-107°W - onshore, has ballpark transformation
        unknown id, NGVD29 height (ftUS) to NAVD88 height (1) + Ballpark geographic offset from NAD27 to NAD83, unknown accuracy, USA - CONUS west of 107°W - onshore, has ballpark transformation
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (1), unknown accuracy, USA - CONUS including EEZ, has ballpark transformation
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (3), unknown accuracy, Canada, has ballpark transformation
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (4), unknown accuracy, Canada - NAD27, has ballpark transformation
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (5), unknown accuracy, Canada - Quebec, has ballpark transformation, at least one grid missing
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (6), unknown accuracy, Canada - Quebec, has ballpark transformation, at least one grid missing
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + NAD27 to NAD83 (9), unknown accuracy, Canada - Saskatchewan, has ballpark transformation, at least one grid missing
        unknown id, Transformation from NGVD29 height (ftUS) to NAVD88 height (ballpark vertical transformation) + Ballpark geographic offset from NAD27 to NAD83, unknown accuracy, World, has ballpark transformation


2. Otherwise, when there is no such transformation, we decompose into 3 steps:

   - transform from the source CRS to the geographic 3D CRS corresponding to it
   - transform from the geographic 3D CRS corresponding to the source CRS to the
     geographic 3D CRS corresponding to the target CRS
   - transform from the geographic 3D CRS corresponding to the target CRS to the
     target CRS.

    Example:

    .. code-block:: shell

        $  projinfo -s "WGS 84 + EGM96 height" -t "ETRS89 + Belfast height" --spatial-test intersects --summary

        Candidate operations found: 7
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Inverse of ETRS89 to WGS 84 (1) + ETRS89 to Belfast height (2), 2.014 m, UK - Northern Ireland - onshore
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Inverse of ETRS89 to WGS 84 (1) + ETRS89 to Belfast height (1), 2.03 m, UK - Northern Ireland - onshore, at least one grid missing
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Null geographic offset from WGS 84 (geog3D) to WGS 84 (geog2D) + Inverse of OSGB 1936 to WGS 84 (4) + OSGB 1936 to ETRS89 (2) + Null geographic offset from ETRS89 (geog2D) to ETRS89 (geog3D) + ETRS89 to Belfast height (2), 19.044 m, unknown domain of validity
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Null geographic offset from WGS 84 (geog3D) to WGS 84 (geog2D) + Inverse of OSGB 1936 to WGS 84 (2) + OSGB 1936 to ETRS89 (2) + Null geographic offset from ETRS89 (geog2D) to ETRS89 (geog3D) + ETRS89 to Belfast height (2), 11.044 m, unknown domain of validity
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Null geographic offset from WGS 84 (geog3D) to WGS 84 (geog2D) + Inverse of TM75 to WGS 84 (2) + TM75 to ETRS89 (3) + Null geographic offset from ETRS89 (geog2D) to ETRS89 (geog3D) + ETRS89 to Belfast height (2), 2.424 m, UK - Northern Ireland - onshore, at least one grid missing
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Null geographic offset from WGS 84 (geog3D) to WGS 84 (geog2D) + Inverse of TM75 to WGS 84 (2) + TM75 to ETRS89 (3) + Null geographic offset from ETRS89 (geog2D) to ETRS89 (geog3D) + ETRS89 to Belfast height (1), 2.44 m, UK - Northern Ireland - onshore, at least one grid missing
        unknown id, Inverse of WGS 84 to EGM96 height (1) + Null geographic offset from WGS 84 (geog3D) to WGS 84 (geog2D) + Inverse of OSGB 1936 to WGS 84 (4) + OSGB 1936 to ETRS89 (2) + Null geographic offset from ETRS89 (geog2D) to ETRS89 (geog3D) + ETRS89 to Belfast height (1), 19.06 m, unknown domain of validity, at least one grid missing


This is implemented by the ``createOperationsCompoundToCompound`` method

When the source or target CRS is a BoundCRS
---------------------------------------------------------------------------------

The BoundCRS concept is an hybrid concept where a CRS is linked to a transformation
from it to a hub CRS, typically WGS 84. This is a long-time practice in PROJ.4
strings with the ``+towgs84``, ``+nadgrids`` and ``+geoidgrids`` keywords, or the
``TOWGS84[]`` node of WKT 1. When encountering those attributes when parsing
a CRS string, PROJ will create a BoundCRS object capturing this transformation.
A BoundCRS object can also be provided with a WKT2 string, and in that case with
a hub CRS being potentially different from WGS 84.

Let's consider the case of a transformation between a BoundCRS
("+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=400000 +y_0=-100000
+ellps=airy +towgs84=446.448,-125.157,542.06,0.15,0.247,0.842,-20.489 +units=m"
which used to be the PROJ.4 definition of "OSGB 1936 / British National Grid")
and a target Geographic CRS, ETRS89.

We apply the following steps:

- transform from the base of the source CRS (that is the CRS wrapped by BoundCRS,
  here a ProjectedCRS) to the geographic CRS of this base CRS
- apply the transformation of the BoundCRS to go from the geographic CRS of this base CRS
  to the hub CRS of the BoundCRS, in that instance WGS 84.
- apply a transformation from the hub CRS to the target CRS.

This is implemented by the ``createOperationsBoundToGeog`` method

Example:

.. code-block:: shell

    $ projinfo -s "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=400000 +y_0=-100000 +ellps=airy +towgs84=446.448,-125.157,542.06,0.15,0.247,0.842,-20.489 +units=m +type=crs" -t ETRS89 -o PROJ

    Candidate operations found: 1
    -------------------------------------
    Operation No. 1:

    unknown id, Inverse of unknown + Transformation from unknown to WGS84 + Inverse of ETRS89 to WGS 84 (1), unknown accuracy, Europe - ETRS89

    PROJ string:
    +proj=pipeline +step +inv +proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=400000 +y_0=-100000 +ellps=airy +step +proj=push +v_3 +step +proj=cart +ellps=airy +step +proj=helmert +x=446.448 +y=-125.157 +z=542.06 +rx=0.15 +ry=0.247 +rz=0.842 +s=-20.489 +convention=position_vector +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1

There are other situations with BoundCRS, involving vertical transformations,
or transforming to other objects than a geographic CRS, but the curious reader
will have to inspect the code for the actual gory details.
