.. _rfc2:

====================================================================
PROJ RFC 2: Initial integration of "GDAL SRS barn" work
====================================================================

:Author: Even Rouault
:Contact: even.rouault at spatialys.com
:Status: Adopted, implemented in PROJ 6.0
:Initial version: 2018-10-09
:Last Updated: 2018-10-31

Summary
-------

This RFC is the result of a first phase of the `GDAL Coordinate System Barn Raising`_
efforts. In its current state, this work mostly consists of:

  - a C++ implementation of the ISO-19111:2018 / OGC Topic 2 "Referencing by
    coordinates" classes to represent Datums, Coordinate systems, CRSs
    (Coordinate Reference Systems) and Coordinate Operations.
  - methods to convert between this C++ modeling and WKT1, WKT2 and PROJ string representations of those objects
  - management and query of a SQLite3 database of CRS and Coordinate Operation definition
  - a C API binding part of those capabilities

.. _`GDAL Coordinate System Barn Raising`: https://gdalbarn.com/


Related standards
-----------------

Consult `Applicable standards`_

.. _`Applicable standards`: http://even.rouault.free.fr/proj_cpp_api/html/general_doc.html#standards

(They will be linked from the PROJ documentation)


Details
-------

Structure in packages / namespaces
**********************************

The C++ implementation of the (upcoming) ISO-19111:2018 / OGC Topic 2 "Referencing by
coordinates" classes follows this abstract modeling as much as possible, using
package names as C++ namespaces, abstract classes and method names. A new
BoundCRS class has been added to cover the modeling of the WKT2 BoundCRS
construct, that is a generalization of the WKT1 TOWGS84 concept. It is
strongly recommended to have the ISO-19111 standard open to have an introduction
for the concepts when looking at the code. A few classes have also been
inspired by the GeoAPI

The classes are organized into several namespaces:

    - osgeo::proj::util
        A set of base types from ISO 19103, GeoAPI and other PROJ "technical"
        specific classes 

        Template optional<T>, classes BaseObject, IComparable, BoxedValue,
        ArrayOfBaseObject, PropertyMap, LocalName, NameSpace, GenericName,
        NameFactory, CodeList, Exception, InvalidValueTypeException,
        UnsupportedOperationException

    - osgeo::proj::metadata:
        Common classes from ISO 19115 (Metadata) standard 

        Classes Citation, GeographicExtent, GeographicBoundingBox,
        TemporalExtent, VerticalExtent, Extent, Identifier, PositionalAccuracy,

    - osgeo::proj::common: 
        Common classes: UnitOfMeasure, Measure, Scale, Angle, Length, DateTime,
        DateEpoch, IdentifiedObject, ObjectDomain, ObjectUsage

    - osgeo::proj::cs:
        Coordinate systems and their axis 
        
        Classes AxisDirection, Meridian, CoordinateSystemAxis, CoordinateSystem,
        SphericalCS, EllipsoidalCS, VerticalCS, CartesianCS, OrdinalCS,
        ParametricCS, TemporalCS, DateTimeTemporalCS, TemporalCountCS,
        TemporalMeasureCS

    - osgeo::proj::datum:
        Datum (the relationship of a coordinate system to the body) 

        Classes Ellipsoid, PrimeMeridian, Datum, DatumEnsemble,
        GeodeticReferenceFrame, DynamicGeodeticReferenceFrame,
        VerticalReferenceFrame, DynamicVerticalReferenceFrame, TemporalDatum,
        EngineeringDatum, ParametricDatum

    - osgeo::proj::crs:
        CRS = coordinate reference system = coordinate system with a datum

        Classes CRS, GeodeticCRS, GeographicCRS, DerivedCRS, ProjectedCRS,
        VerticalCRS, CompoundCRS, BoundCRS, TemporalCRS, EngineeringCRS,
        ParametricCRS, DerivedGeodeticCRS, DerivedGeographicCRS,
        DerivedProjectedCRS, DerivedVerticalCRS

    - osgeo::proj::operation:
        Coordinate operations (relationship between any two coordinate
        reference systems) 

        Classes CoordinateOperation, GeneralOperationParameter,
        OperationParameter, GeneralParameterValue, ParameterValue,
        OperationParameterValue, OperationMethod, InvalidOperation,
        SingleOperation, Conversion, Transformation, PointMotionOperation,
        ConcatenatedOperation 

    - osgeo::proj::io:
        I/O classes: WKTFormatter, PROJStringFormatter, FormattingException,
        ParsingException, IWKTExportable, IPROJStringExportable, WKTNode,
        WKTParser, PROJStringParser, DatabaseContext, AuthorityFactory,
        FactoryException, NoSuchAuthorityCodeException

What does what?
***************

The code to parse WKT and PROJ strings and build ISO-19111 objects is
contained in `io.cpp`_

The code to format WKT and PROJ strings from ISO-19111 objects is mostly
contained in the related exportToWKT() and exportToPROJString() methods
overridden in the applicable classes. `io.cpp`_ contains the general mechanics
to build such strings.

Regarding WKT strings, three variants are handled in import and export:

    - WKT2_2018: variant corresponding to the upcoming ISO-19162:2018 standard

    - WKT2_2015: variant corresponding to the current ISO-19162:2015 standard

    - WKT1_GDAL: variant corresponding to the way GDAL understands the OGC
      01-099 and OGC 99-049 standards

Regarding PROJ strings, two variants are handled in import and export:

    - PROJ5: variant used by PROJ >= 5, possibly using pipeline constructs,
      and avoiding +towgs84 / +nadgrids legacy constructs. This variant honours
      axis order and input/output units. That is the pipeline for the conversion
      of EPSG:4326 to EPSG:32631 will assume that the input coordinates are in
      latitude, longitude order, with degrees.

    - PROJ4: variant used by PROJ 4.x

The raw query of the proj.db database and the upper level construction of
ISO-19111 objects from the database contents is done in `factory.cpp`_

A few design principles
***********************

Methods generally take and return xxxNNPtr objects, that is non-null shared
pointers (pointers with internal reference counting). The advantage of this
approach is that the user has not to care about the life-cycle of the
instances (and this makes the code leak-free by design). The only point of
attention is to make sure no reference cycles are made. This is the case for
all classes, except the CoordinateOperation class that point to CRS for
sourceCRS and targetCRS members, whereas DerivedCRS point to a Conversion
instance (which derives from CoordinateOperation). This issue was detected in
the ISO-19111 standard. The solution adopted here is to use std::weak_ptr
in the CoordinateOperation class to avoid the cycle. This design artefact is
transparent to users.

Another important design point is that all ISO19111 objects are immutable after
creation, that is they only have getters that do not modify their states.
Consequently they could possibly use in a thread-safe way. There are however
classes like PROJStringFormatter, WKTFormatter, DatabaseContext, AuthorityFactory
and CoordinateOperationContext whose instances are mutable and thus can not be
used by multiple threads at once.

Example how to build the EPSG:4326 / WGS84 Geographic2D definition from scratch:

::

    auto greenwich = PrimeMeridian::create(
        util::PropertyMap()
            .set(metadata::Identifier::CODESPACE_KEY,
                metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, 8901)
            .set(common::IdentifiedObject::NAME_KEY, "Greenwich"),
        common::Angle(0));
    // actually predefined as PrimeMeridian::GREENWICH constant

    auto ellipsoid = Ellipsoid::createFlattenedSphere(
        util::PropertyMap()
            .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, 7030)
            .set(common::IdentifiedObject::NAME_KEY, "WGS 84"),
        common::Length(6378137),
        common::Scale(298.257223563));
    // actually predefined as Ellipsoid::WGS84 constant

    auto datum = GeodeticReferenceFrame::create(
        util::PropertyMap()
            .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, 6326)
            .set(common::IdentifiedObject::NAME_KEY, "World Geodetic System 1984");
        ellipsoid
        util::optional<std::string>(), // anchor
        greenwich);
    // actually predefined as GeodeticReferenceFrame::EPSG_6326 constant

    auto geogCRS = GeographicCRS::create(
        util::PropertyMap()
            .set(metadata::Identifier::CODESPACE_KEY, metadata::Identifier::EPSG)
            .set(metadata::Identifier::CODE_KEY, 4326)
            .set(common::IdentifiedObject::NAME_KEY, "WGS 84"),
        datum,
        cs::EllipsoidalCS::createLatitudeLongitude(scommon::UnitOfMeasure::DEGREE));
    // actually predefined as GeographicCRS::EPSG_4326 constant

Algorithmic focus
*****************

On the algorithmic side, a somewhat involved logic is the
CoordinateOperationFactory::createOperations() in `coordinateoperation.cpp`_
that takes a pair of source and target CRS and returns a set of possible
`coordinate operations`_ (either single operations like a Conversion or a
Transformation, or concatenated operations). It uses the intrinsic structure
of those objects to create the coordinate operation pipeline. That is, if
going from a ProjectedCRS to another one, by doing first the inverse conversion
from the source ProjectedCRS to its base GeographicCRS, then finding the
appropriate transformation(s) from this base GeographicCRS to the base
GeographicCRS of the target CRS, and then applying the conversion from this
base GeographicCRS to the target ProjectedCRS. At each step, it queries the
database to find if one or several transformations are available. The
resulting coordinate operations are filtered, and sorted, with user provided hints:

    - desired accuracy
    - area of use, defined as a bounding box in longitude, latitude space (its
      actual CRS does not matter for the intended use)
    - if no area of use is defined, if and how the area of use of the source
      and target CRS should be used. By default, the smallest area of use is
      used. The rationale is for example when transforming between a national
      ProjectedCRS and a world-scope GeographicCRS to use the are of use of
      this ProjectedCRS to select the appropriate datum shifts.
    - how the area of use of the candidate transformations and the desired area of
      use (either explicitly or implicitly defined, as explained above) are
      compared. By default, only transformations whose area of use is fully
      contained in the desired area of use are selected. It is also possible
      to relax this test by specifying that only an intersection test must be used.
    - whether `PROJ transformation grid`_ names should be substituted to the
      official names, when a match is found in the `grid_alternatives` table
      of the database. Defaults to true
    - whether the availability of those grids should be used to filter and sort
      the results. By default, the transformations using grids available in the
      system will be presented first.

The results are sorted, with the most relevant ones appearing first in the
result vector. The criteria used are in that order

    - grid actual availability: operations referencing grids not available will be
      listed after ones with available grids
    - grid potential availability: operation referencing grids not known at
      all in the proj.db will be listed after operations with grids known, but
      not available.
    - known accuracy: operations with unknown accuracies will be listed
      after operations with known accuracy
    - area of use: operations with smaller area of use (the intersection of the
      operation area of used with the desired area of use) will be listed after
      the ones with larger area of use
    - accuracy: operations with lower accuracy will be listed after operations
      with higher accuracy (caution: lower accuracy actually means a higher numeric
      value of the accuracy property, since it is a precision in metre)

All those settings can be specified in the CoordinateOperationContext instance
passed to createOperations().

An interesting example to understand how those parameters play together is
to use `projinfo -s EPSG:4267 -t EPSG:4326` (NAD27 to WGS84 conversions),
and see how specifying desired area of use, spatial criterion, grid availability,
etc. affects the results.

The following command currently returns 78 results:

::

    projinfo -s EPSG:4267 -t EPSG:4326 --summary --spatial-test intersects

The createOperations() algorithm also does a kind of "CRS routing".
A typical example is if wanting to transform between CRS A and
CRS B, but no direct transformation is referenced in proj.db between those.
But if there are transformations between A <--> C and B <--> C, then it
is possible to build a concatenated operation A --> C --> B. The typical
example is when C is WGS84, but the implementation is generic and just finds
a common pivot from the database. An example of finding a non-WGS84 pivot is
when searching a transformation between EPSG:4326 and EPSG:6668 (JGD2011 -
Japanese Geodetic Datum 2011), which has no direct transformation registered
in the EPSG database . However there are transformations between those two
CRS and JGD2000 (and also Tokyo datum, but that one involves less accurate
transformations)

::

    projinfo -s EPSG:4326 -t EPSG:6668  --grid-check none --bbox 135.42,34.84,142.14,41.58 --summary

    Candidate operations found: 7
    unknown id, Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (1), 1.2 m, Japan - northern Honshu
    unknown id, Inverse of JGD2000 to WGS 84 (1) + JGD2000 to JGD2011 (2), 2 m, Japan excluding northern main province
    unknown id, Inverse of Tokyo to WGS 84 (108) + Tokyo to JGD2011 (2), 9.2 m, Japan onshore excluding northern main province
    unknown id, Inverse of Tokyo to WGS 84 (108) + Tokyo to JGD2000 (2) + JGD2000 to JGD2011 (1), 9.4 m, Japan - northern Honshu
    unknown id, Inverse of Tokyo to WGS 84 (2) + Tokyo to JGD2011 (2), 13.2 m, Japan - onshore mainland and adjacent islands
    unknown id, Inverse of Tokyo to WGS 84 (2) + Tokyo to JGD2000 (2) + JGD2000 to JGD2011 (1), 13.4 m, Japan - northern Honshu
    unknown id, Inverse of Tokyo to WGS 84 (1) + Tokyo to JGD2011 (2), 29.2 m, Asia - Japan and South Korea


.. _`coordinate operations`: https://proj.org/operations/index.html

.. _`PROJ transformation grid`: https://proj.org/resource_files.html#transformation-grids


Code repository
---------------

The current state of the work can be found in the
`iso19111 branch of rouault/proj.4 repository`_ , and is also available as
a GitHub pull request at https://github.com/OSGeo/proj.4/pull/1040

Here is a not-so-usable `comparison with a fixed snapshot of master branch`_

.. _`iso19111 branch of rouault/proj.4 repository`: https://github.com/rouault/proj.4/tree/iso19111

.. _`comparison with a fixed snapshot of master branch`: https://github.com/OSGeo/proj.4/compare/iso19111_dev...rouault:iso19111


Database
--------

Content
*******

The database contains CRS and coordinate operation definitions from the `EPSG`_
database (IOGP’s EPSG Geodetic Parameter Dataset) v9.5.3,
`IGNF registry`_ (French National Geographic Institute), ESRI database, as well
as a few customizations.

.. _`EPSG`: http://www.epsg.org/
.. _`IGNF registry`: https://geodesie.ign.fr/index.php?page=documentation#titre3

Building (for PROJ developers creating the database)
****************************************************

The building of the database is a several stage process:

Construct SQL scripts for EPSG
++++++++++++++++++++++++++++++

The first stage consists in constructing .sql scripts mostly with
CREATE TABLE and INSERT statements to create the database structure and
populate it. There is one .sql file for each database table, populated
with the content of the EPSG database, automatically
generated with the `build_db.py`_ script, which processes the PostgreSQL
dumps issued by IOGP. A number of other scripts are dedicated to manual
editing, for example `grid_alternatives.sql`_ file that binds official
grid names to PROJ grid names

Concert UTF8 SQL to sqlite3 db
++++++++++++++++++++++++++++++

The second stage is done automatically by the make process. It pipes the
.sql script, in the right order, to the sqlite3 binary to generate a
first version of the proj.db SQLite3 database.

Add extra registries
++++++++++++++++++++

The third stage consists in creating additional .sql files from the
content of other registries. For that process, we need to bind some
definitions of those registries to those of the EPSG database, to be
able to link to existing objects and detect some boring duplicates.
The `ignf.sql`_ file has been generated using
the `build_db_create_ignf.py`_ script from the current data/IGNF file
that contains CRS definitions (and implicit transformations to WGS84)
as PROJ.4 strings.
The `esri.sql`_ file has been generated using the `build_db_from_esri.py`_
script, from the .csv files in
https://github.com/Esri/projection-engine-db-doc/tree/master/csv

Finalize proj.db
++++++++++++++++

The last stage runs make again to incorporate the new .sql files generated
in the previous stage (so the process of building the database involves
a kind of bootstrapping...)

Building (for PROJ users)
*************************

The make process just runs the second stage mentioned above from the .sql
files. The resulting proj.db is currently 5.3 MB large.

Structure
*********

The database is structured into the following tables and views. They generally
match a ISO-19111 concept, and is generally close to the general structure of
the EPSG database. Regarding identification of objects, where the EPSG database
only contains a 'code' numeric column, the PROJ database identifies objects
with a (auth_name, code) tuple of string values, allowing several registries to
be combined together.

- Technical:
    - `authority_list`: view enumerating the authorities present in the database. Currently: EPSG, IGNF, PROJ
    - `metadata`: a few key/value pairs, for example to indicate the version of the registries imported in the database
    - `object_view`: synthetic view listing objects (ellipsoids, datums, CRS, coordinate operations...) code and name, and the table name where they are further described
    - `alias_names`: list possible alias for the `name` field of object table
    - `link_from_deprecated_to_non_deprecated`: to handle the link between old ESRI to new ESRI/EPSG codes

- Common:
    - `unit_of_measure`: table with UnitOfMeasure definitions.
    - `area`: table with area-of-use (bounding boxes) applicable to CRS and coordinate operations.

- Coordinate systems:
    - `axis`: table with CoordinateSystemAxis definitions.
    - `coordinate_system`: table with CoordinateSystem definitions.

- Ellipsoid and datums:
    - `ellipsoid`: table with ellipsoid definitions.
    - `prime_meridian`: table with PrimeMeridian definitions.
    - `geodetic_datum`: table with GeodeticReferenceFrame definitions.
    - `vertical_datum`: table with VerticalReferenceFrame definitions.

- CRS:
    - `geodetic_crs`: table with GeodeticCRS and GeographicCRS definitions.
    - `projected_crs`: table with ProjectedCRS definitions.
    - `vertical_crs`: table with VerticalCRS definitions.
    - `compound_crs`: table with CompoundCRS definitions.

- Coordinate operations:
    - `coordinate_operation_view`: view giving a number of common attributes shared by the concrete tables implementing CoordinateOperation
    - `conversion`: table with definitions of Conversion (mostly parameter and values of Projection)
    - `concatenated_operation`: table with definitions of ConcatenatedOperation.
    - `grid_transformation`: table with all grid-based transformations.
    - `grid_packages`: table listing packages in which grids can be found. ie "proj-datumgrid", "proj-datumgrid-europe", ...
    - `grid_alternatives`: table binding official grid names to PROJ grid names. e.g "Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree.gz" --> "egm08_25.gtx"
    - `helmert_transformation`: table with all Helmert-based transformations.
    - `other_transformation`: table with other type of transformations.

The main departure with the structure of the EPSG database is the split of
the various coordinate operations over several tables. This was done mostly
for human-readability as the EPSG organization of coordoperation,
coordoperationmethod, coordoperationparam, coordoperationparamusage,
coordoperationparamvalue tables makes it hard to grasp at once all the
parameters and values for a given operation.


Utilities
---------

A new `projinfo` utility has been added. It enables the user to enter a CRS or
coordinate operation by a AUTHORITY:CODE, PROJ string or WKT string, and see
it translated in the different flavors of PROJ and WKT strings.
It also enables to build coordinate operations between two CRSs.

Usage
*****

::

    usage: projinfo [-o formats] [-k crs|operation] [--summary] [-q]
                    [--bbox min_long,min_lat,max_long,max_lat] 
                    [--spatial-test contains|intersects]
                    [--crs-extent-use none|both|intersection|smallest]
                    [--grid-check none|discard_missing|sort]
                    [--boundcrs-to-wgs84]
                    {object_definition} | (-s {srs_def} -t {srs_def})

    -o: formats is a comma separated combination of: all,default,PROJ4,PROJ,WKT_ALL,WKT2_2015,WKT2_2018,WKT1_GDAL
        Except 'all' and 'default', other format can be preceded by '-' to disable them

Examples
********

Specify CRS by AUTHORITY:CODE
+++++++++++++++++++++++++++++

::

    $ projinfo EPSG:4326

    PROJ string: 
    +proj=pipeline +step +proj=longlat +ellps=WGS84 +step +proj=unitconvert +xy_in=rad +xy_out=deg +step +proj=axisswap +order=2,1

    WKT2_2015 string: 
    GEODCRS["WGS 84",
        DATUM["World Geodetic System 1984",
            ELLIPSOID["WGS 84",6378137,298.257223563,
                LENGTHUNIT["metre",1]]],
        PRIMEM["Greenwich",0,
            ANGLEUNIT["degree",0.0174532925199433]],
        CS[ellipsoidal,2],
            AXIS["geodetic latitude (Lat)",north,
                ORDER[1],
                ANGLEUNIT["degree",0.0174532925199433]],
            AXIS["geodetic longitude (Lon)",east,
                ORDER[2],
                ANGLEUNIT["degree",0.0174532925199433]],
        AREA["World"],
        BBOX[-90,-180,90,180],
        ID["EPSG",4326]]


Specify CRS by PROJ string and specify output formats
++++++++++++++++++++++++++++++++++++++++++++++++++++++

::

    $ projinfo -o PROJ4,PROJ,WKT1_GDAL,WKT2_2018 "+title=IGN 1972 Nuku Hiva - UTM fuseau 7 Sud +proj=tmerc +towgs84=165.7320,216.7200,180.5050,-0.6434,-0.4512,-0.0791,7.420400 +a=6378388.0000 +rf=297.0000000000000 +lat_0=0.000000000 +lon_0=-141.000000000 +k_0=0.99960000 +x_0=500000.000 +y_0=10000000.000 +units=m +no_defs"

    PROJ string: 
    Error when exporting to PROJ string: BoundCRS cannot be exported as a PROJ.5 string, but its baseCRS might

    PROJ.4 string: 
    +proj=utm +zone=7 +south +ellps=intl +towgs84=165.732,216.72,180.505,-0.6434,-0.4512,-0.0791,7.4204

    WKT2_2018 string: 
    BOUNDCRS[
        SOURCECRS[
            PROJCRS["IGN 1972 Nuku Hiva - UTM fuseau 7 Sud",
                BASEGEOGCRS["unknown",
                    DATUM["unknown",
                        ELLIPSOID["International 1909 (Hayford)",6378388,297,
                            LENGTHUNIT["metre",1,
                                ID["EPSG",9001]]]],
                    PRIMEM["Greenwich",0,
                        ANGLEUNIT["degree",0.0174532925199433],
                        ID["EPSG",8901]]],
                CONVERSION["unknown",
                    METHOD["Transverse Mercator",
                        ID["EPSG",9807]],
                    PARAMETER["Latitude of natural origin",0,
                        ANGLEUNIT["degree",0.0174532925199433],
                        ID["EPSG",8801]],
                    PARAMETER["Longitude of natural origin",-141,
                        ANGLEUNIT["degree",0.0174532925199433],
                        ID["EPSG",8802]],
                    PARAMETER["Scale factor at natural origin",0.9996,
                        SCALEUNIT["unity",1],
                        ID["EPSG",8805]],
                    PARAMETER["False easting",500000,
                        LENGTHUNIT["metre",1],
                        ID["EPSG",8806]],
                    PARAMETER["False northing",10000000,
                        LENGTHUNIT["metre",1],
                        ID["EPSG",8807]]],
                CS[Cartesian,2],
                    AXIS["(E)",east,
                        ORDER[1],
                        LENGTHUNIT["metre",1,
                            ID["EPSG",9001]]],
                    AXIS["(N)",north,
                        ORDER[2],
                        LENGTHUNIT["metre",1,
                            ID["EPSG",9001]]]]],
        TARGETCRS[
            GEOGCRS["WGS 84",
                DATUM["World Geodetic System 1984",
                    ELLIPSOID["WGS 84",6378137,298.257223563,
                        LENGTHUNIT["metre",1]]],
                PRIMEM["Greenwich",0,
                    ANGLEUNIT["degree",0.0174532925199433]],
                CS[ellipsoidal,2],
                    AXIS["latitude",north,
                        ORDER[1],
                        ANGLEUNIT["degree",0.0174532925199433]],
                    AXIS["longitude",east,
                        ORDER[2],
                        ANGLEUNIT["degree",0.0174532925199433]],
                ID["EPSG",4326]]],
        ABRIDGEDTRANSFORMATION["Transformation from unknown to WGS84",
            METHOD["Position Vector transformation (geog2D domain)",
                ID["EPSG",9606]],
            PARAMETER["X-axis translation",165.732,
                ID["EPSG",8605]],
            PARAMETER["Y-axis translation",216.72,
                ID["EPSG",8606]],
            PARAMETER["Z-axis translation",180.505,
                ID["EPSG",8607]],
            PARAMETER["X-axis rotation",-0.6434,
                ID["EPSG",8608]],
            PARAMETER["Y-axis rotation",-0.4512,
                ID["EPSG",8609]],
            PARAMETER["Z-axis rotation",-0.0791,
                ID["EPSG",8610]],
            PARAMETER["Scale difference",1.0000074204,
                ID["EPSG",8611]]]]

    WKT1_GDAL: 
    PROJCS["IGN 1972 Nuku Hiva - UTM fuseau 7 Sud",
        GEOGCS["unknown",
            DATUM["unknown",
                SPHEROID["International 1909 (Hayford)",6378388,297],
                TOWGS84[165.732,216.72,180.505,-0.6434,-0.4512,-0.0791,7.4204]],
            PRIMEM["Greenwich",0,
                AUTHORITY["EPSG","8901"]],
            UNIT["degree",0.0174532925199433,
                AUTHORITY["EPSG","9122"]],
            AXIS["Longitude",EAST],
            AXIS["Latitude",NORTH]],
        PROJECTION["Transverse_Mercator"],
        PARAMETER["latitude_of_origin",0],
        PARAMETER["central_meridian",-141],
        PARAMETER["scale_factor",0.9996],
        PARAMETER["false_easting",500000],
        PARAMETER["false_northing",10000000],
        UNIT["metre",1,
            AUTHORITY["EPSG","9001"]],
        AXIS["Easting",EAST],
        AXIS["Northing",NORTH]]


Find transformations between 2 CRS
++++++++++++++++++++++++++++++++++

Between "Poland zone I" (based on Pulkovo 42 datum) and "UTM WGS84 zone 34N"

Summary view:

::

    $ projinfo -s EPSG:2171 -t EPSG:32634 --summary

    Candidate operations found: 1
    unknown id, Inverse of Poland zone I + Pulkovo 1942(58) to WGS 84 (1) + UTM zone 34N, 1 m, Poland - onshore

Display of pipelines:

::

    $ PROJ_LIB=data src/projinfo -s EPSG:2171 -t EPSG:32634 -o PROJ

    PROJ string: 
    +proj=pipeline +step +proj=axisswap +order=2,1 +step +inv +proj=sterea +lat_0=50.625 +lon_0=21.0833333333333 +k=0.9998 +x_0=4637000 +y_0=5647000 +ellps=krass +step +proj=cart +ellps=krass +step +proj=helmert +x=33.4 +y=-146.6 +z=-76.3 +rx=-0.359 +ry=-0.053 +rz=0.844 +s=-0.84 +convention=position_vector +step +inv +proj=cart +ellps=WGS84 +step +proj=utm +zone=34 +ellps=WGS84


Impacted files
--------------

New files (excluding makefile.am, CMakeLists.txt and other build infrastructure
artefacts):

    * include/proj/: Public installed C++ headers
        - `common.hpp`_: declarations of osgeo::proj::common namespace.
        - `coordinateoperation.hpp`_:  declarations of osgeo::proj::operation namespace.
        - `coordinatesystem.hpp`_:  declarations of osgeo::proj::cs namespace.
        - `crs.hpp`_:  declarations of osgeo::proj::crs namespace.
        - `datum.hpp`_:  declarations of osgeo::proj::datum namespace.
        - `io.hpp`_:  declarations of osgeo::proj::io namespace.
        - `metadata.hpp`_: declarations of osgeo::proj::metadata namespace.
        - `util.hpp`_: declarations of osgeo::proj::util namespace.
        - `nn.hpp`_: Code from https://github.com/dropbox/nn to manage Non-nullable pointers for C++

    .. _`common.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/common.hpp
    .. _`coordinateoperation.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/coordinateoperation.hpp
    .. _`coordinatesystem.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/coordinatesystem.hpp
    .. _`crs.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/crs.hpp
    .. _`datum.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/datum.hpp
    .. _`io.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/io.hpp
    .. _`metadata.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/metadata.hpp
    .. _`nn.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/nn.hpp
    .. _`util.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/util.hpp

    * include/proj/internal: Private non-installed C++ headers
        - `coordinateoperation_internal.hpp`_: classes InverseCoordinateOperation, InverseConversion, InverseTransformation, PROJBasedOperation, and functions to get conversion mappings between WKT and PROJ syntax
        - `coordinateoperation_constants.hpp`_: Select subset of conversion/transformation EPSG names and codes for the purpose of translating them to PROJ strings
        - `coordinatesystem_internal.hpp`_: classes AxisDirectionWKT1, AxisName and AxisAbbreviation
        - `internal.hpp`_: a few helper functions, mostly to do string-based operations
        - `io_internal.hpp`_: class WKTConstants
        - `helmert_constants.hpp`_: Helmert-based transformation & parameters names and codes.
        - `lru_cache.hpp`_: code from https://github.com/mohaps/lrucache11 to have a generic Least-Recently-Used cache of objects

    .. _`coordinateoperation_internal.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/coordinateoperation_internal.hpp
    .. _`coordinatesystem_internal.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/coordinatesystem_internal.hpp
    .. _`internal.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/internal.hpp
    .. _`io_internal.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/io_internal.hpp
    .. _`coordinateoperation_constants.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/coordinateoperation_constants.hpp
    .. _`helmert_constants.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/helmert_constants.hpp
    .. _`lru_cache.hpp`: https://github.com/rouault/proj.4/blob/iso19111/include/proj/internal/lru_cache.hpp

    * src/:
        - `c_api.cpp`_: C++ API mapped to C functions
        - `common.cpp`_: implementation of `common.hpp`_
        - `coordinateoperation.cpp`_: implementation of `coordinateoperation.hpp`_
        - `coordinatesystem.cpp`_: implementation of `coordinatesystem.hpp`_
        - `crs.cpp`_: implementation of `crs.hpp`_
        - `datum.cpp`_: implementation of `datum.hpp`_
        - `factory.cpp`_: implementation of AuthorityFactory class (from `io.hpp`_)
        - `internal.cpp`_: implementation of `internal.hpp`_
        - `io.cpp`_: implementation of `io.hpp`_
        - `metadata.cpp`_: implementation of `metadata.hpp`_
        - `static.cpp`_: a number of static constants (like pre-defined well-known ellipsoid, datum and CRS), put in the right order for correct static initializations
        - `util.cpp`_: implementation of `util.hpp`_
        - `projinfo.cpp`_: new 'projinfo' binary
        - `general.dox`_: generic introduction documentation.

    .. _`c_api.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/c_api.cpp
    .. _`common.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/common.cpp
    .. _`coordinateoperation.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/coordinateoperation.cpp
    .. _`coordinatesystem.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/coordinatesystem.cpp
    .. _`crs.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/crs.cpp
    .. _`datum.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/datum.cpp
    .. _`factory.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/factory.cpp
    .. _`internal.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/internal.cpp
    .. _`io.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/io.cpp
    .. _`metadata.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/metadata.cpp
    .. _`projinfo.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/projinfo.cpp
    .. _`static.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/static.cpp
    .. _`util.cpp`: https://github.com/rouault/proj.4/blob/iso19111/src/util.cpp
    .. _`general.dox`: https://github.com/rouault/proj.4/blob/iso19111/src/general.dox

    * data/sql/:
        - `area.sql`_: generated by `build_db.py`_
        - `axis.sql`_: generated by `build_db.py`_
        - `begin.sql`_: hand generated (trivial)
        - `commit.sql`_: hand generated (trivial)
        - `compound_crs.sql`_: generated by `build_db.py`_
        - `concatenated_operation.sql`_: generated by `build_db.py`_
        - `conversion.sql`_: generated by `build_db.py`_
        - `coordinate_operation.sql`_: generated by `build_db.py`_
        - `coordinate_system.sql`_: generated by `build_db.py`_
        - `crs.sql`_: generated by `build_db.py`_
        - `customizations.sql`_: hand generated (empty)
        - `ellipsoid.sql`_: generated by `build_db.py`_
        - `geodetic_crs.sql`_: generated by `build_db.py`_
        - `geodetic_datum.sql`_: generated by `build_db.py`_
        - `grid_alternatives.sql`_: hand-generated. Contains links between official registry grid names and PROJ ones
        - `grid_transformation.sql`_: generated by `build_db.py`_
        - `grid_transformation_custom.sql`_: hand-generated
        - `helmert_transformation.sql`_: generated by `build_db.py`_
        - `ignf.sql`_: generated by `build_db_create_ignf.py`_
        - `esri.sql`_: generated by `build_db_from_esri.py`_
        - `metadata.sql`_: hand-generated
        - `other_transformation.sql`_: generated by `build_db.py`_
        - `prime_meridian.sql`_: generated by `build_db.py`_
        - `proj_db_table_defs.sql`_: hand-generated. Database structure: CREATE TABLE / CREATE VIEW / CREATE TRIGGER
        - `projected_crs.sql`_: generated by `build_db.py`_
        - `unit_of_measure.sql`_: generated by `build_db.py`_
        - `vertical_crs.sql`_: generated by `build_db.py`_
        - `vertical_datum.sql`_: generated by `build_db.py`_

    .. _`area.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/area.sql
    .. _`axis.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/axis.sql
    .. _`begin.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/begin.sql
    .. _`commit.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/commit.sql
    .. _`compound_crs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/compound_crs.sql
    .. _`concatenated_operation.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/concatenated_operation.sql
    .. _`conversion.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/conversion.sql
    .. _`coordinate_operation.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/coordinate_operation.sql
    .. _`coordinate_system.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/coordinate_system.sql
    .. _`crs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/crs.sql
    .. _`customizations.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/customizations.sql
    .. _`ellipsoid.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/ellipsoid.sql
    .. _`geodetic_crs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/geodetic_crs.sql
    .. _`geodetic_datum.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/geodetic_datum.sql
    .. _`grid_alternatives.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/grid_alternatives.sql
    .. _`grid_transformation_custom.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/grid_transformation_custom.sql
    .. _`grid_transformation.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/grid_transformation.sql
    .. _`helmert_transformation.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/helmert_transformation.sql
    .. _`ignf.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/ignf.sql
    .. _`esri.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/esri.sql
    .. _`metadata.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/metadata.sql
    .. _`other_transformation.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/other_transformation.sql
    .. _`prime_meridian.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/prime_meridian.sql
    .. _`proj_db_table_defs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/proj_db_table_defs.sql
    .. _`projected_crs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/projected_crs.sql
    .. _`unit_of_measure.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/unit_of_measure.sql
    .. _`vertical_crs.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/vertical_crs.sql
    .. _`vertical_datum.sql`: https://github.com/rouault/proj.4/blob/iso19111/data/sql/vertical_datum.sql

    * scripts/:
        - `build_db.py`_ : generate .sql files from EPSG database dumps
        - `build_db_create_ignf.py`_: generates data/sql/`ignf.sql`_
        - `build_db_from_esri.py`_: generates data/sql/`esri.sql`_
        - `doxygen.sh`_: generates Doxygen documentation
        - `gen_html_coverage.sh`_: generates HTML report of the coverage for --coverage build
        - `filter_lcov_info.py`_: utility used by gen_html_coverage.sh
        - `reformat.sh`_: used by reformat_cpp.sh
        - `reformat_cpp.sh`_: reformat all .cpp/.hpp files according to LLVM-style formatting rules

    .. _`build_db.py`: https://github.com/rouault/proj.4/blob/iso19111/scripts/build_db.py
    .. _`build_db_create_ignf.py`: https://github.com/rouault/proj.4/blob/iso19111/scripts/build_db_create_ignf.py
    .. _`build_db_from_esri.py`: https://github.com/rouault/proj.4/blob/iso19111/scripts/build_db_from_esri.py
    .. _`doxygen.sh`: https://github.com/rouault/proj.4/blob/iso19111/scripts/doxygen.sh
    .. _`gen_html_coverage.sh`: https://github.com/rouault/proj.4/blob/iso19111/scripts/gen_html_coverage.sh
    .. _`filter_lcov_info.py`: https://github.com/rouault/proj.4/blob/iso19111/scripts/filter_lcov_info.py
    .. _`reformat.sh`: https://github.com/rouault/proj.4/blob/iso19111/scripts/reformat.sh
    .. _`reformat_cpp.sh`: https://github.com/rouault/proj.4/blob/iso19111/scripts/reformat_cpp.sh

    * tests/unit/
        - `test_c_api.cpp`_: test of src/c_api.cpp
        - `test_common.cpp`_: test of src/common.cpp
        - `test_util.cpp`_: test of src/util.cpp
        - `test_crs.cpp`_: test of src/crs.cpp
        - `test_datum.cpp`_: test of src/datum.cpp
        - `test_factory.cpp`_: test of src/factory.cpp
        - `test_io.cpp`_: test of src/io.cpp
        - `test_metadata.cpp`_: test of src/metadata.cpp
        - `test_operation.cpp`_: test of src/operation.cpp

    .. _`test_c_api.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_c_api.cpp
    .. _`test_common.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_common.cpp
    .. _`test_util.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_util.cpp
    .. _`test_crs.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_crs.cpp
    .. _`test_datum.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_datum.cpp
    .. _`test_factory.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_factory.cpp
    .. _`test_io.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_io.cpp
    .. _`test_metadata.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_metadata.cpp
    .. _`test_operation.cpp`: https://github.com/rouault/proj.4/blob/iso19111/test/unit/test_operation.cpp

C API
-----

`proj.h`_ has been extended to bind a number of C++ classes/methods to a C API.

The main structure is an opaque PJ_OBJ* roughly encapsulating a osgeo::proj::BaseObject,
that can represent a CRS or a CoordinateOperation object. A number of the
C functions will work only if the right type of underlying C++ object is used
with them. Misuse will be properly handled at runtime. If a user passes
a PJ_OBJ* representing a coordinate operation to a pj_obj_crs_xxxx() function,
it will properly error out. This design has been chosen over creating a
dedicate PJ_xxx object for each C++ class, because such an approach would
require adding many conversion and free functions for little benefit.

This C API is incomplete. In particular, it does not allow to
build ISO19111 objects at hand. However it currently permits a number of
actions:

    - building CRS and coordinate operations from WKT and PROJ strings, or
      from the proj.db database
    - exporting CRS and coordinate operations as WKT and PROJ strings
    - querying main attributes of those objects
    - finding coordinate operations between two CRS.

`test_c_api.cpp`_ should demonstrates simple usage of the API (note:
for the conveniency of writing the tests in C++, test_c_api.cpp wraps the C PJ_OBJ*
instances in C++ 'keeper' objects that automatically call the pj_obj_unref()
function at function end. In a pure C use, the caller must use pj_obj_unref()
to prevent leaks.)

.. _`proj.h`: http://even.rouault.free.fr/proj_cpp_api/html/proj_8h.html


Documentation
-------------

All public C++ classes and methods and C functions are documented with
Doxygen.

`Current snapshot of Class list`_

`Spaghetti inheritance diagram`_

.. _`Current snapshot of Class list`: http://even.rouault.free.fr/proj_cpp_api/html/annotated.html
.. _`Spaghetti inheritance diagram`: http://even.rouault.free.fr/proj_cpp_api/html/inherits.html

A basic integration of the Doxygen XML output into the general PROJ
documentation (using reStructuredText format) has been done with the
Sphinx extension `Breathe`_, producing:

    * `One section with the C++ API`_
    * `One section with the C API`_

.. _`Breathe`: https://breathe.readthedocs.io/en/latest/
.. _`One section with the C++ API`: http://even.rouault.free.fr/proj_cpp_api/rst_generated/html/development/reference/cpp/index.html
.. _`One section with the C API`: http://even.rouault.free.fr/proj_cpp_api/rst_generated/html/development/reference/functions.html#c-api-for-iso-19111-functionality

Testing
-------

Nearly all exported methods are tested by a unit test. Global line coverage
of the new files is 92%. Those tests represent 16k lines of codes.


Build requirements
------------------

The new code leverages on a number of C++11 features (auto keyword, constexpr,
initializer list, std::shared_ptr, lambda functions, etc.), which means that
a C++11-compliant compiler must be used to generate PROJ:

    * gcc >= 4.8
    * clang >= 3.3
    * Visual Studio >= 2015.

Compilers tested by the Travis-CI and AppVeyor continuous integration
environments:

    * GCC 4.8
    * mingw-w64-x86-64 4.8
    * clang 5.0
    * Apple LLVM version 9.1.0 (clang-902.0.39.2)
    * MSVC 2015 32 and 64 bit
    * MSVC 2017 32 and 64 bit

The libsqlite3 >= 3.7 development package must also be available. And the sqlite3
binary must be available to build the proj.db files from the .sql files.

Runtime requirements
--------------------

* libc++/libstdc++/MSVC runtime consistent with the compiler used
* libsqlite3 >= 3.7


Backward compatibility
----------------------

At this stage, no backward compatibility issue is foreseen, as no
existing functional C code has been modified to use the new capabilities

Future work
-----------

The work described in this RFC will be pursued in a number of directions.
Non-exhaustively:

  - Support for ESRI WKT1 dialect (PROJ currently ingest the ProjectedCRS in
    `esri.sql`_ in that dialect, but there is no mapping between it and EPSG
    operation and parameter names, so conversion to PROJ strings does not
    always work.

  - closer integration with the existing code base. In particular, the +init=dict:code
    syntax should now go first to the database (then the `epsg` and `IGNF`
    files can be removed). Similarly proj_create_crs_to_crs() could use the
    new capabilities to find an appropriate coordinate transformation.

  - and whatever else changes are needed to address GDAL and libgeotiff needs


Adoption status
---------------

The RFC has been adopted with support from PSC members Kurt Schwehr, Kristian
Evers, Howard Butler and Even Rouault.
