.. _datatypes:

================================================================================
Data types
================================================================================

This section describes the numerous data types in use in PROJ.4. As a rule
of thumb PROJ.4 data types are prefixed with ``PJ_``, or in one particular case,
is simply called :c:type:`PJ`. A few notable exceptions can be traced
back to the very early days of PROJ.4 when the ``PJ_`` prefix was not
consistently used.

Transformation objects
--------------------------------------------------------------------------------

.. c:type:: PJ

    Object containing everything related to a given projection or transformation.
    As a user of the PROJ.4 library you are only exposed to pointers to this
    object and the contents is hidden behind the public API. :c:type:`PJ` objects
    are created with :c:func:`proj_create` and destroyed with
    :c:func:`proj_destroy`.

.. c:type:: PJ_DIRECTION

    Enumeration that is used to convey in which direction a given transformation
    should be performed. Used in transformation function call as described in
    the section on :ref:`transformation functions <coord_trans_functions>`.

    Forward transformations are defined with the :c:

    .. code-block:: C

        typedef enum proj_direction {
            PJ_FWD   =  1,   /* Forward    */
            PJ_IDENT =  0,   /* Do nothing */
            PJ_INV   = -1    /* Inverse    */
        } PJ_DIRECTION;

    .. cpp:enumerator:: PJ_FWD

        Perform transformation in the forward direction.

    .. cpp:enumerator:: PJ_IDENT

        Identity. Do nothing.

    .. cpp:enumerator:: PJ_INV

        Perform transformation in the inverse direction.

.. c:type:: PJ_CONTEXT

    Context objects enable safe multi-threaded usage of PROJ.4. Each :c:type:`PJ`
    object is connected to a context (if not specified, the default context is
    used). All operations within a context should be performed in the same thread.
    :c:type:`PJ_CONTEXT` objects are created with :c:func:`proj_context_create`
    and destroyed with :c:func:`proj_context_destroy`.


.. c:type:: PJ_AREA

    .. versionadded:: 6.0.0

    Opaque object describing an area in which a transformation is performed.

    It is used with :c:func:`proj_create_crs_to_crs` to select the best transformation
    between the two input coordinate reference systems.

2 dimensional coordinates
--------------------------------------------------------------------------------

Various 2-dimensional coordinate data types.

.. c:type:: PJ_LP

    Geodetic coordinate, latitude and longitude. Usually in radians.

    .. code-block:: C

        typedef struct { double lam, phi; } PJ_LP;

    .. c:member:: double PJ_LP.lam

        Longitude. Lambda.

    .. c:member:: double PJ_LP.phi

        Latitude. Phi.


.. c:type:: PJ_XY

    2-dimensional cartesian coordinate.

    .. code-block:: C

        typedef struct { double x, y; } PJ_XY;


    .. c:member:: double PJ_XY.x

        Easting.

    .. c:member:: double PJ_XY.y

        Northing.


.. c:type:: PJ_UV

    2-dimensional generic coordinate. Usually used when contents can be either
    a :c:type:`PJ_XY` or :c:type:`PJ_LP`.

    .. code-block:: C

        typedef struct {double u, v; } PJ_UV;


    .. c:member:: double PJ_UV.u

        Longitude or easting, depending on use.

    .. c:member:: double PJ_UV.v

        Latitude or northing, depending on use.


3 dimensional coordinates
--------------------------------------------------------------------------------

The following data types are the 3-dimensional equivalents to the data
types above.

.. c:type:: PJ_LPZ

    3-dimensional version of :c:type:`PJ_LP`. Holds longitude, latitude and
    a vertical component.

    .. code-block:: C

        typedef struct { double lam, phi, z; } PJ_LPZ;

    .. c:member:: double PJ_LPZ.lam

        Longitude. Lambda.

    .. c:member:: double PJ_LPZ.phi

        Latitude. Phi.

    .. c:member:: double PJ_LPZ.z

        Vertical component.


.. c:type:: PJ_XYZ

    Cartesian coordinate in 3 dimensions. Extension  of :c:type:`PJ_XY`.

    .. code-block:: C

        typedef struct { double x, y, z; } PJ_XYZ;

    .. c:member:: double PJ_XYZ.x

        Easting or the X component of a 3D cartesian system.

    .. c:member:: double PJ_XYZ.y

        Northing or the Y component of a 3D cartesian system.

    .. c:member:: double PJ_XYZ.z

        Vertical component or the Z component of a 3D cartesian system.


.. c:type:: PJ_UVW

    3-dimensional extension of :c:type:`PJ_UV`.

    .. code-block:: C

        typedef struct {double u, v, w; } PJ_UVW;

    .. c:member:: double PJ_UVW.u

        Longitude or easting, depending on use.

    .. c:member:: double PJ_UVW.v

        Latitude or northing, depending on use.

    .. c:member:: double PJ_UVW.w

        Vertical component.


Spatiotemporal coordinate types
--------------------------------------------------------------------------------

The following data types are extensions of the triplets above into the time
domain.


.. c:type:: PJ_LPZT

    Spatiotemporal version of :c:type:`PJ_LPZ`.

    .. code-block:: C

        typedef struct {
            double lam;
            double phi;
            double z;
            double t;
        } PJ_LPZT;


    .. c:member:: double PJ_LPZT.lam

        Longitude.

    .. c:member:: double PJ_LPZT.phi

        Latitude

    .. c:member:: double PJ_LPZT.z

        Vertical component.

    .. c:member:: double PJ_LPZT.t

        Time component.

.. c:type:: PJ_XYZT

    Generic spatiotemporal coordinate. Useful for e.g. cartesian coordinates with
    an attached time-stamp.

    .. code-block:: C

        typedef struct {
            double x;
            double y;
            double z;
            double t;
        } PJ_XYZT;


    .. c:member:: double PJ_XYZT.x

        Easting or the X component of a 3D cartesian system.

    .. c:member:: double PJ_XYZT.y

        Northing or the Y component of a 3D cartesian system.

    .. c:member:: double PJ_XYZT.z

        Vertical or the Z component of a 3D cartesian system.

    .. c:member:: double PJ_XYZT.t

        Time component.


.. c:type:: PJ_UVWT

    Spatiotemporal version of :c:type:`PJ_UVW`.

    .. code-block:: C

        typedef struct { double u, v, w, t; } PJ_UVWT;

    .. c:member:: double PJ_UVWT.e

        First horizontal component.

    .. c:member:: double PJ_UVWT.n

        Second horizontal component.

    .. c:member:: double PJ_UVWT.w

        Vertical component.

    .. c:member:: double PJ_UVWT.t

        Temporal component.


Ancillary types for geodetic computations
--------------------------------------------------------------------------------


.. c:type:: PJ_OPK

    Rotations, for instance three euler angles.

    .. code-block:: C

        typedef struct { double o, p, k; } PJ_OPK;

    .. c:member:: double PJ_OPK.o

        First rotation angle, omega.

    .. c:member:: double PJ_OPK.p

        Second rotation angle, phi.

    .. c:member:: double PJ_OPK.k

        Third rotation angle, kappa.

.. c:type:: PJ_ENU

    East, north and up components.

    .. code-block:: c

        typedef struct { double e, n, u; }          PJ_ENU;

    .. c:member:: double PJ_ENU.e

        East component.

    .. c:member:: double PJ_ENU.n

        North component.

    .. c:member:: double PJ_ENU.u

        Up component.


.. c:type:: PJ_GEOD

    Geodesic length, forward and reverse azimuths.

    .. code-block:: C

        typedef struct { double s, a1, a2; }        PJ_GEOD;

    .. c:member:: double PJ_GEOD.s

        Geodesic length.

    .. c:member:: double PJ_GEOD.a1

        Forward azimuth.

    .. c:member:: double PJ_GEOD.a2

        Reverse azimuth.



Complex coordinate types
--------------------------------------------------------------------------------


.. c:type:: PJ_COORD

    General purpose coordinate union type, applicable in two, three and four dimensions.
    This is the default coordinate datatype used in PROJ.

    .. code-block:: C

        typedef union {
            double v[4];
            PJ_XYZT xyzt;
            PJ_UVWT uvwt;
            PJ_LPZT lpzt;
            PJ_GEOD geod;
            PJ_OPK opk;
            PJ_ENU enu;
            PJ_XYZ  xyz;
            PJ_UVW  uvw;
            PJ_LPZ  lpz;
            PJ_XY   xy;
            PJ_UV   uv;
            PJ_LP   lp;
        } PJ_COORD ;

    .. c:member:: double v[4]

        Generic four-dimensional vector.

    .. c:member:: PJ_XYZT PJ_COORD.xyzt

        Spatiotemporal cartesian coordinate.

    .. c:member:: PJ_UVWT PJ_COORD.uvwt

        Spatiotemporal generic coordinate.

    .. c:member:: PJ_LPZT PJ_COORD.lpzt

        Longitude, latitude, vertical and time components.

    .. c:member:: PJ_GEOD PJ_COORD.geod

        Geodesic length, forward and reverse azimuths.

    .. c:member:: PJ_OPK PJ_COORD.opk

        Rotations, for instance three euler angles.

    .. c:member:: PJ_ENU PJ_COORD.enu

        East, north and up components.

    .. c:member:: PJ_XYZ PJ_COORD.xyz

        3-dimensional cartesian coordinate.

    .. c:member:: PJ_UVW PJ_COORD.uvw

        3-dimensional generic coordinate.

    .. c:member:: PJ_LPZ PJ_COORD.lpz

        Longitude, latitude and vertical component.

    .. c:member:: PJ_XY PJ_COORD.xy

        2-dimensional cartesian coordinate.

    .. c:member:: PJ_UV PJ_COORD.uv

        2-dimensional generic coordinate.

    .. c:member:: PJ_LP PJ_COORD.lp

        Longitude and latitude.


Projection derivatives
-------------------------------------------------------------------------------

.. c:type:: PJ_FACTORS

    Various cartographic properties, such as scale factors, angular distortion
    and meridian convergence. Calculated with :c:func:`proj_factors`.

    .. code-block:: C

        typedef struct {
            double meridional_scale;
            double parallel_scale;
            double areal_scale;

            double angular_distortion;
            double meridian_parallel_angle;
            double meridian_convergence;

            double tissot_semimajor;
            double tissot_semiminor;

            double dx_dlam;
            double dx_dphi;
            double dy_dlam;
            double dy_dphi;
        } PJ_FACTORS;

    .. c:member:: double PJ_FACTORS.meridional_scale

        Meridional scale at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.parallel_scale

        Parallel scale at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.areal_scale

        Areal scale factor at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.angular_distortion

        Angular distortion at coordinate :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.meridian_parallel_angle

         Meridian/parallel angle, :math:`\theta^\prime`, at coordinate :math:`\left(\lambda,\phi\right)`.

     .. c:member:: double PJ_FACTORS.meridian_convergence

        Meridian convergence at coordinate :math:`\left(\lambda,\phi\right)`.
        Sometimes also described as *grid declination*.


    .. c:member:: double PJ_FACTORS.tissot_semimajor

        Maximum scale factor.

    .. c:member:: double PJ_FACTORS.tissot_semiminor

        Minimum scale factor.


    .. c:member:: double PJ_FACTORS.dx_dlam

        Partial derivative :math:`\frac{\partial x}{\partial \lambda}` of coordinate
        :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.dy_dlam

        Partial derivative :math:`\frac{\partial y}{\partial \lambda}` of coordinate
        :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.dx_dphi

        Partial derivative :math:`\frac{\partial x}{\partial \phi}` of coordinate
        :math:`\left(\lambda,\phi\right)`.

    .. c:member:: double PJ_FACTORS.dy_dphi

        Partial derivative :math:`\frac{\partial y}{\partial \phi}` of coordinate
        :math:`\left(\lambda,\phi\right)`.

List structures
-------------------------------------------------------------------------------

.. c:type:: PJ_OPERATIONS

    Description a PROJ.4 operation

    .. code-block:: C

        struct PJ_OPERATIONS {
            const char  *id;            /* operation keyword */
            PJ *(*proj)(PJ *);          /* operation  entry point */
            char    * const *descr;     /* description text */
        };

    .. c:member:: const char *id

        Operation keyword.

    .. c:member:: PJ *(*op)(PJ *)

        Operation entry point.

    .. c:member:: char * const * descr

        Description of operation.


.. c:type:: PJ_ELLPS

    Description of ellipsoids defined in PROJ.4

    .. code-block:: C

        struct PJ_ELLPS {
            const char  *id;
            const char  *major;
            const char  *ell;
            const char  *name;
        };

    .. c:member:: const char *id

        Keyword name of the ellipsoid.

    .. c:member:: const char *major

        Semi-major axis of the ellipsoid, or radius in case of a sphere.

    .. c:member:: const char *ell

        Elliptical parameter, e.g. ``rf=298.257`` or ``b=6356772.2``.

    .. c:member:: const char *name

        Name of the ellipsoid

.. c:type:: PJ_UNITS

    Distance units defined in PROJ.

    .. code-block:: C

        struct PJ_UNITS {
            const char  *id;        /* units keyword */
            const char  *to_meter;  /* multiply by value to get meters */
            const char  *name;      /* comments */
            double      factor;     /* to_meter factor in actual numbers */
        };

    .. c:member:: const char *id

        Keyword for the unit.

    .. c:member:: const char *to_meter

        Text representation of the factor that converts a given unit to meters

    .. c:member:: const char *name

        Name of the unit.

    .. c:member:: double factor

        Conversion factor that converts the unit to meters.

.. c:type:: PJ_PRIME_MERIDIANS

    Prime meridians defined in PROJ.

    .. code-block:: C

        struct PJ_PRIME_MERIDIANS {
            const char  *id;
            const char  *defn;
        };

    .. c:member:: const char *id

        Keyword for the prime meridian

    .. c:member:: const char *def

        Offset from Greenwich in DMS format.

Info structures
-------------------------------------------------------------------------------

.. c:type:: PJ_INFO

    Struct holding information about the current instance of PROJ. Struct is
    populated by :c:func:`proj_info`.

    .. code-block:: C

        typedef struct {
            int           major;
            int           minor;
            int           patch;
            const char   *release;
            const char   *version;
            const char   *searchpath;
        } PJ_INFO;

    .. c:member:: const char *PJ_INFO.release

        Release info. Version number and release date,
        e.g. "Rel. 4.9.3, 15 August 2016".

    .. c:member:: const char *PJ_INFO.version

        Text representation of the full version number,
        e.g. "4.9.3".

    .. c:member:: int PJ_INFO.major

        Major version number.

    .. c:member:: int PJ_INFO.minor

        Minor version number.

    .. c:member:: int PJ_INFO.patch

        Patch level of release.

    .. c:member:: const char PJ_INFO.searchpath

        Search path for PROJ. List of directories separated by
        semicolons (Windows) or colons (non-Windows), e.g.
        ``C:\\Users\\doctorwho;C:\\OSGeo4W64\\share\\proj``.
        Grids and :ref:`init files <init_files>` are looked for in
        directories in the search path.

.. c:type:: PJ_PROJ_INFO

    Struct holding information about a :c:type:`PJ` object. Populated by
    :c:func:`proj_pj_info`. The :c:type:`PJ_PROJ_INFO` object provides a
    view into the internals of a :c:type:`PJ`, so once the :c:type:`PJ`
    is destroyed or otherwise becomes invalid, so does the
    :c:type:`PJ_PROJ_INFO`

    .. code-block:: C

        typedef struct {
            const char  *id;
            const char  *description;
            const char  *definition;
            int          has_inverse;
            double       accuracy;
        } PJ_PROJ_INFO;

    .. c:member:: const char *PJ_PROJ_INFO.id

        Short ID of the operation the :c:type:`PJ` object is based on, that is,
        what comes after the ``+proj=`` in a proj-string, e.g. "*merc*".

    .. c:member:: const char *PJ_PROJ_INFO.description

        Long describes of the operation the :c:type:`PJ` object is based on,
        e.g. "*Mercator    Cyl, Sph&Ell   lat_ts=*".

    .. c:member:: const char *PJ_PROJ_INFO.definition

        The proj-string that was used to create the :c:type:`PJ` object with,
        e.g. "*+proj=merc +lat_0=24 +lon_0=53 +ellps=WGS84*".

    .. c:member:: int PJ_PROJ_INFO.has_inverse

        1 if an inverse mapping of the defined operation exists, otherwise 0.

    .. c:member:: double PJ_PROJ_INFO.accuracy

        Expected accuracy of the transformation. -1 if unknown.

.. c:type:: PJ_GRID_INFO

    Struct holding information about a specific grid in the search path of
    PROJ. Populated with the function :c:func:`proj_grid_info`.

    .. code-block:: C

        typedef struct {
            char        gridname[32];
            char        filename[260];
            char        format[8];
            LP          lowerleft;
            LP          upperright;
            int         n_lon, n_lat;
            double      cs_lon, cs_lat;
        } PJ_GRID_INFO;

    .. c:member:: char PJ_GRID_INFO.gridname[32]

        Name of grid, e.g. "*BETA2007.gsb*".

    .. c:member:: char PJ_GRID_INFO

        Full path of grid file, e.g. *"C:\\OSGeo4W64\\share\\proj\\BETA2007.gsb"*

    .. c:member:: char PJ_GRID_INFO.format[8]

        File format of grid file, e.g. "*ntv2*"

    .. c:member:: LP PJ_GRID_INFO.lowerleft

        Geodetic coordinate of lower left corner of grid.

    .. c:member:: LP PJ_GRID_INFO.upperright

        Geodetic coordinate of upper right corner of grid.

    .. c:member:: int PJ_GRID_INFO.n_lon

        Number of grid cells in the longitudinal direction.

    .. c:member:: int PJ_GRID_INFO.n_lat

        Number of grid cells in the latitudinal direction.

    .. c:member:: double PJ_GRID_INFO.cs_lon

        Cell size in the longitudinal direction. In radians.

    .. c:member:: double PJ_GRID_INFO.cs_lat

        Cell size in the latitudinal direction. In radians.


.. c:type:: PJ_INIT_INFO

    Struct holding information about a specific init file in the search path of
    PROJ. Populated with the function :c:func:`proj_init_info`.

    .. code-block:: C

        typedef struct {
            char        name[32];
            char        filename[260];
            char        version[32];
            char        origin[32];
            char        lastupdate[16];
        } PJ_INIT_INFO;

    .. c:member:: char PJ_INIT_INFO.name[32]

        Name of init file, e.g. "*epsg*".

    .. c:member:: char PJ_INIT_INFO.filename[260]

        Full path of init file, e.g. "*C:\\OSGeo4W64\\share\\proj\\epsg*"

    .. c:member:: char PJ_INIT_INFO.version[32]

        Version number of init file, e.g. "*9.0.0*"

    .. c:member:: char PJ_INIT_INFO.origin[32]

        Originating entity of the init file, e.g. "*EPSG*"

    .. c:member:: char PJ_INIT_INFO.lastupdate

        Date of last update of the init file.


.. _error_codes:

Error codes
-----------

.. versionadded:: 8.0.0

Three classes of errors are defined below. The belonging of a given error
code to a class can bit tested with a binary and test. The error class itself
can be used as an error value in some rare cases where the error does not
fit into a more precise error value.

Those error codes are still quite generic for a number of them. Details on the
actual errors will be typically logged with the PJ_LOG_ERROR level.

Errors in class PROJ_ERR_INVALID_OP
+++++++++++++++++++++++++++++++++++

.. c:macro:: PROJ_ERR_INVALID_OP

    Class of error codes typically related to coordinate operation initialization,
    typically when creating a PJ* object from a PROJ string.

    .. note:: some of them can also be emitted during coordinate transformation,
              like PROJ_ERR_INVALID_OP_FILE_NOT_FOUND_OR_INVALID in case the resource loading
              is deferred until it is really needed.

.. c:macro:: PROJ_ERR_INVALID_OP_WRONG_SYNTAX

    Invalid pipeline structure, missing +proj argument, etc.

.. c:macro:: PROJ_ERR_INVALID_OP_MISSING_ARG

    Missing required operation parameter

.. c:macro:: PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE

    One of the operation parameter has an illegal value.

.. c:macro:: PROJ_ERR_INVALID_OP_MUTUALLY_EXCLUSIVE_ARGS

    Mutually exclusive arguments

.. c:macro:: PROJ_ERR_INVALID_OP_FILE_NOT_FOUND_OR_INVALID

    File not found or with invalid content (particular case of PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE)

Errors in class PROJ_ERR_COORD_TRANSFM
++++++++++++++++++++++++++++++++++++++

.. c:macro:: PROJ_ERR_COORD_TRANSFM

    Class of error codes related to transformation on a specific coordinate.

.. c:macro:: PROJ_ERR_COORD_TRANSFM_INVALID_COORD

    Invalid input coordinate. e.g. a latitude > 90°.

.. c:macro:: PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN

    Coordinate is outside of the projection domain. e.g. approximate mercator
    with \|longitude - lon_0\| > 90°, or iterative convergence method failed.

.. c:macro:: PROJ_ERR_COORD_TRANSFM_NO_OPERATION

    No operation found, e.g. if no match the required accuracy, or if ballpark transformations
    were asked to not be used and they would be only such candidate.

.. c:macro:: PROJ_ERR_COORD_TRANSFM_OUTSIDE_GRID

    Point to transform falls outside grid/subgrid/TIN.

.. c:macro:: PROJ_ERR_COORD_TRANSFM_GRID_AT_NODATA

    Point to transform falls in a grid cell that evaluates to nodata.

Errors in class PROJ_ERR_OTHER
++++++++++++++++++++++++++++++

.. c:macro:: PROJ_ERR_OTHER

    Class of error codes that do not fit into one of the above class.

.. c:macro:: PROJ_ERR_OTHER_API_MISUSE

    Error related to a misuse of PROJ API.

.. c:macro:: PROJ_ERR_OTHER_NO_INVERSE_OP

    No inverse method available

.. c:macro:: PROJ_ERR_OTHER_NETWORK_ERROR

    Failure when accessing a network resource.


Logging
-------------------------------------------------------------------------------

.. c:type:: PJ_LOG_LEVEL

    Enum of logging levels in PROJ. Used to set the logging level in PROJ.
    Usually using :c:func:`proj_log_level`.

    .. cpp:enumerator:: PJ_LOG_NONE

        Don't log anything.

    .. cpp:enumerator:: PJ_LOG_ERROR

        Log only errors.

    .. cpp:enumerator:: PJ_LOG_DEBUG

        Log errors and additional debug information.

    .. cpp:enumerator:: PJ_LOG_TRACE

        Highest logging level. Log everything including very detailed debug
        information.

    .. cpp:enumerator:: PJ_LOG_TELL

        Special logging level that when used in :c:func:`proj_log_level`
        will return the current logging level set in PROJ.

    .. versionadded:: 5.1.0

.. c:type:: PJ_LOG_FUNC

    Function prototype for the logging function used by PROJ.
    Defined as

    .. code-block:: C

        typedef void (*PJ_LOG_FUNCTION)(void *, int, const char *);

    where the first argument (void pointer) references a data structure used by the
    calling application, the second argument (int type) is used to set the logging level
    and the third argument (const char pointer) is the string that will be logged
    by the function.


    .. versionadded:: 5.1.0


Setting custom I/O functions
-------------------------------------------------------------------------------

.. versionadded:: 7.0.0

.. doxygenstruct:: PROJ_FILE_API
   :project: doxygen_api
   :members:

.. doxygentypedef:: PROJ_FILE_HANDLE
   :project: doxygen_api

.. doxygenenum:: PROJ_OPEN_ACCESS
   :project: doxygen_api


Network related functionality
-------------------------------------------------------------------------------

.. versionadded:: 7.0.0

.. doxygentypedef:: PROJ_NETWORK_HANDLE
   :project: doxygen_api

.. doxygentypedef:: proj_network_open_cbk_type
   :project: doxygen_api

.. doxygentypedef:: proj_network_close_cbk_type
   :project: doxygen_api

.. doxygentypedef:: proj_network_get_header_value_cbk_type
   :project: doxygen_api

.. doxygentypedef:: proj_network_read_range_type
   :project: doxygen_api


C API for ISO-19111 functionality
-------------------------------------------------------------------------------

.. doxygengroup:: iso19111_types
   :project: doxygen_api
   :content-only:
   :members:


