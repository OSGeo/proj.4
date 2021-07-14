.. _rfc4:

====================================================================
PROJ RFC 4: Remote access to grids and GeoTIFF grids
====================================================================

:Author: Even Rouault, Howard Butler
:Contact: even.rouault@spatialys.com, howard@hobu.co
:Status: Adopted
:Implementation target: PROJ 7
:Last Updated: 2020-01-10

Motivation
-------------------------------------------------------------------------------

PROJ 6 brings undeniable advances in the management of coordinate
transformations between datums by relying and applying information available in
the PROJ database. PROJ's rapid evolution from a cartographic projections
library with a little bit of geodetic capability to a full geodetic
transformation and description environment has highlighted the importance of
the support data. Users desire the convenience of software doing the right
thing with the least amount of fuss, and survey organizations wish to deliver
their models across as wide a software footprint as possible.  To get results
with the highest precision, a grid file that defines a model that provides
dimension shifts is often needed. The proj-datumgrid project centralizes grids
available under an open data license and bundles them in different archives
split along major geographical regions of the world .

It is assumed that a PROJ user has downloaded and installed grid files that are
referred to in the PROJ database. These files can be quite large in aggregate,
and packaging support by major distribution channels is somewhat uneven due to
their size, sometimes ambiguous licensing story, and difficult-to-track
versioning and lineage.  It is not always clear to the user, especially to
those who may not be so familiar with geodetic operations, that the highest
precision transformation may not always being applied if grid data is not
available. Users want both convenience and correctness, and management of the
shift files can be challenging to those who may not be aware of their
importance to the process.

The computing environment in which PROJ operates is also changing. Because the
shift data can be so large (currently more than 700 MB of uncompressed data,
and growing), deployment of high accuracy operations can be limited due to
deployment size constraints (serverless operations, for example). Changing to a
delivery format that supports incremental access over a network along with
convenient access and compression will ease the resource burden the shift files
present while allowing the project to deliver transformation capability with
the highest known precision provided by the survey organizations.

Adjustment grids also tend to be provided in many different formats depending
on the organization and country that produced them. In PROJ, we have over time
"standardized" on using horizontal shift grids as NTv2 and vertical shift grids
using GTX. Both have poor general support as dedicated formats, limited
metadata capabilities, and neither are not necessarily "cloud optimized" for
incremental access across a network.

Summary of work planned by this RFC
-------------------------------------------------------------------------------

- Grids will be hosted by one or several Content Delivery Networks (CDN)
- Grid loading mechanism will be reworked to be able to download grids or parts
  of grids from a online repository. When opted in, users will no longer have to
  manually fetch grid files and place them in PROJ_LIB.
  Full and accurate capability of the software will no longer require hundreds
  of megabytes of grid shift files in advance, even if only just a few of them
  are needed for the transformations done by the user.
- Local caching of grid files, or even part of files, so that users end up
  mirroring what they actually use.
- A grid shift format, for both horizontal and vertical shift grids (and in
  potential future steps, for other needs, such as deformation models) will be
  implemented.

The use of grids locally available will of course still be available, and will
be the default behavior. 

Network access to grids
-------------------------------------------------------------------------------

curl will be an optional build dependency of PROJ, added in autoconf and cmake
build systems. It can be disabled at build time, but this must be
an explicit setting of configure/cmake as the resulting builds have less functionality.
When curl is enabled at build time, download of grids themselves will not be
enabled by default at runtime. It will require explicit consent of the user, either
through the API
(:c:func:`proj_context_set_enable_network`) through the PROJ_NETWORK=ON
environment variable, or the ``network = on`` setting of proj.ini.

Regarding the minimum version of libcurl required, given GDAL experience that
can build with rather ancient libcurl for similar functionality, we can aim for
libcurl >= 7.29.0 (as being available in RHEL 7).

An alternate pluggable network interface can also be set by the user in case
support for libcurl was not built in, or if for the desired context of use, the
user wishes to provide the network implementation (a typical use case could be
QGIS that would use its QT-based networking facilities to solve issues with
SSL, proxy, authentication, etc.)

A text configuration file, installed in ${installation_prefix}/share/proj/proj.ini
(or ${PROJ_LIB}/proj.ini)
will contain the URL of the CDN that will be used.
The user may also override this setting with the
:c:func:`proj_context_set_url_endpoint` or through the PROJ_NETWORK_ENDPOINT
environment variable.

The rationale for putting proj.ini in that location is
that it is a well-known place by PROJ users, with the existing PROJ_LIB mechanics
for systems like Windows where hardcoded paths at runtime aren't generally usable.

C API
+++++

The preliminary C API for the above is:

.. code-block:: c

    /** Enable or disable network access.
    *
    * @param ctx PROJ context, or NULL
    * @return TRUE if network access is possible. That is either libcurl is
    *         available, or an alternate interface has been set.
    */
    int proj_context_set_enable_network(PJ_CONTEXT* ctx, int enable);

    /** Define URL endpoint to query for remote grids.
    *
    * This overrides the default endpoint in the PROJ configuration file or with
    * the PROJ_NETWORK_ENDPOINT environment variable.
    *
    * @param ctx PROJ context, or NULL
    * @param url Endpoint URL. Must NOT be NULL.
    */
    void proj_context_set_url_endpoint(PJ_CONTEXT* ctx, const char* url);

    /** Opaque structure for PROJ. Implementations might cast it to their
     * structure/class of choice. */
    typedef struct PROJ_NETWORK_HANDLE PROJ_NETWORK_HANDLE;

    /** Network access: open callback
    * 
    * Should try to read the size_to_read first bytes at the specified offset of
    * the file given by URL url,
    * and write them to buffer. *out_size_read should be updated with the actual
    * amount of bytes read (== size_to_read if the file is larger than size_to_read).
    * During this read, the implementation should make sure to store the HTTP
    * headers from the server response to be able to respond to
    * proj_network_get_header_value_cbk_type callback.
    *
    * error_string_max_size should be the maximum size that can be written into
    * the out_error_string buffer (including terminating nul character).
    *
    * @return a non-NULL opaque handle in case of success.
    */
    typedef PROJ_NETWORK_HANDLE* (*proj_network_open_cbk_type)(
                                                        PJ_CONTEXT* ctx,
                                                        const char* url,
                                                        unsigned long long offset,
                                                        size_t size_to_read,
                                                        void* buffer,
                                                        size_t* out_size_read,
                                                        size_t error_string_max_size,
                                                        char* out_error_string,
                                                        void* user_data);

    /** Network access: close callback */
    typedef void (*proj_network_close_cbk_type)(PJ_CONTEXT* ctx,
                                                PROJ_NETWORK_HANDLE* handle,
                                                void* user_data);

    /** Network access: get HTTP headers */
    typedef const char* (*proj_network_get_header_value_cbk_type)(
                                                PJ_CONTEXT* ctx,
                                                PROJ_NETWORK_HANDLE* handle,
                                                const char* header_name,
                                                void* user_data);

    /** Network access: read range
    *
    * Read size_to_read bytes from handle, starting at offset, into
    * buffer.
    * During this read, the implementation should make sure to store the HTTP
    * headers from the server response to be able to respond to
    * proj_network_get_header_value_cbk_type callback.
    *
    * error_string_max_size should be the maximum size that can be written into
    * the out_error_string buffer (including terminating nul character).
    *
    * @return the number of bytes actually read (0 in case of error)
    */
    typedef size_t (*proj_network_read_range_type)(
                                                PJ_CONTEXT* ctx,
                                                PROJ_NETWORK_HANDLE* handle,
                                                unsigned long long offset,
                                                size_t size_to_read,
                                                void* buffer,
                                                size_t error_string_max_size,
                                                char* out_error_string,
                                                void* user_data);

    /** Define a custom set of callbacks for network access.
    *
    * All callbacks should be provided (non NULL pointers).
    *
    * @param ctx PROJ context, or NULL
    * @param open_cbk Callback to open a remote file given its URL
    * @param close_cbk Callback to close a remote file.
    * @param get_header_value_cbk Callback to get HTTP headers
    * @param read_range_cbk Callback to read a range of bytes inside a remote file.
    * @param user_data Arbitrary pointer provided by the user, and passed to the
    * above callbacks. May be NULL.
    * @return TRUE in case of success.
    */
    int proj_context_set_network_callbacks(
        PJ_CONTEXT* ctx,
        proj_network_open_cbk_type open_cbk,
        proj_network_close_cbk_type close_cbk,
        proj_network_get_header_value_cbk_type get_header_value_cbk,
        proj_network_read_range_type read_range_cbk,
        void* user_data);


To make network access efficient, PROJ will internally have a in-memory cache
of file ranges to only issue network requests by chunks of 16 KB or multiple of them,
to limit the number of HTTP GET requests and minimize latency caused by network
access. This is very similar to the behavior of the GDAL
`/vsicurl/ <https://gdal.org/user/virtual_file_systems.html#vsicurl-http-https-ftp-files-random-access>`_
I/O layer. The plan is to mostly copy GDAL's vsicurl implementation inside PROJ, with
needed adjustments and proper namespacing of it.

A retry strategy (typically a delay with an exponential back-off and some random
jitter) will be added to account for intermittent network or server-side failure.

URL building
++++++++++++

The PROJ database has a ``grid_transformation`` grid whose column ``grid_name``
(and possibly ``grid2_name``) contain the name of the grid as indicated by the
authority having registered the transformation (typically EPSG). As those
grid names are not generally directly usable by PROJ, the PROJ database has
also a ``grid_alternatives`` table that link original grid names to the ones used
by PROJ. When network access will be available and needed due to lack of a
local grid, the full URL will be the 
endpoint from the configuration or set by the user, the basename of the PROJ
usable filename, and the "tif" suffix. So if the CDN is at http://example.com
and the name from ``grid_alternatives`` is egm96_15.gtx, then the URL will
be http://example.com/egm96_15.tif

Grid loading
++++++++++++

The following files will be affected, in one way or another, by the above describes
changes:
nad_cvt.cpp, nad_intr.cpp, nad_init.cpp, grid_info.cpp, grid_list.cpp, apply_gridshift.cpp,
apply_vgridshift.cpp.

In particular the current logic that consists to ingest all the values of a
grid/subgrid in the ct->cvs array will be completely modified, to enable
access to grid values at a specified (x,y) location.

proj_create_crs_to_crs() / proj_create_operations() impacts
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Once network access is available, all grids known to the PROJ database
(grid_transformation + grid_alternatives table) will be assumed to be available,
when computing the potential pipelines between two CRS.

Concretely, this will be equivalent to calling
:cpp:func:`proj_operation_factory_context_set_grid_availability_use`
with the ``use`` argument set to a new enumeration value 

.. code-block:: c

    /** Results will be presented as if grids known to PROJ (that is
    * registered in the grid_alternatives table of its database) were
    * available. Used typically when networking is enabled.
    */
    PROJ_GRID_AVAILABILITY_KNOWN_AVAILABLE


Local on-disk caching of remote grids
+++++++++++++++++++++++++++++++++++++

As many workflows will tend to use the same grids over and over, a local
on-disk caching of remote grids will be added. The cache will be a single
SQLite3 database, in a user-writable directory shared by all applications using
PROJ.

Its total size will be configurable, with a default maximum size of 100 MB
in proj.ini. The cache will also keep the timestamp of the last time it checked
various global properties of the file (its size, Last-Modified and ETag headers).
A time-to-live parameter, with a default of 1 day in proj.ini, will be used to
determine whether the CDN should be hit to verify if the information in the
cache is still up-to-date.

.. code-block:: c

    /** Enable or disable the local cache of grid chunks
    *
    * This overrides the setting in the PROJ configuration file.
    *
    * @param ctx PROJ context, or NULL
    * @param enabled TRUE if the cache is enabled.
    */
    void proj_grid_cache_set_enable(PJ_CONTEXT *ctx, int enabled);

    /** Override, for the considered context, the path and file of the local
    * cache of grid chunks.
    *
    * @param ctx PROJ context, or NULL
    * @param fullname Full name to the cache (encoded in UTF-8). If set to NULL,
    *                 caching will be disabled.
    */
    void proj_grid_cache_set_filename(PJ_CONTEXT* ctx, const char* fullname);

    /** Override, for the considered context, the maximum size of the local
    * cache of grid chunks.
    *
    * @param ctx PROJ context, or NULL
    * @param max_size_MB Maximum size, in mega-bytes (1024*1024 bytes), or
    *                    negative value to set unlimited size.
    */
    void proj_grid_cache_set_max_size(PJ_CONTEXT* ctx, int max_size_MB);

    /** Override, for the considered context, the time-to-live delay for
    * re-checking if the cached properties of files are still up-to-date.
    *
    * @param ctx PROJ context, or NULL
    * @param ttl_seconds Delay in seconds. Use negative value for no expiration.
    */
    void proj_grid_cache_set_ttl(PJ_CONTEXT* ctx, int ttl_seconds);

    /** Clear the local cache of grid chunks.
     *
     * @param ctx PROJ context, or NULL.
     */
    void proj_grid_cache_clear(PJ_CONTEXT* ctx);

The planned database structure is:

.. code-block:: sql

    -- General properties on a file
    CREATE TABLE properties(
     url          TEXT PRIMARY KEY NOT NULL,
     lastChecked  TIMESTAMP NOT NULL,
     fileSize     INTEGER NOT NULL,
     lastModified TEXT,
     etag         TEXT
    );

    -- Store chunks of data. To avoid any potential fragmentation of the
    -- cache, the data BLOB is always set to the maximum chunk size of 16 KB
    -- (right padded with 0-byte)
    -- The actual size is stored in chunks.data_size
    CREATE TABLE chunk_data(
     id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),
     data      BLOB NOT NULL
    );

    -- Record chunks of data by (url, offset)
    CREATE TABLE chunks(
     id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),
     url       TEXT NOT NULL,
     offset    INTEGER NOT NULL,
     data_id   INTEGER NOT NULL,
     data_size INTEGER NOT NULL,
     CONSTRAINT fk_chunks_url FOREIGN KEY (url) REFERENCES properties(url),
     CONSTRAINT fk_chunks_data FOREIGN KEY (data_id) REFERENCES chunk_data(id)
    );
    CREATE INDEX idx_chunks ON chunks(url, offset);

    -- Doubly linked list of chunks. The next link is to go to the least-recently
    -- used entries.
    CREATE TABLE linked_chunks(
     id        INTEGER PRIMARY KEY AUTOINCREMENT CHECK (id > 0),
     chunk_id  INTEGER NOT NULL,
     prev      INTEGER,
     next      INTEGER,
     CONSTRAINT fk_links_chunkid FOREIGN KEY (chunk_id) REFERENCES chunks(id),
     CONSTRAINT fk_links_prev FOREIGN KEY (prev) REFERENCES linked_chunks(id),
     CONSTRAINT fk_links_next FOREIGN KEY (next) REFERENCES linked_chunks(id)
    );
    CREATE INDEX idx_linked_chunks_chunk_id ON linked_chunks(chunk_id);

    -- Head and tail pointers of the linked_chunks. The head pointer is for
    -- the most-recently used chunk.
    -- There should be just one row in this table.
    CREATE TABLE linked_chunks_head_tail(
      head       INTEGER,
      tail       INTEGER,
      CONSTRAINT lht_head FOREIGN KEY (head) REFERENCES linked_chunks(id),
      CONSTRAINT lht_tail FOREIGN KEY (tail) REFERENCES linked_chunks(id)
    );
    INSERT INTO linked_chunks_head_tail VALUES (NULL, NULL);

The chunks table will store 16 KB chunks (or less for terminating chunks).
The linked_chunks and linked_chunks_head_tail table swill act as a doubly linked
list of chunks, with the least recently used ones at the end of the list, which
will be evicted when the cache saturates.

The directory used to locate this database will be ${XDG_DATA_HOME}/proj
(per https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
where ${XDG_DATA_HOME} defaults to ${HOME}/.local/share on Unix builds
and ${LOCALAPPDATA} on Windows builds. Exact details to be sorted out, but
https://github.com/ActiveState/appdirs/blob/a54ea98feed0a7593475b94de3a359e9e1fe8fdb/appdirs.py#L45-L97
can be a good reference.

As this database might be accessed by several threads or processes at the same
time, the code accessing to it will carefully honour SQLite3 errors regarding
to locks, to do appropriate retries if another thread/process is currently
locking the database. Accesses requiring a modification of the database will
start with a BEGIN IMMEDIATE transaction so as to acquire a write lock.

.. note:: This database should be hosted on a local disk, not a network one.
          Otherwise SQLite3 locking issues are to be expected.

CDN provider
++++++++++++

`Amazon Public Datasets <https://aws.amazon.com/opendata/public-datasets/>`_
has offered to be a storage and CDN provider.

The program covers storage and egress (bandwidth) of the data.
They generally don't allow usage of CloudFront
(their CDN) as part of the program (we would usually look to have it covered
by credits), but in this instance, they would be fine to provide it.
They'd only ask that we keep the CloudFront URL "visible" (as appropriate for
the use case) so people can see where the data is hosted in case they go looking.
Their terms can be seen at https://aws.amazon.com/service-terms/ and CloudFront
has its own, small section. Those terms may change a bit from time to time for
minor changes. Major changing service terms is assumed to be unfrequent.
There are also the Public Dataset Program terms at http://aws.amazon.com/public-datasets/terms/.
Those also do not effectively change over time and are renewed on a 2 year basis.

Criteria for grid hosting
+++++++++++++++++++++++++

The grids hosted on the CDN will be exactly the ones collected,
currently and in the future, by the `proj-datumgrid <https://github.com/OSGeo/proj-datumgrid/>`_
initiative. In particular, new grids are accepted as long as
they are released under a license that is compatible with the
`Open Source Definition <https://opensource.org/osd-annotated>`_ and the source
of the grid is clearly stated and verifiable. Suitable licenses include:

- Public domain
- X/MIT
- BSD 2/3/4 clause
- CC0
- CC-BY (v3.0 or later)
- CC-BY-SA (v3.0 or later)

For new grids to be transparently used by the proj_create_crs_to_crs() mechanics,
they must be registered in the PROJ database (proj.db) in the ``grid_transformation`` and
``grid_alternatives`` table. The nominal path to have a new record in the grid_transformation
is to have a transformation being registered in the EPSG dataset (if there is no
existing one), which will be subsequently imported into the PROJ database.

Versioning, historical preservation of grids
++++++++++++++++++++++++++++++++++++++++++++

The policy regarding this should be similar to the one applied to
`proj-datumgrid <https://github.com/OSGeo/proj-datumgrid/>`_, which even if
not formalized, is around the following lines:

- Geodetic agencies release regularly new version of grids. Typically for the
  USA, NOAA has released GEOID99, GEOID03, GEOID06, GEOID09, GEOID12A, GEOID12B,
  GEOID18 for the NAVD88 to NAD83/NAD83(2011) vertical adjustments. Each of these
  grids is considered by EPSG and PROJ has a separate object, with distinct filenames.
  The release of a new version does not cause the old grid to be automatically removed.
  That said, due to advertized accuracies and supersession rules of the EPSG dataset, the
  most recent grid will generally be used for a CRS -> CRS transformation if the
  user uses proj_create_crs_to_crs() (with the exception that if a VERT_CRS WKT
  includes a GEOID_MODEL known to PROJ, an old version of the grid will be used).
  If the user specifies a whole pipeline with an explicit grid name, it will be
  of course strictly honoured.
  As time goes, the size of the datasets managed by proj-datumgrid will be increasing,
  we will have to explore on we managed that for the distributed .zip / .tar.gz
  archives. This should not be a concern for CDN hosted content.

- In case software-related conversion errors from the original grid format to the
  one used by PROJ (be it GTX, NTv2 or GeoTIFF) would happen, the previous erroneous
  version of the dataset would be replaced by the corrected one. In that situation,
  this might have an effect with the local on-disk caching of remote grids. We will
  have to see with the CDN providers used if we can use for example the ETag HTTP header
  on the client to detect a change, so that old cached content is not erroneously
  reused (if not possible, we'll have to use some text file listing the grid names and their
  current md5sum)


Grids in GeoTIFF format
-------------------------------------------------------------------------------

Limitations of current formats
++++++++++++++++++++++++++++++

Several formats exist depending on the ad-hoc needs and ideas of the original
data producer. It would be appropriate to converge on a common format able to
address the different use cases.

- Not tiled. Tiling is a nice to have property for cloud-friendly access to
  large files.
- No support for compression
- The NTv2 structures is roughly: header of main grid, data of main grid,
  header of subgrid 1, data of subgrid 1, header of subgrid 2, data of subgrid 2,
  etc.Due to the headers being scattered through the file, it is not possibly
  to retrieve with a single HTTP GET request all header information.
- GTX format has no provision to store metadata besides the minimum georeferencing
  of the grid. NTv2 is a bit richer, but no extensible metadata possible.

Discussion on choice of format
++++++++++++++++++++++++++++++

We have been made recently aware of other initiatives from the industry to come
with a common format to store geodetic adjustment data. Some discussions have
happen recently within the OGC CRS Working group. Past efforts include the
Esri's proposed Geodetic data Grid eXchange Format, GGXF, briefly mentioned at
page 86 of
https://iag.dgfi.tum.de/fileadmin/IAG-docs/Travaux2015/01_Travaux_Template_Comm_1_tvd.pdf
and page 66 of ftp://ftp.iaspei.org/pub/meetings/2010-2019/2015-Prague/IAG-Geodesy.pdf
The current trend of those works would be to use a netCDF / HDF5 container.

So, for the sake of completeness, we list hereafter a few potential candidate
formats and their pros and cons.

TIFF/GeoTIFF
************

Strong points:

* TIFF is a well-known and widespread format.

* The GeoTIFF encoding is a widely industry supported scheme to encode georeferencing.
  It is now a `OGC standard <https://www.opengeospatial.org/standards/geotiff>`_

* There are independent initiatives to share grids as GeoTIFF, like
  `that one <https://www.agisoft.com/downloads/geoids/>`_

* TIFF can contain multiple images (IFD: Image File Directory) chained together.
  This is the mechanism used for multiple-page scanned TIFF files, or in the
  geospatial field to store multi-resolution/pyramid rasters. So it can be
  used with sub-grids as in the NTv2 format.

* Extensive experience with the TIFF format, and its appropriateness for network
  access, in particular through the `Cloud Optimized GeoTIFF initiative <https://www.cogeo.org/>`_
  whose layout can make use of sub-grids efficient from a network access
  perspective, because grid headers can be put at the beginning of the file, and
  so being retrieved in a single HTTP GET request.

* TIFF can be tiled.

* TIFF can be compressed. Commonly found compression formats are DEFLATE, LZW,
  combined with differential integer or floating point predictors

* A TIFF image can contain a configurable number of channels/bands/samples.
  In the rest of the document, we will use the sample terminology for this concept.

* TIFF sample organization can be configured: either the values of different
  samples are packed together (`PlanarConfiguration <https://www.awaresystems.be/imaging/tiff/tifftags/planarconfiguration.html>`_ = Contig), or put in separate tiles/strips
  (PlanarConfiguration = Separate)

* libtiff is a dependency commonly found in binary distributions of the
  "ecosystem" to which PROJ belongs too

* libtiff benefits from many years of efforts to increase its security, for
  example being integrated to the oss-fuzz initiative. Given the potential
  fetching of grids, using security tested components is an important concern.

* Browser-side: there are "ports" of libtiff/libgeotiff in the browser such
  as https://geotiffjs.github.io/ which could potentially make a port of PROJ
  easier.

Weak points:

* we cannot use libgeotiff, since it depends itself on PROJ (to resolve CRS
  or components of CRS from their EPSG codes). That said, for PROJ intended
  use, we only need to decode the ModelTiepointTag and ModelPixelScaleTag TIFF
  tags, so this can be done "at hand"

* the metadata capabilities of TIFF baseline are limited. The TIFF format comes
  with a predefined set of metadata items whose keys have numeric values. That
  said, GDAL has used for the last 20 years or so a dedicated tag,
  `GDAL_METADATA <https://www.awaresystems.be/imaging/tiff/tifftags/gdal_metadata.html>`_
  of code 42112 that holds a XML-formatted string being able to store arbitrary
  key-pair values.

netCDF v3
*********

Strong points:

* The binary format description as given in
  `OGC 10-092r3 <http://portal.opengeospatial.org/files/?artifact_id=43734>`_ is relatively simple,
  but it would still probably be necessary to use libnetcdf-c to access it

* Metadata can be stored easily in netCDF attributes


Weak points:

* No compression in netCDF v3

* No tiling in netCDF v3

* Multi-samples variables are located in different sections of the files
  (correspond to TIFF PlanarConfiguration = Separate)

* No natural way of having hierarchical / multigrids. They must be encoded as
  separate variables

* georeferencing in netCDF is somewhat less standardized than TIFF/GeoTIFF.
  The generally used model is `the conventions for CF (Climate and Forecast)
  metadata <http://cfconventions.org/>`_
  but there is nothing really handy in them for simple georeferencing with
  the coordinate of the upper-left pixel and the resolution. The practice is
  to write explicit lon and lat variables with all values taken by the grid.
  GDAL has for many years supported a simpler syntax, using a GeoTransform
  attribute.

* From the format description, its layout could be relatively cloud friendly,
  except that libnetcdf has no API to plug an alternate I/O layer.

* Most binary distributions of netCDF nowadays are based on libnetcdf v4, which
  implies the HDF5 dependency.

* From a few issues we identified a few years ago regarding crashes on corrupted
  datasets, we contacted libnetcdf upstream, but they did not seem to be
  interested in addressing those security issues.

netCDF v4 / HDF5
****************

Note: The netCDF v4 format is a profile of the HDF5 file format.

Strong points:

* Compression supported (ZLIB and SZIP predefined)

* Tiling (chunking) supported

* Values of Multi-sample variables can be interleaved together (similarly
  to TIFF PlanarConfiguration = Contig) by using compound data types.

* Hierarchical organization with groups

* While the netCDF API does not provide an alternate I/O layer, this is
  possible with the HDF5 API.

* Grids can be indexed by more than 2 dimensions (for current needs, we
  don't need more than 2D support)

Weak points:

* The `HDF 5 File format <https://support.hdfgroup.org/HDF5/doc/H5.format.html>`_
  is more complex than netCDF v3, and likely more than TIFF. We do not have
  in-depth expertise of it to assess its cloud-friendliness.

* The ones mentioned for netCDF v3 regarding georeferencing and security apply.


GeoPackage
**********

As PROJ has already a SQLite3 dependency, GeoPackage could be examined as a
potential solution.

Strong points:

* SQLite3 dependency

* OGC standard

* Multi-grid capabilities

* Tiling

* Compression

* Metadata capabilities

Weak points:

* GeoPackage mostly address the RGB(A) Byte use case, or via the tile gridded
  data extension, single-sample non-Byte data. No native support for multi-sample
  non-Byte data: each sample should be put in a separate raster table.

* Experience shows that SQLite3 layout (at least the layout adopted when using
  the standard libsqlite3) is not cloud friendly. Indices may be scattered in
  different places of the file.

Conclusions
***********

The 2 major contenders regarding our goals and constraints are GeoTIFF and HDF5.
Given past positive experience and its long history, GeoTIFF remains our preferred
choice.


.. _description_geotiff_format:

Format description
++++++++++++++++++

The format description is available in a dedicated :ref:`geodetictiffgrids`
document.

Tooling
+++++++

A script will be developed to accept a list of individual grids to combine
together into a single file.

A ntv2_to_gtiff.py convenience script will be created to convert NTv2 grids,
including their subgrids, to the above
described GeoTIFF layout.

A validation Python script will be created to check that a file meets the above
described requirements and recommendations.

Build requirements
++++++++++++++++++

The minimum libtiff version will be 4.0 (RHEL 7 ships with libtiff 4.0.3).
To be able to read grids stored on the CDN, libtiff will need to build against
zlib to have DEFLATE and LZW support, which is met by all known binary distributions
of libtiff.

The libtiff dependency can be disabled at build time, but this must be
an explicit setting of configure/cmake as the resulting builds have less functionality.

Dropping grid catalog functionality
-------------------------------------------------------------------------------

While digging through existing code, I more or less discovered that the PROJ
code base has the concept of a grid catalog. This is a feature apparently triggered by
using the +catalog=somefilename.csv in a PROJ string, where the CSV file list
grid names, their extent, priority and date. It seems to be an alternative to using
+nadgrids with multiple grids, with the extra ability to interpolate shift values between
several grids if a +date parameter is provided and the grid catalog mentions a
date for each grids.
It was added in June 2012 per `commit fcb186942ec8532655ff6cf4cc990e5da669a3bc
<https://github.com/OSGeo/PROJ/commit/fcb186942ec8532655ff6cf4cc990e5da669a3bc>`_

This feature is likely unknown to most users as there is no known documentation for
it (neither in current documentation, nor in `historic one <https://web.archive.org/web/20160601000000*/http://trac.osgeo.org/proj/wiki/GenParms>`_).
It is not either tested by PROJ tests, so its working status is unknown. It would
likely make implementation of this RFC easier if this was removed. This would result in
completely dropping the gridcatalog.cpp and gc_reader.cpp files, their call sites
and the catalog_name and datum_date parameter from the PJ structure.

In case similar functionality would be be needed, it might be later reintroduced
as an extra mode of :ref:`hgridshift`, or using a dedicated transformation method,
similarly to the :ref:`deformation` one,
and possibly combining the several grids to interpolate among in the same file,
with a date metadata item.

Backward compatibility issues
-------------------------------------------------------------------------------

None anticipated, except the removal of the (presumably little used) grid catalog
functionality.

Potential future related work
-----------------------------

The foundations set in the definition of the GeoTIFF grid format should hopefully
be reused to extend them to support deformation models (was initially discussed
per https://github.com/OSGeo/PROJ/issues/1001).

Definition of such an extension is out of scope of this RFC.

Documentation
-------------------------------------------------------------------------------

- New API function will be documented.
- A dedicated documentation page will be created to explain the working of
  network-based access.
- A dedicated documentation page will be created to describe the GeoTIFF based
  grid format. Mostly reusing above material.

Testing
-------------------------------------------------------------------------------

Number of GeoTIFF formulations (tiled vs untiled, PlanarConfiguration Separate vs
Contig, data types, scale+offset vs not, etc.) will be tested.

For testing of network capabilities, a mix of real hits to the CDN and use of
the alternate pluggable network interface to test edge cases will be used.

Proposed implementation
-------------------------------------------------------------------------------

A proposed implementation is available at https://github.com/OSGeo/PROJ/pull/1817

Tooling scripts are currently available at https://github.com/rouault/sample_proj_gtiff_grids/
(will be ultimately stored in PROJ repository)

Adoption status
-------------------------------------------------------------------------------

The RFC was adopted on 2020-01-10 with +1's from the following PSC members

* Kristian Evers
* Even Rouault
* Thomas Knudsen
* Howard Butler
* Kurt Schwehr
