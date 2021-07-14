.. _geodetictiffgrids:

================================================================================
Geodetic TIFF grids (GTG)
================================================================================

.. versionadded:: 7.0

Introduction
++++++++++++

The Geodetic TIFF grid format has been introduced per :ref:`rfc4`. It is a
profile of the TIFF and GeoTIFF formats that addresses the specific requirements
of geodetic grids: horizontal shifts, vertical shifts, velocity grids, etc...
It also follows the `Cloud Optimized GeoTIFF <http://cogeo.org>`_ principles.

Such grids are available on a :ref:`CDN of GeoTIFF grids <cdn_grids>`.

.. _gtg_general_description:

General description
+++++++++++++++++++

The general principles that guide the following requirements and recommendations
are such that files will be properly recognized by PROJ, and also by GDAL which
is an easy way to inspect such grid files:

- `TIFF 6.0 <https://www.awaresystems.be/imaging/tiff/specification/TIFF6.pdf>`_
  based (could possibly be BigTIFF without code changes, if we ever
  need some day to handle grids larger than 4GB)

- `GeoTIFF 1.1 <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html>`_ for the georeferencing.
  GeoTIFF 1.1 is a recent standard, compared to the original GeoTIFF 1.0 version,
  but its backward compatibility is excellent, so that should not cause much trouble
  to readers that are not official GeoTIFF 1.1 compliant.

- Files hosted on the CDN will use a Geographic 2D CRS for the GeoTIFF GeoKeys.
  That CRS is intended to be the interpolation CRS as defined in
  `OGC Abstract Specification Topic 2 <http://docs.opengeospatial.org/as/18-005r4/18-005r4.html>`_,
  that is the CRS to which grid values are referred to.

  Given that they will nominally be related to the EPSG dataset, the `GeodeticCRSGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_geodeticcrsgeokey>`_
  will be used to store the EPSG code of the CRS. If the CRS cannot be reliably
  encoded through that key or other geokeys, the ``interpolation_crs_wkt`` metadata
  item detailed afterwards should be used.

  This CRS will be generally the source CRS (for geographic to
  geographic horizontal shift grids, or geographic to vertical shift grids), but
  for vertical to vertical CRS adjustment, this will be the geographic CRS to
  which the grid is referenced. In some very rare cases of geographic to vertical
  shift grids, the interpolation CRS might be a geographic CRS that is not the
  same as the source CRS (into which ellipsoidal height are expressed). The only
  instance we have in mind is for the EPSG:7001 "ETRS89 to NAP height (1)" transformation
  using the naptrans2008 VDatum-grid which is referenced to Amersfoort EPSG:4289
  instead of ETRS89...

  On the reading side, PROJ will ignore that information:
  the CRS is already stored in the source_crs or interpolation_crs column of the
  grid_transformation table.

  For geographic to vertical shift files (geoid models), the GeoTIFF 1.1
  convention will be used to store the value of the `VerticalGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_verticalgeokey>`_
  So a geoid model that apply to WGS 84 EPSG:4979 will have GeodeticCRSGeoKey = 4326
  and VerticalGeoKey = 4979.

- Files hosted on the CDN will use the GeoTIFF defined `ModelTiepointTag and ModelPixelScaleTag
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_raster_to_model_coordinate_transformation_requirements>`_ TIFF tags
  to store the coordinates of the upper-left pixel and the resolution of the pixels.
  On the reading side, they will be required and ModelTransformationTag will be ignored.

  .. note::

    Regarding anti-meridian handling, a variety of possibilities exist.
    We do not attempt to standardize this and files hosted on the CDN will use
    a georeferencing close to the original data producer.
    For example, NOAA vertical grids that apply to Conterminous USA might even have a top-left
    longitude beyond 180 (for consistency with Alaska grids, whose origin is < 180)
    Anti-meridian handling in PROJ has probably issues. This RFC does not attempt
    to address them in particular, as they are believed to be orthogonal to the
    topics it covers, and being mostly implementation issues.

- Files hosted on the CDN will use the `GTRasterTypeGeoKey
  <http://docs.opengeospatial.org/is/19-008r4/19-008r4.html#_requirements_class_gtrastertypegeokey>`_
  = PixelIsPoint convention.
  This is the convention used by most existing grid formats currently. Note that GDAL
  typically use a PixelIsArea convention (but can handle both conventions), so the
  georeferencing it displays when opening a .gsb or .gtx file appears to have a 
  half-pixel shift regarding to the coordinates stored in the original grid file. On
  the reading side, PROJ will accept both conventions (for equivalent georeferencing,
  the value of the origin in a PixelIsArea convention is shifted by a half-pixel
  towards the upper-left direction). Unspecified behavior if this GeoKey is absent.

- Files hosted on the CDN will be tiled, presumably with 256x256 tiles (small
  grids that are smaller than 256x256 will use a single strip). On the reading
  side, PROJ will accept TIFF files with any strip or tile organization.
  Tiling is expressed by specifying the TileWidth, TileHeight, TileOffsets
  and TileByteCounts tags. Strip organization is expressed by specifying the
  RowsPerStrip, StripByteCounts and StripOffsets tags.

- Files hosted on the CDN will use `Compression
  <https://www.awaresystems.be/imaging/tiff/tifftags/compression.html>`_ = DEFLATE
  or LZW (to be determined, possibly with
  `Predictor <https://www.awaresystems.be/imaging/tiff/tifftags/predictor.html>`_ = 2
  or 3)
  On the reading side, PROJ will accept TIFF files with any compression method
  (appropriate for the data types and PhotometricInterpretation considered)
  supported by the libtiff build used by PROJ. Of course uncompressed files will be supported.

- Files hosted on the CDN will use little-endian byte ordering. On the reading
  side, libtiff will transparently handle both little-endian and big-endian
  ordering.

- Files hosted on the CDN will use PlanarConfiguration=Separate.
  The tools described in a later section will order blocks so that blocks needed
  for a given location are close to each other.
  On the reading side, PROJ will handle also PlanarConfiguration=Contig.

- Files hosted on the CDN will generally use Float32 (BitsPerSample=32 and SampleFormat=IEEEFP)
  Files may be created using Signed Int 16 (
  `BitsPerSample <https://www.awaresystems.be/imaging/tiff/tifftags/bitspersample.html>`_ =16 and
  `SampleFormat <https://www.awaresystems.be/imaging/tiff/tifftags/sampleformat.html>`_ = INT),
  Unsigned Int 16 (BitsPerSample=16 and SampleFormat=UINT), Signed Int 32 or Unsigned Int 32 generally with an
  associate scale/offset.
  On the reading side, only those three data types will be supported as well.

- Files hosted on the CDN will have a `PhotometricInterpretation
  <https://www.awaresystems.be/imaging/tiff/tifftags/photometricinterpretation.html>`_ = MinIsBlack.
  It will be assumed, and ignored on the reading side.

- Files hosted on the CDN will nominally have:

    * `SamplesPerPixel <https://www.awaresystems.be/imaging/tiff/tifftags/samplesperpixel.html>`_ = 2
      for horizontal shift grid, with the first sample being the longitude offset
      and the second sample being the latitude offset.

    * SamplesPerPixel = 1 for vertical shift grids.

    * SamplesPerPixel = 3 for deformation models combining
      horizontal and vertical adjustments.

   And even for the current identified needs of horizontal or vertical shifts,
   more samples may be present (to indicate for example uncertainties), but
   will be ignored by PROJ.

   The `ExtraSamples <https://www.awaresystems.be/imaging/tiff/tifftags/extrasamples.html>`_
   tag should be set to a value of SamplesPerPixel - 1 (given the rules that
   apply for PhotometricInterpretation = MinIsBlack)

- The `ImageDescription <https://www.awaresystems.be/imaging/tiff/tifftags/imagedescription.html>`_
  tag may be used to convey extra information about the name, provenance, version
  and last updated date of the grid.
  Will be set when possible for files hosted on the CDN.
  Ignored by PROJ.

- The `Copyright <https://www.awaresystems.be/imaging/tiff/tifftags/copyright.html>`_
  tag may be used to convey extra information about the copyright and license of the grid.
  Will be set when possible for files hosted on the CDN.
  Ignored by PROJ.

- The `DateTime <https://www.awaresystems.be/imaging/tiff/tifftags/datetime.html>`_
  tag may be used to convey the date at which the file has been created or
  converted. In case of a file conversion, for example from NTv2, this will be
  the date at which the conversion has been performed. The ``ImageDescription``
  tag however will contain the latest of the CREATED or UPDATED fields from the NTv2 file.
  Will be set when possible for files hosted on the CDN.
  Ignored by PROJ.

- Files hosted on the CDN will use the `GDAL_NODATA
  <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_nodata.html>`_ tag to encode
  the value of the nodata / missing value, when it applies to the grid.

  If offset and/or scaling is used, the nodata value corresponds to the raw value,
  before applying offset and scaling.
  The value found in this tag, if present, will be honoured (to the extent to
  which current PROJ code makes use of nodata).
  For floating point data, writers are strongly discouraged to use non-finite values
  (+/- infinity, NaN) of nodata to maximimize interoperability.
  The GDAL_NODATA value applies to all samples of a given TIFF IFD.

- Files hosted on the CDN will use the `GDAL_METADATA
  <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_metadata.html>`_ tag to encode extra
  metadata not supported by baseline or extended TIFF.

  * The root XML node should be ``GDALMetadata``

  * Zero, one or several child XML nodes ``Item`` may be present.

  * A Item should have a ``name`` attribute, and a child text node with its value.
    ``role`` and ``sample`` attributes may be present for attributes that have
    a special semantics (recognized by GDAL). The value of `sample` should be
    a integer value between 0 and number_of_samples - 1.

  * Scale and offset to convert integer raw values to floating point values
    may be expressed with XML `Item` elements whose name attribute is respectively
    ``SCALE`` and ``OFFSET``, and their ``role`` attribute is respectively ``scale``
    and ``offset``. The decoded value will be: {offset} + {scale} * raw_value_from_geotiff_file

    For a offset value of 1 and scaling of 2, the following payload should be
    stored:

    .. code-block:: xml

        <GDALMetadata>
            <Item name="OFFSET" sample="0" role="offset">1</Item>
            <Item name="SCALE" sample="0" role="scale">2</Item>
        </GDALMetadata>

  * The type of the grid must be specified with a `Item` whose ``name`` is set
    to ``TYPE``.

    Values recognized by PROJ currently are:

    - ``HORIZONTAL_OFFSET``: implies the presence of at least two samples.
      The first sample must contain the latitude offset and the second
      sample must contain the longitude offset.
      Corresponds to PROJ :ref:`hgridshift` method.

    - ``VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL``: implies the presence of at least one sample.
      The first sample must contain the vertical adjustment. Must be used when
      the source/interpolation CRS is a Geographic CRS and the target CRS a Vertical CRS.
      Corresponds to PROJ  :ref:`vgridshift` method.

    - ``VERTICAL_OFFSET_VERTICAL_TO_VERTICAL``: implies the presence of at least one sample.
      The first sample must contain the vertical adjustment. Must be used when
      the source and target CRS are Vertical CRS.
      Corresponds to PROJ :ref:`vgridshift` method.

    - ``GEOCENTRIC_TRANSLATION``: implies the presence of at least 3 samples.
      The first 3 samples must be respectively the geocentric adjustments along
      the X, Y and Z axis. Must be used when the source and target CRS are
      geocentric CRS. The interpolation CRS must be a geographic CRS.
      Corresponds to PROJ :ref:`xyzgridshift` method.

    - ``VELOCITY``: implies the presence of at least 3 samples.
      The first 3 samples must be respectively the velocities along
      the E(ast), N(orth), U(p) axis in the local topocentric coordinate system.
      Corresponds to PROJ :ref:`deformation` method.

    - ``DEFORMATION_MODEL``: implies the presence of the ``DISPLACEMENT_TYPE``
      and ``UNCERTAINTY_TYPE`` metadata items.
      Corresponds to PROJ :ref:`defmodel` method.

    For example:

    .. code-block:: xml

        <Item name="TYPE">HORIZONTAL_OFFSET</Item>

  * The description of each sample must be specified with a Item whose ``name``
    attribute is set to ``DESCRIPTION`` and ``role`` attribute to ``description``.

    Values recognized by PROJ for this Item are currently:

    + ``latitude_offset``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the value to add a latitude expressed in the CRS encoded in the GeoKeys
      to obtain a latitude value expressed in the target CRS.

    + ``longitude_offset``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the value to add a longitude expressed in the CRS encoded in the GeoKeys
      to obtain a longitude value expressed in the target CRS.

    + ``geoid_undulation``: valid for TYPE=VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL.
      For a source CRS being a geographic CRS and a target CRS being a vertical CRS,
      sample values should be the value to add to a geoid-related height (that
      is expressed in the target CRS) to
      get an ellipsoidal height (that is expressed in the source CRS), also
      called the geoid undulation.
      Note the possible confusion related to what is the source CRS and target CRS and
      the semantics of the value stored (to convert from the source to the target,
      one must subtract the value contained in the grid). This is the convention
      used by the `EPSG:9665 <https://www.epsg-registry.org/export.htm?gml=urn:ogc:def:method:EPSG::9665>`_
      operation method.

    + ``vertical_offset``: valid for TYPE=VERTICAL_OFFSET_VERTICAL_TO_VERTICAL.
      For a source and target CRS being vertical CRS,
      sample values should be the value to add to an elevation expressed in the
      source CRS to obtain a longitude value expressed in the target CRS.

    + ``x_translation`` / ``y_translation`` / ``z_translation``: valid for
      TYPE=GEOCENTRIC_TRANSLATION.
      Sample values should be the value to add to the input geocentric coordinates
      expressed in the source CRS to geocentric coordinates expressed in the target CRS.

    + ``east_velocity`` / ``north_velocity`` / ``up_velocity``: valid for
      TYPE=VELOCITY.
      Sample values should be the velocity in a linear/time unit in a ENU local
      topocentric coordinate system.

    + ``east_offset`` / ``north_offset`` / ``vertical_offset``: valid for
      TYPE=DEFORMATION_MODEL.
      For east_offset and north_offset, the unit might be degree or metre.
      For vertical_offset, the unit must be metre.

    For example:

    .. code-block:: xml

        <Item name="DESCRIPTION" sample="0" role="description">latitude_offset</Item>
        <Item name="DESCRIPTION" sample="1" role="description">longitude_offset</Item>

    Other values may be used (not used by PROJ):

    + ``latitude_offset_accuracy``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the accuracy of corresponding latitude_offset samples. Generally in metre (if converted from NTv2)

    + ``longitude_offset_accuracy``: valid for TYPE=HORIZONTAL_OFFSET. Sample values should be
      the accuracy of corresponding longitude_offset samples. Generally in metre (if converted from NTv2)

  * The sign convention for the values of the ``longitude_offset`` channel
    should be indicated with an Item named ``positive_value`` whose value
    can be ``west`` or ``east``. NTv2 products originally use a ``west``
    convention, but when converting from them to GeoTIFF, the sign of those
    samples will be inverted so they use a more natural ``east`` convention.
    If this item is absent, the default value is ``east``.

  * The unit of the values stored in the grid must be specified for each
    sample through an Item of name ``UNITTYPE`` and role ``unittype``
    Valid values should be the name of entries from the EPSG ``unitofmeasure``
    table. To maximize interoperability, writers are strongly encouraged to
    limit themselves to the following values:

    For linear units:

    - ``metre`` (default value assumed if absent for vertical shift grid files, and value used for files stored on PROJ CDN)
    - ``US survey foot``

    For angular units:

    - ``degree``
    - ``arc-second`` (default value assumed if absent for longitude and latitude offset samples of horizontal shift grid files, and value used for files stored on PROJ CDN)

    For velocity units:

    - ``millimetres per year``

    The longitude and latitude offset samples should use the same unit.
    The geocentric translation samples should use the same unit.
    The velocity samples should use the same unit.

    Example:

    .. code-block:: xml

        <Item name="UNITTYPE" sample="0" role="unittype">arc-second</Item>
        <Item name="UNITTYPE" sample="1" role="unittype">arc-second</Item>

  * For TYPE=DEFORMATION_MODEL, the type of the displacement must be specified
    with a `Item` whose ``name`` is set to ``DISPLACEMENT_TYPE``.

    The accepted values are: ``HORIZONTAL``, ``VERTICAL``, ``3D`` or ``NONE``

  * For TYPE=DEFORMATION_MODEL, the type of the uncertainty must be specified
    with a `Item` whose ``name`` is set to ``UNCERTAINTY_TYPE``.

    The accepted values are: ``HORIZONTAL``, ``VERTICAL``, ``3D`` or ``NONE``

  * The ``target_crs_epsg_code`` metadata item should be present.
    For a horizontal shift grid, this is the EPSG
    code of the target geographic CRS. For a vertical shift grid, this is the
    EPSG code of a the target vertical CRS.
    If the target CRS has no associated EPSG code, ``target_crs_wkt`` must be
    used.
    Ignored by PROJ currently.

  * The ``target_crs_wkt`` metadata item must be present if the
    ``target_crs_epsg_code`` cannot be used.
    Its value should be a valid WKT string according to
    `WKT:2015 <http://docs.opengeospatial.org/is/12-063r5/12-063r5.html>`_
    or `WKT:2019 <hhttp://docs.opengeospatial.org/is/18-010r7/18-010r7.html>`_
    Ignored by PROJ currently.

  * The ``source_crs_epsg_code`` metadata item must be present if the source
    and interpolation CRS are not the same (typical use case is vertical CRS to vertical CRS
    transformation), because the GeoKeys encode the interpolation CRS and not the source CRS.
    If the source CRS has no associated EPSG code, ``source_crs_wkt`` must be
    used.
    Ignored by PROJ currently.

  * The ``source_crs_wkt`` metadata item must be present if the
    ``source_crs_epsg_code`` cannot be used.
    Its value should be a valid WKT string according to WKT:2015 or WKT:2019.
    Ignored by PROJ currently.

  * The ``interpolation_crs_wkt`` metadata item may be present if the GeoKeys
    cannot be used to express reliably the interpolation CRS.
    Its value should be a valid WKT string according to WKT:2015 or WKT:2019.
    Ignored by PROJ currently.

  * The ``recommended_interpolation_method`` metadata item may be present to
    describe the method to use to interpolation values at locations not
    coincident with nodes stored in the grid file. Potential values: ``bilinear``,
    ``bicubic``.
    Ignored by PROJ currently.

  * The ``area_of_use`` metadata item can be used to indicate plain text information
    about the area of use of the grid (like "USA - Wisconsin"). In case of multiple
    subgrids, it should be set only on the first one, but applies to the whole
    set of grids, not just the first one.

  * The ``grid_name`` metadata item should be present if there are
    subgrids for this grid (that is grids whose extent is contained in the extent
    of this grid), or if this is a subgrid.
    It is intended to be a relatively short identifier
    Will be ignored by PROJ (this information can be inferred by the grids extent)

  * The ``parent_grid_name`` metadata item should be present if this is a
    subgrid and its value should be equal to the parent's ``grid_name``
    Will be ignored by PROJ (this information can be inferred by the grids extent)

  * The ``number_of_nested_grids`` metadata item should be present if there are
    subgrids for this grid (that is grids whose extent is contained in the extent
    of this grid).
    Will be ignored by PROJ (this information can be inferred by the grids extent)

Example
+++++++

https://github.com/OSGeo/PROJ-data/blob/master/fr_ign/fr_ign_ntf_r93.tif has
been converted from https://github.com/OSGeo/proj-datumgrid/blob/master/ntf_r93.gsb
with https://github.com/OSGeo/PROJ-data/blob/master/grid_tools/ntv2_to_gtiff.py

::

    $ tiffinfo ntf_r93.tif 

    TIFF Directory at offset 0x4e (78)
    Image Width: 156 Image Length: 111
    Bits/Sample: 32
    Sample Format: IEEE floating point
    Compression Scheme: AdobeDeflate
    Photometric Interpretation: min-is-black
    Extra Samples: 3<unspecified, unspecified, unspecified>
    Samples/Pixel: 4
    Rows/Strip: 111
    Planar Configuration: separate image planes
    ImageDescription: NTF (EPSG:4275) to RGF93 (EPSG:4171). Converted from ntf_r93.gsb (version IGN07_01, last updated on 2007-10-31)
    DateTime: 2019:12:09 00:00:00
    Copyright: Derived from work by IGN France. Open License https://www.etalab.gouv.fr/wp-content/uploads/2014/05/Open_Licence.pdf
    Tag 33550: 0.100000,0.100000,0.000000
    Tag 33922: 0.000000,0.000000,0.000000,-5.500000,52.000000,0.000000
    Tag 34735: 1,1,1,3,1024,0,1,2,1025,0,1,2,2048,0,1,4275
    Tag 42112: <GDALMetadata>
    <Item name="grid_name">FRANCE</Item>
    <Item name="target_crs_epsg_code">4171</Item>
    <Item name="TYPE">HORIZONTAL_OFFSET</Item>
    <Item name="UNITTYPE" sample="0" role="unittype">arc-second</Item>
    <Item name="DESCRIPTION" sample="0" role="description">latitude_offset</Item>
    <Item name="positive_value" sample="1">east</Item>
    <Item name="UNITTYPE" sample="1" role="unittype">arc-second</Item>
    <Item name="DESCRIPTION" sample="1" role="description">longitude_offset</Item>
    <Item name="UNITTYPE" sample="2" role="unittype">arc-second</Item>
    <Item name="DESCRIPTION" sample="2" role="description">latitude_offset_accuracy</Item>
    <Item name="UNITTYPE" sample="3" role="unittype">arc-second</Item>
    <Item name="DESCRIPTION" sample="3" role="description">longitude_offset_accuracy</Item>
    </GDALMetadata>

    Predictor: floating point predictor 3 (0x3)


::

    $ listgeo ntf_r93.tif 

    Geotiff_Information:
        Version: 1
        Key_Revision: 1.1
        Tagged_Information:
            ModelTiepointTag (2,3):
                0                 0                 0                
                -5.5              52                0                
            ModelPixelScaleTag (1,3):
                0.1               0.1               0                
            End_Of_Tags.
        Keyed_Information:
            GTModelTypeGeoKey (Short,1): ModelTypeGeographic
            GTRasterTypeGeoKey (Short,1): RasterPixelIsPoint
            GeodeticCRSGeoKey (Short,1): Code-4275 (NTF)
            End_Of_Keys.
        End_Of_Geotiff.

    GCS: 4275/NTF
    Datum: 6275/Nouvelle Triangulation Francaise
    Ellipsoid: 7011/Clarke 1880 (IGN) (6378249.20,6356515.00)
    Prime Meridian: 8901/Greenwich (0.000000/  0d 0' 0.00"E)
    Projection Linear Units: User-Defined (1.000000m)

    Corner Coordinates:
    Upper Left    (  5d30' 0.00"W, 52d 0' 0.00"N)
    Lower Left    (  5d30' 0.00"W, 40d54' 0.00"N)
    Upper Right   ( 10d 6' 0.00"E, 52d 0' 0.00"N)
    Lower Right   ( 10d 6' 0.00"E, 40d54' 0.00"N)
    Center        (  2d18' 0.00"E, 46d27' 0.00"N)

::

    $ gdalinfo ntf_r93.tif

    Driver: GTiff/GeoTIFF
    Files: ntf_r93.tif
    Size is 156, 111
    Coordinate System is:
    GEOGCRS["NTF",
        DATUM["Nouvelle Triangulation Francaise",
            ELLIPSOID["Clarke 1880 (IGN)",6378249.2,293.466021293627,
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
        ID["EPSG",4275]]
    Data axis to CRS axis mapping: 2,1
    Origin = (-5.550000000000000,52.049999999999997)
    Pixel Size = (0.100000000000000,-0.100000000000000)
    Metadata:
      AREA_OR_POINT=Point
      grid_name=FRANCE
      target_crs_epsg_code=4171
      TIFFTAG_DATETIME=2019:12:09 00:00:00
      TIFFTAG_IMAGEDESCRIPTION=NTF (EPSG:4275) to RGF93 (EPSG:4171). Converted from ntf_r93.gsb (version IGN07_01, last updated on 2007-10-31)
      TYPE=HORIZONTAL_OFFSET
    Image Structure Metadata:
      COMPRESSION=DEFLATE
      INTERLEAVE=BAND
    Corner Coordinates:
    Upper Left  (  -5.5500000,  52.0500000) (  5d33' 0.00"W, 52d 3' 0.00"N)
    Lower Left  (  -5.5500000,  40.9500000) (  5d33' 0.00"W, 40d57' 0.00"N)
    Upper Right (  10.0500000,  52.0500000) ( 10d 3' 0.00"E, 52d 3' 0.00"N)
    Lower Right (  10.0500000,  40.9500000) ( 10d 3' 0.00"E, 40d57' 0.00"N)
    Center      (   2.2500000,  46.5000000) (  2d15' 0.00"E, 46d30' 0.00"N)
    Band 1 Block=156x111 Type=Float32, ColorInterp=Gray
      Description = latitude_offset
      Unit Type: arc-second
    Band 2 Block=156x111 Type=Float32, ColorInterp=Undefined
      Description = longitude_offset
      Unit Type: arc-second
      Metadata:
        positive_value=east
    Band 3 Block=156x111 Type=Float32, ColorInterp=Undefined
      Description = latitude_offset_accuracy
      Unit Type: arc-second
    Band 4 Block=156x111 Type=Float32, ColorInterp=Undefined
      Description = longitude_offset_accuracy
      Unit Type: arc-second

Multi-grid storage
++++++++++++++++++

Formats like NTv2 can contain multiple subgrids. This can be transposed to
TIFF by using several IFD chained together with the last 4 bytes (or 8 bytes
for BigTIFF) of an IFD pointing to the offset of the next one.

The first IFD should have a full description according to the
:ref:`General description <gtg_general_description>`.
Subsequent IFD might have a more compact description, omitting for example, CRS
information if it is identical to the main IFD (which should be the case for
the currently envisionned use cases), or Copyright / ImageDescription metadata
items.

Each IFD will have its
`NewSubfileType <https://www.awaresystems.be/imaging/tiff/tifftags/newsubfiletype.html>`_
tag set to 0.

If a low-resolution grid is available, it should be put before subgrids of
higher-resolution in the chain of IFD linking. On reading, PROJ will use the
value from the highest-resolution grid that contains the point of interest.

For efficient reading from the network, files hosted on the CDN will use
a layout similar to the one described in the `low level paragraph of the Cloud Optimized GeoTIFF
GDAL driver page <https://gdal.org/drivers/raster/cog.html#low-level>`_

The layout for a file converted from NTv2 will for example be:

- TIFF/BigTIFF header/signature and pointer to first IFD (Image File Directory)
- "ghost area" indicating the generated process
- IFD of the first grid, followed by TIFF tags values, excluding the TileOffsets and TileByteCounts arrays
- ...
- IFD of the last grid, followed by TIFF tags values, excluding the GDAL_METADATA tag, TileOffsets and TileByteCounts arrays
- TileOffsets and TileByteCounts arrays for first IFD
- ...
- TileOffsets and TileByteCounts arrays for last IFD
- Value of GDAL_METADATA tag for IFDs following the first IFD
- First IFD: Data corresponding to latitude offset of Block_0_0
- First IFD: Data corresponding to longitude offset of Block_0_0
- First IFD: Data corresponding to latitude offset of Block_0_1
- First IFD: Data corresponding to longitude offset of Block_0_1
- ...
- First IFD: Data corresponding to latitude offset of Block_n_m
- First IFD: Data corresponding to longitude offset of Block_n_m
- ...
- Last IFD: Data corresponding to latitude offset of Block_0_0
- Last IFD: Data corresponding to longitude offset of Block_0_0
- Last IFD: Data corresponding to latitude offset of Block_0_1
- Last IFD: Data corresponding to longitude offset of Block_0_1
- ...
- Last IFD: Data corresponding to latitude offset of Block_n_m
- Last IFD: Data corresponding to longitude offset of Block_n_m

If longitude_offset_accuracy and latitude_offset_accuracy are present, this
will be followed by:

- First IFD: Data corresponding to latitude offset accuracy of Block_0_0
- First IFD: Data corresponding to longitude offset accuracy of Block_0_0
- ...
- First IFD: Data corresponding to latitude offset accuracy of Block_n_m
- First IFD: Data corresponding to longitude offset accuracy of Block_n_m
- ...
- Last IFD: Data corresponding to latitude offset accuracy of Block_0_0
- Last IFD: Data corresponding to longitude offset accuracy of Block_0_0
- ...
- Last IFD: Data corresponding to latitude offset accuracy of Block_n_m
- Last IFD: Data corresponding to longitude offset accuracy of Block_n_m

.. note::

    TIFF has another mechanism to link IFDs, the SubIFD tag. This potentially
    enables to define a hierarchy of IFDs (similar to HDF5 groups). There is no
    support for that in most TIFF-using software, notably GDAL, and no compelling
    need to have a nested hierarchy, so "flat" organization with the standard IFD chaining
    mechanism is adopted.

Examples of multi-grid dataset
++++++++++++++++++++++++++++++

https://github.com/OSGeo/PROJ-data/blob/master/au_icsm/au_icsm_GDA94_GDA2020_conformal.tif has
been converted from https://github.com/OSGeo/proj-datumgrid/blob/master/oceania/GDA94_GDA2020_conformal.gsb
with https://github.com/OSGeo/PROJ-data/blob/master/grid_tools/ntv2_to_gtiff.py

It contains 5 subgrids. All essential metadata to list the subgrids and their
georeferencing is contained within the first 3 KB of the file.

The file size is 4.8 MB using DEFLATE compression and floating-point predictor.
To be compared with the 83 MB of the original .gsb file.

https://github.com/OSGeo/PROJ-data/blob/master/ca_nrc/ca_nrc_ntv2_0.tif has
been converted from https://github.com/OSGeo/proj-datumgrid/blob/master/north-america/ntv2_0.gsb

It contains 114 subgrids. All essential metadata to list the subgrids and their
georeferencing is contained within the first 40 KB of the file.
