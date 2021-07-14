.. _news:

News
###############################################################################

8.1.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*July 1st 2021*

Updates
-------

+ **Database**

  * Update to EPSG v10.027 (`#2751 <https://github.com/OSGeo/PROJ/issues/2751>`_)

  * Decrease DB size by using ``WITHOUT ROWID`` tables (`#2730 <https://github.com/OSGeo/PROJ/issues/2730>`_) (#2647)

  * Add a ``ANALYZE`` step during ``proj.db`` creation allowing for faster lookups (`#2729 <https://github.com/OSGeo/PROJ/issues/2729>`_)

  * Added a ``PROJ.VERSION`` metadata entry (`#2646 <https://github.com/OSGeo/PROJ/issues/2646>`_)

  * Added NGO48 (EPSG:4273) to ETRS89 (EPSG:4258) triangulation-based transformation (`#2554 <https://github.com/OSGeo/PROJ/issues/2554>`_)

  * Additions to the norwegian NKG2020 transformation (`#2548 <https://github.com/OSGeo/PROJ/issues/2548>`_)

  * ESRI projection database updated to version 12.8 (`#2717 <https://github.com/OSGeo/PROJ/issues/2717>`_)

+ **API additions**

  * Added :c:func:`proj_get_geoid_models_from_database()` function that returns a list of geoid models available for a given CRS (`#2681 <https://github.com/OSGeo/PROJ/issues/2681>`_)

  * Added :c:func`proj_get_celestial_body_list_from_database()` that returns a list of celestial bodies in the PROJ database (`#2667 <https://github.com/OSGeo/PROJ/issues/2667>`_)

  * Added :c:func:`proj_get_celestial_body_name()` (`#2662 <https://github.com/OSGeo/PROJ/issues/2662>`_)

+ **Various improvements**

  * :c:func:`proj_trans()`/:program:`cs2cs`: If two operations have the same accuracy, use the one that is contained within a larger one (`#2750 <https://github.com/OSGeo/PROJ/issues/2750>`_)

  * Share SQLite database handle among all contexts (`#2738 <https://github.com/OSGeo/PROJ/issues/2738>`_)

  * Added ``proj/internal/mutex.hpp`` as compat layer for mingw32 for std::mutex (`#2736 <https://github.com/OSGeo/PROJ/issues/2736>`_)

  * :program:`projsync`: make it filter out files not intended for the current version (`#2725 <https://github.com/OSGeo/PROJ/issues/2725>`_)

  * Improvements related to ``DerivedVerticalCRS`` using Change Unit and
    Height/Depth reversal methods (`#2696 <https://github.com/OSGeo/PROJ/issues/2696>`_)

  * Update internal ``nlohmann/json`` to 3.9.1, and add a CMake option to
    be able to use external ``nlohmann/json`` (`#2686 <https://github.com/OSGeo/PROJ/issues/2686>`_)

  * :cpp:func:`createFromUserInput()`: change name of CRS built from URN combined references to match the convention of EPSG projected CRS (`#2677 <https://github.com/OSGeo/PROJ/issues/2677>`_)

  * Parse compound id with two authorities, like ESRI:103668+EPSG:5703 (`#2669 <https://github.com/OSGeo/PROJ/issues/2669>`_)

  * Added :program:`projinfo` option option ``--list-crs`` (supports ``--area``) (`#2663 <https://github.com/OSGeo/PROJ/issues/2663>`_)

  * Added support for hyperbolic Cassini-Soldner (`#2637 <https://github.com/OSGeo/PROJ/issues/2637>`_)

  * Added capability to get SQL statements to add custom CRS in the database (`#2577 <https://github.com/OSGeo/PROJ/issues/2577>`_)

Bug fixes
---------

* Fix 'Please include winsock2.h before windows.h' warning with msys (`#2692 <https://github.com/OSGeo/PROJ/issues/2692>`_)

* Minor changes to address lint in ``geodesic.c`` (`#2752 <https://github.com/OSGeo/PROJ/issues/2752>`_)

* :cpp:func:`BoundCRS::identify()`: avoid incompatible transformation for WKT1 / TOWGS84 export (`#2747 <https://github.com/OSGeo/PROJ/issues/2747>`_)

* :c:func:`proj_create()`: do not open proj.db if string is a PROJ string, even if :c:func:`proj_context_set_autoclose_database()` has been set (`#2735 <https://github.com/OSGeo/PROJ/issues/2735>`_)

* Fix export of transformation to PROJ string in a particular situation where CompoundCRS are involved (`#2721 <https://github.com/OSGeo/PROJ/issues/2721>`_)


8.0.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*May 5th 2021*

Updates
-------

* Database: update to EPSG v10.018 (`#2636 <https://github.com/OSGeo/PROJ/issues/2636>`_)

* Add transformations for CHGeo2004, Swiss geoid model (`#2604 <https://github.com/OSGeo/PROJ/issues/2604>`_)

* Additions to the norwegian NKG2020 transformation (`#2600 <https://github.com/OSGeo/PROJ/issues/2600>`_)

Bug fixes
---------

* :c:func:`pj_vlog()`: fix buffer overflow in case of super lengthy error message (`#2693 <https://github.com/OSGeo/PROJ/issues/2693>`_)

* Revert ":c:func:`proj_create_crs_to_crs_from_pj()`: do not use PROJ_SPATIAL_CRITERION_PARTIAL_INTERSECTION if area is specified" (`#2679 <https://github.com/OSGeo/PROJ/issues/2679>`_)

* UTM: error out when value of ``+zone=`` is not an integer (`#2672 <https://github.com/OSGeo/PROJ/issues/2672>`_)

* :cpp:func:`getCRSInfoList()`: make result order deterministic (by increasing auth_name,
  code) (`#2661 <https://github.com/OSGeo/PROJ/issues/2661>`_)

* :cpp:func:`createOperation()`: make sure no to discard deprecated operations if the
  replacement uses an unknow grid (`#2623 <https://github.com/OSGeo/PROJ/issues/2623>`_)

* Fix build on Solaris 11.4 (`#2621 <https://github.com/OSGeo/PROJ/issues/2621>`_)

* Add mapping of ESRI Equal_Area projection method to EPSG (`#2612 <https://github.com/OSGeo/PROJ/issues/2612>`_)

* Fix incorrect EPGS extent code for EPSG:7789>EPSG:4976 NKG transformation (`#2599 <https://github.com/OSGeo/PROJ/issues/2599>`_)

* fix wrong capitalization of CHENyx06_ETRS.gsb (`#2597 <https://github.com/OSGeo/PROJ/issues/2597>`_)

* :cpp:func:`createOperations()`: improve handling of vertical transforms when
  when compound CRSs are used (`#2592 <https://github.com/OSGeo/PROJ/issues/2592>`_)

* :cpp:func:`CRS::promoteTo3D()`: propagate the extent from the 2D CRS (`#2589 <https://github.com/OSGeo/PROJ/issues/2589>`_)

* :cpp:func:`createFromCRSCodesWithIntermediates()`: improve performance when there is
  no match (`#2583 <https://github.com/OSGeo/PROJ/issues/2583>`_)

* Fix :c:func:`proj_clone()` to work on 'meta' coordinate operation ``PJ*`` objects that
  can be returned by :c:func:`proj_create_crs_to_crs()` (`#2582 <https://github.com/OSGeo/PROJ/issues/2582>`_)

* add ``PROJ_COMPUTE_VERSION``, ``PROJ_VERSION_NUMBER``,
  ``PROJ_AT_LEAST_VERSION`` macros (`#2581 <https://github.com/OSGeo/PROJ/issues/2581>`_)

* Make :c:func:`proj_lp_dist()` and :c:func:`proj_geod()` work on a ``PJ*`` CRS object (`#2570 <https://github.com/OSGeo/PROJ/issues/2570>`_)

* Fix gcc 11 ``-Wnonnull`` compilation warnings (`#2559 <https://github.com/OSGeo/PROJ/issues/2559>`_)

* Fix use of uninitialized memory in gie tests (`#2558 <https://github.com/OSGeo/PROJ/issues/2558>`_)

* :c:func:`createOperations()`: fix incorrect height transformation between 3D promoted RGF93 and CH1903+ (`#2555 <https://github.com/OSGeo/PROJ/issues/2555>`_)



>>>>>>> Stashed changes
8.0.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*March 1st 2021*

With the release of PROJ 8 the ``proj_api.h`` API is finally removed. See
:ref:`API_migration` for more info on how to migrate from the old to the
``proj.h`` API.

With the removal of ``proj_api.h`` it has been possible to simplify error codes
and messages given by the software. The error codes are exposed in the API.

Several improvements has been made to the command line utilities as well as
tweaks in the underlying API.

Updates
-------

* Public header file ``proj_api.h`` removed (`#837 <https://github.com/OSGeo/PROJ/issues/837>`_)

* Improved accuracy of the Mercator projection (`#2397 <https://github.com/OSGeo/PROJ/issues/2397>`_)

* Copyright statement wording updated (`#2417 <https://github.com/OSGeo/PROJ/issues/2417>`_)

* Allow :program:`cct` to instantiate operations via object codes or names (`#2419 <https://github.com/OSGeo/PROJ/issues/2419>`_)

* Allow ``@filename`` syntax in :program:`cct` (`#2420 <https://github.com/OSGeo/PROJ/issues/2420>`_)

* Added :ref:`topocentric` (``+proj=topocentric``) (`#2444 <https://github.com/OSGeo/PROJ/issues/2444>`_)

* Update GeographicLib to version 1.51 (`#2445 <https://github.com/OSGeo/PROJ/issues/2445>`_)

* Added option to allow export of Geographic/Projected 3D CRS
  in WKT1_GDAL (`#2450 <https://github.com/OSGeo/PROJ/issues/2450>`_)

* Added ``--area`` and ``--bbox`` options in :program:`cs2cs` to restrict candidate
  coordinate operations (`#2466 <https://github.com/OSGeo/PROJ/issues/2466>`_)

* Added build time option to make :envvar:`PROJ_LIB` env var tested last (`#2476 <https://github.com/OSGeo/PROJ/issues/2476>`_)

* Added ``--authority`` switch in :program:`cs2cs` to control where coordinate operations
  are looked for. C API function :c:func:`proj_create_crs_to_crs_from_pj()` updated
  accordingly (`#2477 <https://github.com/OSGeo/PROJ/issues/2477>`_)

* Error codes revised and exposed in the public API (`#2487 <https://github.com/OSGeo/PROJ/issues/2487>`_)

* Added ``--accuracy`` options to :program:`projinfo`. C API function
  :c:func:`proj_create_crs_to_crs_from_pj()` updated accordingly (`#2488 <https://github.com/OSGeo/PROJ/issues/2488>`_)

* Added :c:func:`proj_crs_is_derived()` function to C API (`#2496 <https://github.com/OSGeo/PROJ/issues/2496>`_)

* Enabled linking against static cURL on Windows (`#2514 <https://github.com/OSGeo/PROJ/issues/2514>`_)

* Updated ESRI CRS database to 12.7 (10.8.1/2.6) (`#2519 <https://github.com/OSGeo/PROJ/issues/2519>`_)

* Allow a WKT BoundCRS to use a PROJ string transformation (`#2521 <https://github.com/OSGeo/PROJ/issues/2521>`_)

* Update to EPSG v10.015 (`#2539 <https://github.com/OSGeo/PROJ/issues/2539>`_)

* Default log level set to ``PJ_LOG_ERROR`` (`#2542 <https://github.com/OSGeo/PROJ/issues/2542>`_)

* CMake installs a pkg-config file ``proj.pc``, where supported (`#2547 <https://github.com/OSGeo/PROJ/issues/2547>`_)

Bug fixes
---------

* Do not restrict longitude to [-90;90] range in spherical transverse Mercator
  forward projection (`#2471 <https://github.com/OSGeo/PROJ/issues/2471>`_)

* :cpp:func:`createOperations()`: fix Compound to Geog3D/Projected3D CRS with non-metre ellipsoidal height (`#2500 <https://github.com/OSGeo/PROJ/issues/2500>`_)

* Avoid error messages to be emitted log level is set to ``PJ_LOG_NONE`` (`#2527 <https://github.com/OSGeo/PROJ/issues/2527>`_)

* Close database connection when autoclose set to True (`#2532 <https://github.com/OSGeo/PROJ/issues/2532>`_)


7.2.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*January 1st 2021*

Updates
-------

* Add metadata with the version number of the database layout (`#2474 <https://github.com/OSGeo/PROJ/issues/2474>`_)

* Split ``coordinateoperation.cpp`` and ``test_operation.cpp`` in several parts (`#2484 <https://github.com/OSGeo/PROJ/issues/2484>`_)

* Update to EPSG v10.008 (`#2490 <https://github.com/OSGeo/PROJ/issues/2490>`_)

* Added the NKG 2008 and 2020 transformations in ``proj.db`` (`#2495 <https://github.com/OSGeo/PROJ/issues/2495>`_)

Bug fixes
---------

* Set ``CURL_ENABLED`` definition on projinfo build (`#2405 <https://github.com/OSGeo/PROJ/issues/2405>`_)

* :c:func:`createBoundCRSToWGS84IfPossible()`: make it return same result with a CRS
  built from EPSG code or WKT1 (`#2412 <https://github.com/OSGeo/PROJ/issues/2412>`_)

* WKT2 parsing: several fixes related to map projection parameter units (`#2428 <https://github.com/OSGeo/PROJ/issues/2428>`_)

* :c:func:`createOperation()`: make it work properly when one of the CRS is a BoundCRS of
  a DerivedGeographicCRS (``+proj=ob_tran +o_proj=lonlat +towgs84=....``) (`#2441 <https://github.com/OSGeo/PROJ/issues/2441>`_)

* WKT parsing: fix ingestion of WKT with a Geocentric CRS as the base of the
  projected CRS (`#2443 <https://github.com/OSGeo/PROJ/issues/2443>`_)

* ``GeographicCRS::_isEquivalentTo(EQUIVALENT_EXCEPT_AXIS_ORDER_GEOGCRS)``:
  make it work when comparing easting,northing,up and northing,easting,up (`#2446 <https://github.com/OSGeo/PROJ/issues/2446>`_)

* :c:func:`createOperation()`: add a ballpark vertical transformation when dealing
  with ``GEOIDMODEL[]`` (`#2449 <https://github.com/OSGeo/PROJ/issues/2449>`_)

* Use same arguments to printf format string for both radians and degrees in
  output by cct (`#2453 <https://github.com/OSGeo/PROJ/issues/2453>`_)

* PRIMEM WKT handling: fixes on import for 'sexagesimal DMS' or from WKT1:GDAL/ESRI
  when GEOGCS UNIT != Degree; morph to ESRI the PRIMEM name on export (`#2455 <https://github.com/OSGeo/PROJ/issues/2455>`_)

* :c:func:`createObjectsFromName()`: in exact match, make looking for 'ETRS89 / UTM zone 32N'
  return only the exact match (`#2462 <https://github.com/OSGeo/PROJ/issues/2462>`_)

* Inverse tmerc spherical: fix wrong sign of latitude when lat_0 is used (`#2469 <https://github.com/OSGeo/PROJ/issues/2469>`_)

* Add option to allow export of Geographic/Projected 3D CRS in WKT1_GDAL (`#2470 <https://github.com/OSGeo/PROJ/issues/2470>`_)

* Fix building ``proj.db`` with SQLite built with ``-DSQLITE_DQS=0`` (`#2480 <https://github.com/OSGeo/PROJ/issues/2480>`_)

* Include JSON Schema files in CMake builds (`#2485 <https://github.com/OSGeo/PROJ/issues/2485>`_)

* :c:func:`createOperations()`: fix inconsistent chaining exception when transforming from BoundCRS of projected CRS based on NTF Paris to BoundCRS of geog CRS NTF Paris (`#2486 <https://github.com/OSGeo/PROJ/issues/2486>`_)


7.2.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*November 1st 2020*

Updates
-------

+ **Command line tools**

  * Add multi-line PROJ string export capability, and use it by default in
    :program:`projinfo` (unless ``--single-line`` is specified) (`#2381 <https://github.com/OSGeo/PROJ/issues/2381>`_)

+ **Coordinate operations**

   * :ref:`col_urban` projection, implementing a EPSG projection method
     used by a number of projected CRS in Colombia (`#2395 <https://github.com/OSGeo/PROJ/issues/2395>`_)

   * :ref:`tinshift` for triangulation-based transformations (`#2344 <https://github.com/OSGeo/PROJ/issues/2344>`_)

   * Added ellipsoidal formulation of :ref:`ortho` (`#2361 <https://github.com/OSGeo/PROJ/issues/2361>`_)


+ **Database**

  * Update to EPSG 10.003 and make code base robust to dealing with
    WKT CRS with DatumEnsemble (`#2370 <https://github.com/OSGeo/PROJ/issues/2370>`_)

  * Added Finland tinshift operations (`#2392 <https://github.com/OSGeo/PROJ/issues/2392>`_)

  * Added transformation from JGD2011 Geographic 3D to JGD2011
    height using GSIGEO2011 (`#2393 <https://github.com/OSGeo/PROJ/issues/2393>`_)

  * Improve CompoundCRS identification and name morphing in VerticalCRS
    with ESRI WKT1 (`#2386 <https://github.com/OSGeo/PROJ/issues/2386>`_)

  * Added OGC:CRS27 and OGC:CRS83 CRS entries for NAD27 and NAD83
    in longitude, latitude order (`#2350 <https://github.com/OSGeo/PROJ/issues/2350>`_)

+ **API**

  * Added temporal, engineering, and parametric datum :c:type:`PJ_TYPE` enumerations (`#2274 <https://github.com/OSGeo/PROJ/issues/2274>`_)

  * Various improvements to context handling (#2329, #2331)

  * :c:func:`proj_create_vertical_crs_ex()`: add a ``ACCURACY`` option to provide
    an explicit accuracy, or derive it from the grid name if it is
    known (`#2342 <https://github.com/OSGeo/PROJ/issues/2342>`_)

  * :c:func:`proj_crs_create_bound_crs_to_WGS84()`: make it work on
    verticalCRS/compoundCRS such as EPSG:4326+5773 and
    EPSG:4326+3855 (`#2365 <https://github.com/OSGeo/PROJ/issues/2365>`_)

  * :cpp:func:`promoteTo3D()`: add a remark with the original CRS identifier (`#2369 <https://github.com/OSGeo/PROJ/issues/2369>`_)

  * Added :c:func:`proj_context_clone` (`#2383 <https://github.com/OSGeo/PROJ/issues/2383>`_)


Bug fixes
---------

* Avoid core dumps when copying contexts in certain scenarios (`#2324 <https://github.com/OSGeo/PROJ/issues/2324>`_)

* :c:func:`proj_trans()`: reset errno before attemptying a retry with a new
  coordinate operation (`#2353 <https://github.com/OSGeo/PROJ/issues/2353>`_)

* PROJJSON schema corrected to allow prime meridians values with
  explicitly stating a unit (degrees assumed) (`#2354 <https://github.com/OSGeo/PROJ/issues/2354>`_)

* Adjust :cpp:func:`createBoundCRSToWGS84IfPossible()` and operation filtering
  (for POSGAR 2007 to WGS84 issues) (`#2357 <https://github.com/OSGeo/PROJ/issues/2357>`_)

* :cpp:func:`createOperations()`: several fixes affecting NAD83 -> NAD83(2011) (`#2364 <https://github.com/OSGeo/PROJ/issues/2364>`_)

* WKT2:2019 import/export: handle DATUM (at top level object) with PRIMEM

* WKT1_ESRI: fix import and export of CompoundCRS (`#2389 <https://github.com/OSGeo/PROJ/issues/2389>`_)


7.1.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*September 1st 2020*


Updates
-------

* Added various Brazilian grids to the database (`#2277 <https://github.com/OSGeo/PROJ/issues/2277>`_)

* Added geoid file for Canary Islands to the database (`#2312 <https://github.com/OSGeo/PROJ/issues/2312>`_)

* Updated EPSG database to version 9.8.15 (`#2310 <https://github.com/OSGeo/PROJ/issues/2310>`_)

Bug fixes
---------

* WKT parser: do not raise warning when parsing a WKT2:2015 TIMECRS
  whose TIMEUNIT is at the CS level, and not inside (`#2281 <https://github.com/OSGeo/PROJ/issues/2281>`_)

* Parse '+proj=something_not_latlong +vunits=' without +geoidgrids as a
  Projected3D CRS and not a compound CRS with a unknown datum (`#2289 <https://github.com/OSGeo/PROJ/issues/2289>`_)

* C API: Avoid crashing due to missing SANITIZE_CTX() in entry points (`#2293 <https://github.com/OSGeo/PROJ/issues/2293>`_)

* CMake build: Check "target_clones" before use (`#2297 <https://github.com/OSGeo/PROJ/issues/2297>`_)

* PROJ string export of +proj=krovak +czech: make sure we export +czech… (`#2301 <https://github.com/OSGeo/PROJ/issues/2301>`_)

* Helmert 2D: do not require a useless +convention= parameter (`#2305 <https://github.com/OSGeo/PROJ/issues/2305>`_)

* Fix a few spelling errors ("vgridshit" vs. "vgridshift") (`#2307 <https://github.com/OSGeo/PROJ/issues/2307>`_)

* Fix ability to identify EPSG:2154 as a candidate for 'RGF93_Lambert_93' (`#2316 <https://github.com/OSGeo/PROJ/issues/2316>`_)

* WKT importer: tune for Oracle WKT and 'Lambert Conformal Conic' (`#2322 <https://github.com/OSGeo/PROJ/issues/2322>`_)

* Revert compiler generated Fused Multiply Addition optimized routines (`#2328 <https://github.com/OSGeo/PROJ/issues/2328>`_)



7.1.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*July 1st 2020*

Updates
-------

+ **New transformations**

  * Add a +proj=defmodel transformation for multi-component time-based deformation models (`#2206 <https://github.com/OSGeo/PROJ/pull/2206>`_):

+ **New projections**

  * Add square conformal projections from libproject (`#2148 <https://github.com/OSGeo/PROJ/issues/2148>`_):

    - :ref:`adams_hemi`

    - :ref:`adams_ws1`

    - :ref:`adams_ws2`

    - :ref:`guyou`

    - :ref:`peirce_q`

  * Adams Square II: map ESRI WKT to PROJ string, and implement iterative
    inverse method (`#2157 <https://github.com/OSGeo/PROJ/issues/2157>`_)

  * Added :ref:`igh_o` projection (`#2226 <https://github.com/OSGeo/PROJ/issues/2226>`_)

  * Add :ref:`wink2` inverse by generic inversion of forward method (`#2243 <https://github.com/OSGeo/PROJ/issues/2243>`_)

+ **Database**

  * Update to EPSG 9.8.12, ESRI 10.8.1 and import scope and remarks for
    conversion (`#2238 <https://github.com/OSGeo/PROJ/issues/2238>`_) (#2267)

  * Map the Behrmann projection to ``cae`` when converting ESRI CRSes (`#1986 <https://github.com/OSGeo/PROJ/issues/1986>`_)

  * Support conversion of Flat_Polar_Quartic projection method (`#1987 <https://github.com/OSGeo/PROJ/issues/1987>`_)

  * Register 4 new Austrian height grids (see https://github.com/OSGeo/PROJ-data/pull/13)
    and handle 'Vertical Offset by Grid Interpolation (BEV AT)' method (`#1989 <https://github.com/OSGeo/PROJ/issues/1989>`_)

  * Add ESRI projection method mappings for Mercator_Variant_A, Mercator_Variant_B
    and Transverse_Cylindrical_Equal_Area and various grid mappings (`#2020 <https://github.com/OSGeo/PROJ/issues/2020>`_) (#2195)

  * Map ESRI Transverse_Mercator_Complex to Transverse Mercator (`#2040 <https://github.com/OSGeo/PROJ/issues/2040>`_)

  * Register grids for New Caledonia (see https://github.com/OSGeo/PROJ-data/pull/16) (`#2051 <https://github.com/OSGeo/PROJ/issues/2051>`_) (#2239)

  * Register NZGD2000 -> ITRF96 transformation for NZGD2000 database (`#2248 <https://github.com/OSGeo/PROJ/issues/2248>`_)

  * Register geoid file for UK added
    (see https://github.com/OSGeo//PROJ-data/pull/25() (`#2250 <https://github.com/OSGeo/PROJ/issues/2250>`_)

  * Register Slovakian geoid transformations with needed code changes (`#2259 <https://github.com/OSGeo/PROJ/issues/2259>`_)

  * Register Spanish SPED2ETV2 grid for ED50->ETRS89 (`#2261 <https://github.com/OSGeo/PROJ/issues/2261>`_)

+ **API**

  * Add API function :c:func:`proj_get_units_from_database()` (`#2065 <https://github.com/OSGeo/PROJ/issues/2065>`_)

  * Add API function :c:func:`proj_get_suggested_operation()` (`#2068 <https://github.com/OSGeo/PROJ/issues/2068>`_)

  * Add API functions :c:func:`proj_degree_input()` and :c:func:`proj_degree_output()` (`#2144 <https://github.com/OSGeo/PROJ/issues/2144>`_)

  * Moved :c:func:`proj_context_get_url_endpoint()` & :c:func:`proj_context_get_user_writable_directory()`
    from ``proj_experimental.h`` to ``proj.h`` (`#2162 <https://github.com/OSGeo/PROJ/issues/2162>`_)

  * :c:func:`createFromUserInput()`: allow compound CRS with the 2 parts given by names,
    e.g. 'WGS 84 + EGM96 height' (`#2126 <https://github.com/OSGeo/PROJ/issues/2126>`_)

  * :c:func:`createOperations()`: when converting CompoundCRS<-->Geographic3DCrs, do not
    use discard change of ellipsoidal height if a Helmert transformation is
    involved (`#2227 <https://github.com/OSGeo/PROJ/issues/2227>`_)

+ **Optimizations**

  * ``tmerc/utm``: add a +algo=auto/evenden_snyder/poder_engsager parameter (`#2030 <https://github.com/OSGeo/PROJ/issues/2030>`_)

  * Extended ``tmerc`` (Poder/Engsager): speed optimizations (`#2036 <https://github.com/OSGeo/PROJ/issues/2036>`_)

  * Approximate ``tmerc`` (Snyder): speed optimizations (`#2039 <https://github.com/OSGeo/PROJ/issues/2039>`_)

  * :c:func:`pj_phi2()`: speed-up computation (and thus inverse ellipsoidal Mercator and LCC) (`#2052 <https://github.com/OSGeo/PROJ/issues/2052>`_)

  * Inverse ``cart``: speed-up computation by 33% (`#2145 <https://github.com/OSGeo/PROJ/issues/2145>`_)

  * Extended ``tmerc``: speed-up forward path by ~5% (`#2147 <https://github.com/OSGeo/PROJ/issues/2147>`_)

+ **Various**

  * Follow PDAL's CMake RPATH strategy (`#2009 <https://github.com/OSGeo/PROJ/issues/2009>`_)

  * WKT import/export: add support for WKT1_ESRI VERTCS syntax (`#2024 <https://github.com/OSGeo/PROJ/issues/2024>`_)

  * :program:`projinfo`: add a ``--hide-ballpark`` option (`#2127 <https://github.com/OSGeo/PROJ/issues/2127>`_)

  * :program:`gie`: implement a strict mode with ``<gie-strict> </gie-strict>`` (`#2168 <https://github.com/OSGeo/PROJ/issues/2168>`_)

  * Allow importing WKT1 COMPD_CS with a VERT_DATUM[Ellipsoid,2002] (`#2229 <https://github.com/OSGeo/PROJ/issues/2229>`_)

  * Add runtime checking that sqlite3 is >= 3.11 (`#2235 <https://github.com/OSGeo/PROJ/issues/2235>`_)


Bug fixes
---------

* :cpp:func:`createOperations()`: do not remove ballpark transformation if there are only grid
  based operations, even if they cover the whole area of use (`#2155 <https://github.com/OSGeo/PROJ/issues/2155>`_)

* :cpp:func:`createFromProjString()`: handle default parameters of '+krovak +type=crs', and
  handle ``+czech`` correctly (`#2200 <https://github.com/OSGeo/PROJ/issues/2200>`_)

* :cpp:func:`ProjectedCRS::identify()`: fix identification of EPSG:3059 (`#2215 <https://github.com/OSGeo/PROJ/issues/2215>`_)

* Database: add a 'WGS84' alias for the EPSG:4326 CRS (`#2218 <https://github.com/OSGeo/PROJ/issues/2218>`_)

* Fixes related to CompoundCRS and BoundCRS (`#2222 <https://github.com/OSGeo/PROJ/issues/2222>`_)

* Avoid 2 warnings about missing database indices (`#2223 <https://github.com/OSGeo/PROJ/issues/2223>`_)

* Make ``projinfo --3d --boundcrs-to-wgs84`` work better (`#2224 <https://github.com/OSGeo/PROJ/issues/2224>`_)

* Many fixes regarding BoundCRS, CompoundCRS, Geographic3D CRS with
  non-metre units (`#2234 <https://github.com/OSGeo/PROJ/issues/2234>`_)

* Fix identification of (one of the) ESRI WKT formulations of EPSG:3035 (`#2240 <https://github.com/OSGeo/PROJ/issues/2240>`_)

* Avoid using deprecated and removed Windows API function with Mingw32 (`#2246 <https://github.com/OSGeo/PROJ/issues/2246>`_)

* :cpp:func:`normalizeForVisualization()`: make it switch axis for EPSG:5482
  (RSRGD2000 / RSPS2000) (`#2256 <https://github.com/OSGeo/PROJ/issues/2256>`_)

* Fix access violation in :c:func:`proj_context_get_database_metadata()` (`#2260 <https://github.com/OSGeo/PROJ/issues/2260>`_)


7.0.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*May 1st 2020*

Updates
-------

* Database: update to EPSG v9.8.9 (`#2141 <https://github.com/OSGeo/PROJ/issues/2141>`_)

Bug fixes
---------

* Make tests independent of proj-datumgrid (`#1995 <https://github.com/OSGeo/PROJ/issues/1995>`_)

* Add missing projection property tables (`#1996 <https://github.com/OSGeo/PROJ/issues/1996>`_)

* Avoid crash when running against SQLite3 binary built with
  ``-DSQLITE_OMIT_AUTOINIT`` (`#1999 <https://github.com/OSGeo/PROJ/issues/1999>`_)

* :c:func:`createOperations`: fix wrong pipeline generation with CRS that has ``+nadgrids=``
  and ``+pm=`` (`#2002 <https://github.com/OSGeo/PROJ/issues/2002>`_)

* Fix bad copy&replace pattern on HEALPix and rHEALPix projection names (`#2007 <https://github.com/OSGeo/PROJ/issues/2007>`_)

* :c:func:`createUnitOfMeasure`: use full double resolution for the conversion
  factor (`#2014 <https://github.com/OSGeo/PROJ/issues/2014>`_)

* Update README with info on PROJ-data (`#2015 <https://github.com/OSGeo/PROJ/issues/2015>`_)

* ``utm/ups``: make sure to set errno to ``PJD_ERR_ELLIPSOID_USE_REQUIRED`` if
  ``+es==0`` (`#2045 <https://github.com/OSGeo/PROJ/issues/2045>`_)

* ``data/Makefile.am``: remove bashism (`#2048 <https://github.com/OSGeo/PROJ/issues/2048>`_)

* :cpp:func:`ProjectedCRS::identify`: tune it to better work with ESRI WKT
  representation of EPSG:2193 (`#2059 <https://github.com/OSGeo/PROJ/issues/2059>`_)

* Fix build with gcc 4.8.5 (`#2066 <https://github.com/OSGeo/PROJ/issues/2066>`_)

* Autotools/pkg-conf: Define datarootdir (`#2069 <https://github.com/OSGeo/PROJ/issues/2069>`_)

* :program:`cs2cs`: don't require ``+to`` for '{source_crs} {target_crs} filename...'
  syntax (`#2081 <https://github.com/OSGeo/PROJ/issues/2081>`_)

* CMake: fix bug with ``find_package(PROJ)`` with macOS (`#2082 <https://github.com/OSGeo/PROJ/issues/2082>`_)

* ESRI WKT import / identification: special case for
  NAD_1983_HARN_StatePlane_Colorado_North_FIPS_0501 with Foot_US unit (`#2088 <https://github.com/OSGeo/PROJ/issues/2088>`_)

* ESRI WKT import / identification: special case for
  NAD_1983_HARN_StatePlane_Colorado_North_FIPS_0501 with Foot_US unit (`#2089 <https://github.com/OSGeo/PROJ/issues/2089>`_)

* EngineeringCRS: when exporting to WKT1_GDAL, output unit and axis (`#2092 <https://github.com/OSGeo/PROJ/issues/2092>`_)

* Use jtsk03-jtsk horizontal grid from CDN (`#2098 <https://github.com/OSGeo/PROJ/issues/2098>`_)

* CMake: prefer to use use PROJ_SOURCE_DIR and PROJ_BINARY_DIR (`#2100 <https://github.com/OSGeo/PROJ/issues/2100>`_)

* Fix wrong grids file name in esri.sql (`#2104 <https://github.com/OSGeo/PROJ/issues/2104>`_)

* Fix identification of projected CRS whose name is close but not strictly
  equal to a ESRI alias (`#2106 <https://github.com/OSGeo/PROJ/issues/2106>`_)

* Fix working of Helmert transform between the horizontal part of
  2 compoundCRS (`#2111 <https://github.com/OSGeo/PROJ/issues/2111>`_)

* Database: fix registration of custom entries of grid_transformation_custom.sql
  for geoid grids (`#2114 <https://github.com/OSGeo/PROJ/issues/2114>`_)

* ESRI_WKT ingestion: make sure to identify to non-deprecated EPSG entry when
  possible (`#2119 <https://github.com/OSGeo/PROJ/issues/2119>`_)

* Make sure that importing a Projected 3D CRS from WKT:2019 keeps the base
  geographic CRS as 3D (`#2125 <https://github.com/OSGeo/PROJ/issues/2125>`_)

* :c:func:`createOperations`: improve results of compoundCRS to compoundCRS case (`#2131 <https://github.com/OSGeo/PROJ/issues/2131>`_)

* hgridshift/vgridshift: defer grid opening when grid has already
  been opened (`#2132 <https://github.com/OSGeo/PROJ/issues/2132>`_)

* Resolve a few shadowed declaration warnings (`#2142 <https://github.com/OSGeo/PROJ/issues/2142>`_)

* ProjectedCRS identification: deal with switched 1st/2nd std parallels for
  LCC_2SP(`#2153 <https://github.com/OSGeo/PROJ/issues/2153>`_)

* Fix Robinson inverse projection (`#2154 <https://github.com/OSGeo/PROJ/issues/2154>`_)

* :c:func:`createOperations()`: do not remove ballpark transformation if there are only
  grid based operations, even if they cover the whole area of use (`#2156 <https://github.com/OSGeo/PROJ/issues/2156>`_)

* :c:func:`createFromCoordinateReferenceSystemCodes`: 'optimization' to avoid using
  C++ exceptions (`#2161 <https://github.com/OSGeo/PROJ/issues/2161>`_)

* Ingestion of WKT1_GDAL: correctly map 'Cylindrical_Equal_Area' (`#2167 <https://github.com/OSGeo/PROJ/issues/2167>`_)

* Add limited support for non-conformant WKT1 LAS COMPD_CS[] (`#2172 <https://github.com/OSGeo/PROJ/issues/2172>`_)

* PROJ4 string import: take into correctly non-metre unit when the string
  looks like the one for WGS 84 / Pseudo Mercator (`#2177 <https://github.com/OSGeo/PROJ/issues/2177>`_)

* ``io.hpp``: avoid dependency to ``proj_json_streaming_writer.hpp`` (`#2184 <https://github.com/OSGeo/PROJ/issues/2184>`_)

* Fix support of WKT1_GDAL with netCDF rotated pole formulation (`#2186 <https://github.com/OSGeo/PROJ/issues/2186>`_)



6.3.2 Release Notes
++++++++++++++++++++++++++++++++++++++++
*May 1st 2020*


Bug fixes
---------

* :c:func:`validateParameters`: fix false-positive warning on
  Equidistant Cylindrical (`#1947 <https://github.com/OSGeo/PROJ/issues/1947>`_)

* :c:func:`proj_create_crs_to_cr`: avoid potential reprojection failures when
  reprojecting area of use to source and target CRS (`#1993 <https://github.com/OSGeo/PROJ/issues/1993>`_)

* :c:func:`createOperations`: fix wrong pipeline generation with CRS that has ``+nadgrids=``
  and ``+pm=`` (`#2003 <https://github.com/OSGeo/PROJ/issues/2003>`_)

* Fix bad copy&replace pattern on HEALPix and rHEALPix projection names (`#2006 <https://github.com/OSGeo/PROJ/issues/2006>`_)

* :c:func:`createUnitOfMeasure`: use full double resolution for the conversion
  factor (`#2013 <https://github.com/OSGeo/PROJ/issues/2013>`_)

* ``data/Makefile.am``: remove bashism (`#2047 <https://github.com/OSGeo/PROJ/issues/2047>`_)

* :cpp:func:``ProjectedCRS::identify``: tune it to better work with ESRI WKT representation
  of EPSG:2193 (`#2058 <https://github.com/OSGeo/PROJ/issues/2058>`_)

* EngineeringCRS: when exporting to WKT1_GDAL, output unit and axis (`#2091 <https://github.com/OSGeo/PROJ/issues/2091>`_)

* Add missing entries in grid_alternatives for Portugal grids coming from
  ESRI entries (`#2103 <https://github.com/OSGeo/PROJ/issues/2103>`_)

* Fix working of Helmert transform between the horizontal part of 2
  compoundCRS (`#2110 <https://github.com/OSGeo/PROJ/issues/2110>`_)

* ESRI_WKT ingestion: make sure to identify to non-deprecated EPSG entry when
  possible (`#2118 <https://github.com/OSGeo/PROJ/issues/2118>`_)

* Make sure that importing a Projected 3D CRS from WKT:2019 keeps the base
  geographic CRS as 3D (`#2124 <https://github.com/OSGeo/PROJ/issues/2124>`_)

* :c:func:`createOperations`: improve results of compoundCRS to compoundCRS case (`#2130 <https://github.com/OSGeo/PROJ/issues/2130>`_)

* PROJ4 string import: take into correctly non-metre unit when the string looks
  like the one for WGS 84 / Pseudo Mercator (`#2178 <https://github.com/OSGeo/PROJ/issues/2178>`_)

* Fix support of WKT1_GDAL with netCDF rotated pole formulation (`#2187 <https://github.com/OSGeo/PROJ/issues/2187>`_)

* ``io.hpp``: avoid dependency to ``proj_json_streaming_writer.hpp`` (`#2188 <https://github.com/OSGeo/PROJ/issues/2188>`_)


7.0.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*March 1st 2020*

The major feature in PROJ 7 is significantly improved handling of gridded
models. This was implemented in :ref:`RFC4`.
The main features of the RFC4 work is that PROJ now implements a new grid format,
Geodetic TIFF grids, for exchanging gridded transformation models. In addition
to the new grid format, PROJ can now also access grids online using a data
store in the cloud.

The grids that was previously available via the proj-datumgrid packages are now
available in two places:

  1. As a single combined data archive including all available resource files
  2. From the cloud via https://cdn.proj.org

In Addition, provided with PROJ is a utility called :program:`projsync` that can be used
download grids from the data store in the cloud.

The use of the new grid format and the data from the cloud requires that
PROJ is build against ``libtiff`` and ``libcurl``. Both are optional dependencies
to PROJ but it is highly encouraged that the software is build against both.


.. warning::

    PROJ 7 will be last major release version that includes the ``proj_api.h``
    header. The functionality in ``proj_api.h`` is deprecated and only supported in
    maintenance mode. It is inferior to the functionality provided by functions
    in the ``proj.h`` header and all projects still relying on ``proj_api.h`` are encouraged
    to migrate to the new API in ``proj.h``. See :ref:`API_migration`.
    for more info on how to migrate from the old to the new API.


Updates
-------

* Added new file access API to ``proj.h`` (`#866 <https://github.com/OSGeo/PROJ/issues/866>`_)

* Updated the name of the most recent version of the WKT2 standard from
  WKT2_2018 to WKT2_2019 to reflect the proper name of the standard (`#1585 <https://github.com/OSGeo/PROJ/issues/1585>`_)

* Improvements in transformations from/to WGS 84 (Gxxxx) realizations and
  vertical <--> geog transormations (`#1608 <https://github.com/OSGeo/PROJ/issues/1608>`_)

* Update to version 1.50 of the geodesic library (`#1629 <https://github.com/OSGeo/PROJ/issues/1629>`_)

* Promote :c:func:`proj_assign_context` to ``proj.h`` from ``proj_experimental.h`` (`#1630 <https://github.com/OSGeo/PROJ/issues/1630>`_)

* Add rotation support to the HEALPix projection (`#1638 <https://github.com/OSGeo/PROJ/issues/1638>`_)

* Add C function :c:func:`proj_crs_create_bound_vertical_crs` (`#1689 <https://github.com/OSGeo/PROJ/issues/1689>`_)

* Use Win32 Unicode APIs and expect all strings to be UTF-8 (`#1765 <https://github.com/OSGeo/PROJ/issues/1765>`_)

* Improved name aliases lookup (`#1827 <https://github.com/OSGeo/PROJ/issues/1827>`_)

* CMake: Employ better use of CTest with the ``BUILD_TESTING`` option (`#1870 <https://github.com/OSGeo/PROJ/issues/1870>`_)

* Grid correction: fix handling grids spanning antimeridian (`#1882 <https://github.com/OSGeo/PROJ/issues/1882>`_)

* Remove legacy CMake target name ``proj`` (`#1883 <https://github.com/OSGeo/PROJ/issues/1883>`_)

* :program:`projinfo` add ``--searchpaths`` switch (`#1892 <https://github.com/OSGeo/PROJ/issues/1892>`_)

* Add :ref:`+proj=set operation<set>` to set component(s) of a coordinate to a fixed
  value (`#1896 <https://github.com/OSGeo/PROJ/issues/1896>`_)

* Add EPSG records for 'Geocentric translation by Grid Interpolation (IGN)'
  (``gr3df97a.txt``) and map them to new :ref:`+proj=xyzgridshift<xyzgridshift>` (`#1897 <https://github.com/OSGeo/PROJ/issues/1897>`_)

* Remove ``null`` grid file as it is now a special hardcoded case in grid
  code (`#1898 <https://github.com/OSGeo/PROJ/issues/1898>`_)

* Add :program:`projsync` utility (`#1903 <https://github.com/OSGeo/PROJ/issues/1903>`_)

* Make ``PROJ`` the CMake project name (`#1910 <https://github.com/OSGeo/PROJ/issues/1910>`_)

* Use relative directory to locate PROJ resource files (`#1921 <https://github.com/OSGeo/PROJ/issues/1921>`_)


Bug fixes
---------

* Horizontal grid shift: fix failures on points slightly outside a
  subgrid (`#209 <https://github.com/OSGeo/PROJ/issues/209>`_)

* Fix ASAN issue with SQLite3VFS class (`#1902 <https://github.com/OSGeo/PROJ/issues/1902>`_)

* tests: force use of bash for ``proj_add_test_script_sh`` (`#1905 <https://github.com/OSGeo/PROJ/issues/1905>`_)


Breaking changes
----------------

* Reject NTV2 files where ``GS_TYPE != SECONDS`` (`#1294 <https://github.com/OSGeo/PROJ/issues/1294>`_)

* On Windows the name of the library is now fixed to ``proj.lib`` instead
  of encoding the version number in the library name (`#1581 <https://github.com/OSGeo/PROJ/issues/1581>`_)

* Require C99 compiler (`#1624 <https://github.com/OSGeo/PROJ/issues/1624>`_)

* Remove deprecated JNI bindings (`#1825 <https://github.com/OSGeo/PROJ/issues/1825>`_)

* Remove -ld option from :program:`proj` and :program:`cs2cs` (`#1844 <https://github.com/OSGeo/PROJ/issues/1844>`_)

* Increase CMake minimum version from 3.5 to 3.9 (`#1907 <https://github.com/OSGeo/PROJ/issues/1907>`_)





6.3.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*February 11th 2020*

Updates
-------

* Update the EPSG database to version 9.8.6

* Database: add mapping for gg10_smv2.mnt and gg10_sbv2.mnt French grids

* Database: add mapping for TOR27CSv1.GSB

Bug fixes
---------

* Fix wrong use of derivingConversionRef() that caused issues with use of
  +init=epsg:XXXX by GDAL (affecting R spatial libraries) or in MapServer

* fix exporting CoordinateSystem to PROJ JSON with ID

* projinfo: use No. abbreviation instead of UTF-8 character
  (`#1828 <https://github.com/OSGeo/PROJ/issues/1828>`_)

* CompoundCRS::identify(): avoid exception when horiz/vertical part is a
  BoundCRS

* createOperations(): fix dealing with projected 3D CRS whose Z units != metre

* WKT1_GDAL export: limit datum name massaging to names matching EPSG
  (`#1835 <https://github.com/OSGeo/PROJ/issues/1835>`_)

* unitconvert with mjd time format: avoid potential integer overflow
  (ossfuzz 20072)

* ProjectedCRS::identify(): fix wrong identification of some ESRI WKT linked
  to units

* Database: add a geoid_like value for proj_method column of grid_alternatives,
  fix related entries and simplify/robustify logic to deal with EPSG
  'Geographic3D to GravityRelatedHeight' methods

* Fix ingestion of +proj=cea with +k_0 (`#1881 <https://github.com/OSGeo/PROJ/issues/1881>`_)

* Fix performance issue, affecting PROJ.4 string generation of EPSG:7842
  (`#1913 <https://github.com/OSGeo/PROJ/issues/1913>`_)

* Fix identification of ESRI-style datum names starting with D\_ but without
  alias (`#1911 <https://github.com/OSGeo/PROJ/issues/1911>`_)

* cart: Avoid discontinuity at poles in the inverse case
  (`#1906 <https://github.com/OSGeo/PROJ/issues/1906>`_)

* Various updates to make regression test suite pass with gcc on i386
  (`#1906 <https://github.com/OSGeo/PROJ/issues/1906>`_)


6.3.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*January 1st 2020*

Updates
-------

* Database: tune accuracy of Canadian NTv1 file w.r.t NTv2 (`#1812 <https://github.com/OSGeo/PROJ/issues/1812>`_)

* Modify verbosity level of some debug/trace messages (`#1811 <https://github.com/OSGeo/PROJ/issues/1811>`_)

* :program:`projinfo`: no longer call createBoundCRSToWGS84IfPossible() for WKT1:GDAL
  (`#1810 <https://github.com/OSGeo/PROJ/issues/1810>`_)

* :c:func:`proj_trans`: add retry logic to select other transformation if the best one
  fails. (`#1809 <https://github.com/OSGeo/PROJ/issues/1809>`_)

* `BoundCRS::identify()`: improvements to discard CRS that aren't relevant
  (`#1802 <https://github.com/OSGeo/PROJ/issues/1802>`_)

* Database: update to IGNF v3.1.0 (`#1785 <https://github.com/OSGeo/PROJ/issues/1785>`_)

* Build: Only export symbols if building DLL (`#1773 <https://github.com/OSGeo/PROJ/issues/1773>`_)

* Database: update ESRI entries with ArcGIS Desktop version 10.8.0 database
  (`#1762 <https://github.com/OSGeo/PROJ/issues/1762>`_)

* :c:func:`createOperations()`: chain operations whose middle CRSs are not identical but
  have the same datum (`#1734 <https://github.com/OSGeo/PROJ/issues/1734>`_)

* import/export PROJJSON: support a interpolation_crs key to geoid_model
  (`#1732 <https://github.com/OSGeo/PROJ/issues/1732>`_)

* Database: update to EPSG v9.8.4 (`#1725 <https://github.com/OSGeo/PROJ/issues/1725>`_)

* Build: require SQLite 3.11 (`#1721 <https://github.com/OSGeo/PROJ/issues/1721>`_)

* Add support for GEOIDMODEL (`#1710 <https://github.com/OSGeo/PROJ/issues/1710>`_)

* Better filtering based on extent and performance improvements (`#1709 <https://github.com/OSGeo/PROJ/issues/1709>`_)

Bug fixes
---------

* Horizontal grid shift: fix issue on iterative inverse computation when
  switching between (sub)grids (`#1797 <https://github.com/OSGeo/PROJ/issues/1797>`_)

* :c:func:`createOperations()`: make filtering out of 'uninteresting' operations less
  aggressive (`#1788 <https://github.com/OSGeo/PROJ/issues/1788>`_)

* Make EPSG:102100 resolve to ESRI:102100 (`#1786 <https://github.com/OSGeo/PROJ/issues/1786>`_)

* ``ob_tran``: restore traditional handling of ``+to_meter`` with :c:func:`pj_transform()` and
  :program:`proj` utility (`#1783 <https://github.com/OSGeo/PROJ/issues/1783>`_)

* CRS identification: use case insensitive comparison for authority name
  (`#1780 <https://github.com/OSGeo/PROJ/issues/1780>`_)

* :c:func:`normalizeForVisualization()` and other methods applying on a ProjectedCRS: do
  not mess the derivingConversion object of the original object (`#1746 <https://github.com/OSGeo/PROJ/issues/1746>`_)

* :c:func:`createOperations()`: fix transformation computation from/to a CRS with
  ``+geoidgrids`` and ``+vunits`` != m (`#1731 <https://github.com/OSGeo/PROJ/issues/1731>`_)

* Fix :c:func:`proj_assign_context()`/:c:func:`pj_set_ctx()` with pipelines and alternative coord
  operations (`#1726 <https://github.com/OSGeo/PROJ/issues/1726>`_)

* Database: add an auxiliary concatenated_operation_step table to allow
  arbitrary number of steps (`#1696 <https://github.com/OSGeo/PROJ/issues/1696>`_)

* Fix errors running gie-based tests in Debug mode on Window (`#1688 <https://github.com/OSGeo/PROJ/issues/1688>`_)

6.2.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*November 1st 2019*

Updates
-------

* Update the EPSG database to version 9.8.2

Bug fixes
----------

* Fixed erroneous spelling of "Potsdam" (`#1573 <https://github.com/OSGeo/PROJ/issues/1573>`_)

* Calculate y-coordinate correctly in :ref:`bertin1953` in all cases (`#1579 <https://github.com/OSGeo/PROJ/issues/1579>`_)

* :c:func:`proj_create_crs_to_crs_from_pj()`: make the PJ* arguments const PJ* (`#1583 <https://github.com/OSGeo/PROJ/issues/1583>`_)

* `PROJStringParser::createFromPROJString()`: avoid potential infinite
  recursion (`#1574 <https://github.com/OSGeo/PROJ/issues/1574>`_)

* Avoid core dump when setting ``ctx==NULL`` in functions
  :c:func:`proj_coordoperation_is_instantiable` and
  :c:func:`proj_coordoperation_has_ballpark_transformation` (`#1590 <https://github.com/OSGeo/PROJ/issues/1590>`_)

* :c:func:`createOperations()`: fix conversion from/to PROJ.4 CRS strings with
  non-ISO-kosher options and ``+towgs84``/``+nadgrids`` (`#1602 <https://github.com/OSGeo/PROJ/issues/1602>`_)

* :c:func:`proj_trans_generic()`: properly set coordinate time to ``HUGE_VAL``
  when no value is passed to the function (`#1604 <https://github.com/OSGeo/PROJ/issues/1604>`_)

* Fix support for ``+proj=ob_tran +o_proj=lonlat/latlong/latlon`` instead of only
  only allowing ``+o_proj=longlat`` (`#1601 <https://github.com/OSGeo/PROJ/issues/1601>`_)

* Improve backwards compatibility of vertical transforms (`#1613 <https://github.com/OSGeo/PROJ/issues/1613>`_)

* Improve emulation of deprecated ``+init`` style initialization (`#1614 <https://github.com/OSGeo/PROJ/issues/1614>`_)

* :program:`cs2cs`: autopromote CRS to 3D when there's a mix of 2D and 3D (`#1563 <https://github.com/OSGeo/PROJ/issues/1563>`_)

* Avoid divisions by zero in odd situations (`#1620 <https://github.com/OSGeo/PROJ/issues/1620>`_)

* Avoid compile error on Solaris (`#1639 <https://github.com/OSGeo/PROJ/issues/1639>`_)

* :c:func:`proj_create_crs_to_crs()`: fix when there are only transformations with
  ballpark steps (`#1643 <https://github.com/OSGeo/PROJ/issues/1643>`_)

* PROJ string CRS ingester: recognize more unit-less parameters, and general
  handling of ``+key=string_value`` parameters (`#1645 <https://github.com/OSGeo/PROJ/issues/1645>`_)

* Only call pkg-config in configure when necessary (`#1652 <https://github.com/OSGeo/PROJ/issues/1652>`_)

* :ref:`aeqd`: for spherical forward path, go to higher precision ellipsoidal
  case when the point coordinates are super close to the origin (`#1654 <https://github.com/OSGeo/PROJ/issues/1654>`_)

* :c:func:`proj_create_crs_to_crs()`: remove elimination of Ballpark operations
  that caused transformation failures in some cases (`#1665 <https://github.com/OSGeo/PROJ/issues/1665>`_)

* :c:func:`createOperations()`: allow transforming from a compoundCRS of a bound
  verticalCRS to a 2D CRS (`#1667 <https://github.com/OSGeo/PROJ/issues/1667>`_)

* Avoid segfaults in case of out-of-memory situations (`#1679 <https://github.com/OSGeo/PROJ/issues/1679>`_)

* :c:func:`createOperations()`: fix double vertical unit conversion from CompoundCRS
  to other CRS when the horizontal part of the projected CRS uses non-metre
  unit (#1683)(`#1683 <https://github.com/OSGeo/PROJ/issues/1683>`_)

* :c:func:`importFromWkt()`: fix axis orientation for non-standard ESRI WKT (`#1690 <https://github.com/OSGeo/PROJ/issues/1690>`_)



6.2.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*September 1st 2019*

Updates
-------

 * Introduced :ref:`PROJJSON`, a JSON encoding of WKT2 (`#1547 <https://github.com/OSGeo/PROJ/issues/1547>`_)
 * Support CRS instantiation of OGC URN's (`#1505 <https://github.com/OSGeo/PROJ/issues/1505>`_)

 * Expose scope and remarks of database objects (`#1537 <https://github.com/OSGeo/PROJ/issues/1537>`_)

 * EPSG Database updated to version 9.7.0 (`#1558 <https://github.com/OSGeo/PROJ/issues/1558>`_)

 * Added C API function proj_grid_get_info_from_database() (`#1494 <https://github.com/OSGeo/PROJ/issues/1494>`_)

 * Added C API function
   proj_operation_factory_context_set_discard_superseded() (`#1534 <https://github.com/OSGeo/PROJ/issues/1534>`_)

 * Added C API function proj_context_set_autoclose_database() (`#1566 <https://github.com/OSGeo/PROJ/issues/1566>`_)

 * Added C API function proj_create_crs_to_crs_from_pj() (`#1567 <https://github.com/OSGeo/PROJ/issues/1567>`_)

 * Added C API function proj_cleanup() (`#1569 <https://github.com/OSGeo/PROJ/issues/1569>`_)



Bug Fixes
---------

 * Fixed build failure on Solaris systems (`#1554 <https://github.com/OSGeo/PROJ/issues/1554>`_)


6.1.1 Release Notes
++++++++++++++++++++++++++++++++++++++++
*July 1st 2019*

Updates
-------

* Update EPSG registry to version 9.6.3 (`#1485 <https://github.com/OSGeo/PROJ/issues/1485>`_)

Bug Fixes
---------

* Take the passed authority into account when identifying
  objects (`#1466 <https://github.com/OSGeo/PROJ/issues/1466>`_)

* Avoid exception when transforming from NAD83 to projected
  CRS using NAD83(2011) (`#1477 <https://github.com/OSGeo/PROJ/issues/1477>`_)

* Avoid off-by-one reading of name argument if name of resource
  file has length 1 (`#11489 <https://github.com/OSGeo/PROJ/issues/1489>`_)

* Do not include :envvar:`PROJ_LIB` in ``proj_info().searchpath`` when context
  search path is set (`#1498 <https://github.com/OSGeo/PROJ/issues/1498>`_)

* Use correct delimiter for the current platform when parsing
  PROJ_LIB (`#1497 <https://github.com/OSGeo/PROJ/issues/1497>`_)

* Do not confuse 'ID74' CRS with WKT2 ID[] node (`#1506 <https://github.com/OSGeo/PROJ/issues/1506>`_)

* WKT1 importer: do case insensitive comparison for axis
  direction (`#1509 <https://github.com/OSGeo/PROJ/issues/1509>`_)

* Avoid compile errors on GCC 4.9.3 (`#1512 <https://github.com/OSGeo/PROJ/issues/1512>`_)

* Make sure that pipelines including ``+proj=ob_tran`` can be
  created (`#1526 <https://github.com/OSGeo/PROJ/issues/1526>`_)


6.1.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*May 15th 2019*

Updates
-------

* Include custom ellipsoid definitions from QGIS (`#1137 <https://github.com/OSGeo/proj.4/issues/1337>`_)

* Add ``-k ellipsoid`` option to projinfo (`#1338 <https://github.com/OSGeo/proj.4/issues/1338>`_)

* Make cs2cs support 4D coordinates (`#1355 <https://github.com/OSGeo/proj.4/issues/1355>`_)

* WKT2 parser: update to OGC 18-010r6 (`#1360 <https://github.com/OSGeo/proj.4/issues/1360>`_ `#1366 <https://github.com/OSGeo/proj.4/issues/1366>`_))

* Update internal version of googletest to v1.8.1 (`#1361 <https://github.com/OSGeo/proj.4/issues/1361>`_)

* Database update: EPSG v9.6.2 (`#1462 <https://github.com/OSGeo/proj.4/issues/1462>`_), IGNF v3.0.3, ESRI 10.7.0
  and add operation_version column (`#1368 <https://github.com/OSGeo/proj.4/issues/1368>`_)

* Add :c:func:`proj_normalize_for_visualization` that attempts to apply axis
  ordering as used by most GIS applications and PROJ <6 (`#1387 <https://github.com/OSGeo/proj.4/issues/1387>`_)

* Added noop operation (`#1391 <https://github.com/OSGeo/proj.4/issues/1391>`_)

* Paths set by user take priority over :envvar:`PROJ_LIB` for search paths (`#1398 <https://github.com/OSGeo/proj.4/issues/1398>`_)

* Reduced database size (`#1438 <https://github.com/OSGeo/proj.4/issues/1438>`_)

* add support for compoundCRS and concatenatedOperation named from
  their components (`#1441 <https://github.com/OSGeo/proj.4/issues/1441>`_)

Bug fixes
---------

* Have :program:`gie` return non-zero code when file can't be opened (`#1312 <https://github.com/OSGeo/proj.4/issues/1312>`_)

* CMake cross-compilation fix (`#1316 <https://github.com/OSGeo/proj.4/issues/1316>`_)

* Use 1st eccentricity instead of 2nd eccentricity in Molodensky (`#1324 <https://github.com/OSGeo/proj.4/issues/1324>`_)

* Make sure to include grids when doing Geocentric to CompoundCRS with
  nadgrids+geoidgrids transformations (`#1326 <https://github.com/OSGeo/proj.4/issues/1326>`_)

* Handle coordinates outside of bbox better (`#1333 <https://github.com/OSGeo/proj.4/issues/1333>`_)

* Enable system error messages in command line automatically in builds (`#1336 <https://github.com/OSGeo/proj.4/issues/1336>`_)

* Make sure to install projinfo man page with CMake (`#1347 <https://github.com/OSGeo/proj.4/issues/1347>`_)

* Add data dir to pkg-config file proj.pc (`#1348 <https://github.com/OSGeo/proj.4/issues/1348>`_)

* Fix GCC 9 warning about useless ``std::move()`` (`#1352 <https://github.com/OSGeo/proj.4/issues/1352>`_)

* Grid related fixes (`#1369 <https://github.com/OSGeo/proj.4/issues/1369>`_)

* Make sure that ISO19111 C++ code sets pj_errno on errors (`#1405 <https://github.com/OSGeo/proj.4/issues/1405>`_)

* vgridshift: handle longitude wrap-around for grids with 360deg
  longitude extent (`#1429 <https://github.com/OSGeo/proj.4/issues/1429>`_)

* :program:`proj`/:program:`cs2cs`: validate value of ``-f`` parameter to avoid potential crashes (`#1434 <https://github.com/OSGeo/proj.4/issues/1434>`_)

* Many division by zero and similar bug fixes found by OSS Fuzz.


6.0.0 Release Notes
++++++++++++++++++++++++++++++++++++++++
*March 1st 2019*


PROJ 6 has undergone extensive changes to increase its functional scope from a
cartographic projection engine with so-called "early-binding" geodetic datum
transformation capabilities to a more complete library supporting coordinate
transformations and coordinate reference systems.

As a foundation for other enhancements, PROJ now includes a C++ implementation
of the modelisation propopsed by the ISO-19111:2019 standard / OGC Abstract
Specification Topic 2: "Referencing By Coordinates", for geodetic reference
frames (datums), coordinate reference systems and coordinate operations.
Construction and query of those geodetic objects is available through a new C++
API, and also accessible for the most part from bindings in the C API.

Those geodetic objects can be imported and exported from and into the OGC
Well-Known Text format (WKT) in its different variants: ESRI WKT, GDAL WKT 1,
WKT2:2015 (ISO 19162:2015) and WKT2:2018 (ISO 19162:2018). Import and export of
CRS objects from and into PROJ strings is also supported. This functionality
was previously available in the GDAL software library (except WKT2 support
which is a new feature), and is now an integral part of PROJ.

A unified database of geodetic objects, coordinate reference systems and their
metadata, and coordinate operations between those CRS is now available in a
SQLite3 database file, proj.db. This includes definitions imported from the
IOGP EPSG dataset (v9.6.0 release), the IGNF (French national mapping agency)
geodetic registry and the ESRI projection engine database. PROJ is now the
reference software in the "OSGeo C stack" for this CRS and coordinate operation
database, whereas previously this functionality was spread over PROJ, GDAL and
libgeotiff, and used CSV or other adhoc text-based formats.

Late-binding coordinate operation capabilities, that takes  metadata such as
area of use and accuracy into account, has been added. This can avoid in a
number of situations the past requirement of using WGS84 as a pivot system,
which could cause unneeded accuracy loss, or was not doable at all sometimes
when transformation to WGS84 was not available. Those late-binding capabilities
are now used by the proj_create_crs_to_crs() function and the cs2cs utility.

A new command line utility, projinfo, has been added to query information about
a geodetic object of the database, import and export geodetic objects from/into
WKT and PROJ strings, and display coordinate operations available between two
CRSs.

UPDATES
-------

* Removed projects.h as a public interface (`#835 <https://github.com/OSGeo/proj.4/issues/835>`_)

* Deprecated the proj_api.h interface. The header file is still available
  but will be removed with the next major version release of PROJ. It is
  now required to define :c:macro:`ACCEPT_USE_OF_DEPRECATED_PROJ_API_H`
  before the interface can be used (`#836 <https://github.com/OSGeo/proj.4/issues/836>`_)

* Removed support for the nmake build system (`#838 <https://github.com/OSGeo/proj.4/issues/838>`_)

* Removed support for the ``proj_def.dat`` defaults file (`#201 <https://github.com/OSGeo/proj.4/issues/201>`_)

* C++11 required for building PROJ (`#1203 <https://github.com/OSGeo/proj.4/issues/1203>`_)

* Added build dependency on SQLite 3.7 (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Added :program:`projinfo` command line application (`#1189 <https://github.com/OSGeo/proj.4/issues/1189>`_)

* Added many functions to ``proj.h`` for handling ISO19111 functionality (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Added C++ API exposing ISO19111 functionality (`#1175 <https://github.com/OSGeo/proj.4/issues/1175>`_)

* Updated :program:`cs2cs` to use late-binding features (`#1182 <https://github.com/OSGeo/proj.4/issues/1182>`_)

* Removed the ``nad2bin`` application. Now available in the
  `proj-datumgrid <https://github.com/OSGeo/proj-datumgrid>`_
  git repository (`#1236 <https://github.com/OSGeo/proj.4/issues/1236>`_)

* Removed support for Chebyshev polynomials in :program:`proj`
  (`#1226 <https://github.com/OSGeo/proj.4/issues/1226>`_)

* Removed :c:func:`proj_geocentric_latitude` from `proj.h` API
  (`#1170 <https://github.com/OSGeo/proj.4/issues/1170>`_)

* Changed behavior of :program:`proj`: Now only allow initialization of
  projections (`#1162 <https://github.com/OSGeo/proj.4/issues/1162>`_)

* Changed behavior of :ref:`tmerc <tmerc>`: Now defaults to the Extended
  Transverse Mercator algorithm (``etmerc``). Old implementation available
  by adding ``+approx``
  (`#404 <https://github.com/OSGeo/proj.4/issues/404>`_)

* Changed behavior: Default ellipsoid now set to GRS80 (was WGS84) (`#1210 <https://github.com/OSGeo/proj.4/issues/1210>`_)

* Allow multiple directories in :envvar:`PROJ_LIB` environment variable (`#1281 <https://github.com/OSGeo/proj.4/issues/1281>`_)

* Added :ref:`Lambert Conic Conformal (2SP Michigan) <lcc>` projection (`#1142 <https://github.com/OSGeo/proj.4/issues/1142>`_)

* Added :ref:`Bertin1953 <bertin1953>` projection (`#1133 <https://github.com/OSGeo/proj.4/issues/1133>`_)

* Added :ref:`Tobler-Mercator <tobmerc>` projection (`#1153 <https://github.com/OSGeo/proj.4/issues/1153>`_)

* Added :ref:`Molodensky-Badekas <molobadekas>` transform (`#1160 <https://github.com/OSGeo/proj.4/issues/1160>`_)

* Added :ref:`push <push>` and :ref:`pop <pop>` coordinate operations (`#1250 <https://github.com/OSGeo/proj.4/issues/1250>`_)

* Removed ``+t_obs`` parameter from helmert and deformation (`#1264 <https://github.com/OSGeo/proj.4/issues/1264>`_)

* Added :option:`+dt` parameter to deformation as replacement for
  removed ``+t_obs`` (`#1264 <https://github.com/OSGeo/proj.4/issues/1264>`_)

BUG FIXES
---------

* Read :option:`+towgs84` values correctly on locales not using dot as comma separator (`#1136 <https://github.com/OSGeo/proj.4/issues/1136>`_)

* Fixed file offset for reading of shift values in NTv1 files (`#1144 <https://github.com/OSGeo/proj.4/issues/1144>`_)

* Avoid problems with :c:macro:`PTHREAD_MUTEX_RECURSIVE` when using CMake (`#1158 <https://github.com/OSGeo/proj.4/issues/1158>`_)

* Avoid raising errors when setting ellipsoid flattening to zero (`#1191 <https://github.com/OSGeo/proj.4/issues/1191>`_)

* Fixed lower square calculations in :ref:`rHealpix <rhealpix>` projection (`#1206 <https://github.com/OSGeo/proj.4/issues/1206>`_)

* Allow :ref:`Molodensky <molodensky>` transform parameters to be zero (`#1194 <https://github.com/OSGeo/proj.4/issues/1194>`_)

* Fixed wrong parameter in ``ITRF2000`` init file (`#1240 <https://github.com/OSGeo/proj.4/issues/1240>`_)

* Fixed use of grid paths including spaces (`#1152 <https://github.com/OSGeo/proj.4/issues/1152>`_)

* :ref:`Robinson <robin>`: fix wrong values for forward path for latitudes >= 87.5,
  and fix inaccurate inverse method (`#1172 <https://github.com/OSGeo/proj.4/issues/1172>`_)


PROJ 5.2.0
++++++++++++++++++++++++++++++++++++++++
*September 15th 2018*

UPDATES
-------

* Added support for deg, rad and grad in unitconvert (`#1054 <https://github.com/OSGeo/proj.4/issues/1054>`_)

* Assume :option:`+t_epoch` as time input when not otherwise specified (`#1065 <https://github.com/OSGeo/proj.4/issues/1065>`_)

* Added inverse Lagrange projection (`#1058 <https://github.com/OSGeo/proj.4/issues/1058>`_)

* Added :option:`+multiplier` option to vgridshift (`#1072 <https://github.com/OSGeo/proj.4/issues/1072>`_)

* Added Equal Earth projection (`#1085 <https://github.com/OSGeo/proj.4/issues/1085>`_)

* Added "require_grid" option to gie (`#1088 <https://github.com/OSGeo/proj.4/issues/1088>`_)

* Replace :option:`+transpose` option of Helmert transform with :option:`+convention`.
  From now on the convention used should be explicitly written. An
  error will be returned when using the +transpose option (`#1091 <https://github.com/OSGeo/proj.4/issues/1091>`_)

* Improved numerical precision of inverse spherical Mercator
  projection (`#1105 <https://github.com/OSGeo/proj.4/issues/1105>`_)

* :program:`cct` will now forward text after coordinate input to output
  stream (`#1111 <https://github.com/OSGeo/proj.4/issues/1111>`_)


BUG FIXES
------------

* Do not pivot over WGS84 when doing cs2cs-emulation with geocent (`#1026 <https://github.com/OSGeo/proj.4/issues/1026>`_)

* Do not scan past the end of the read data in :c:func:`pj_ctx_fgets` (`#1042 <https://github.com/OSGeo/proj.4/issues/1042>`_)

* Make sure :c:func:`proj_errno_string()` is available in DLL (`#1050 <https://github.com/OSGeo/proj.4/issues/1050>`_)

* Respect `+to_meter` setting when doing cs2cs-emulation (`#1053 <https://github.com/OSGeo/proj.4/issues/1053>`_)

* Fixed unit conversion factors for :program:`geod` (`#1075 <https://github.com/OSGeo/proj.4/issues/1075>`_)

* Fixed test failures related to GCC 8 (`#1084 <https://github.com/OSGeo/proj.4/issues/1084>`_)

* Improved handling of `+geoc` flag (`#1093 <https://github.com/OSGeo/proj.4/issues/1093>`_)

* Calculate correct projection factors for Webmercator (`#1095 <https://github.com/OSGeo/proj.4/issues/1095>`_)

* :program:`cs2cs` now always outputs degrees when transformed coordinates are
  in angular units (`#1112 <https://github.com/OSGeo/proj.4/issues/1112>`_)


PROJ 5.1.0
++++++++++++++++++++++++++++++++++++++++
*June 1st 2018*


UPDATES
-------

* Function :c:func:`proj_errno_string()` added to ``proj.h`` API (`#847 <https://github.com/OSGeo/proj.4/issues/847>`_)

* Validate units between pipeline steps and ensure transformation
  sanity (`#906 <https://github.com/OSGeo/proj.4/issues/906>`_)

* Print help when calling :program:`cct` and :program:`gie` without arguments (`#907 <https://github.com/OSGeo/proj.4/issues/907>`_)

* `CITATION` file added to source distribution (`#914 <https://github.com/OSGeo/proj.4/issues/914>`_)

* Webmercator operation added (`#925 <https://github.com/OSGeo/proj.4/issues/925>`_)

* Enhanced numerical precision of forward spherical Mercator near
  the Equator (`#928 <https://github.com/OSGeo/proj.4/issues/928>`_)

* Added ``--skip-lines`` option to :program:`cct` (`#923 <https://github.com/OSGeo/proj.4/issues/923>`_)

* Consistently return ``NaN`` values on ``NaN`` input (`#949 <https://github.com/OSGeo/proj.4/issues/949>`_)

* Removed unused ``src/org_proj4_Projections.h`` file (`#956 <https://github.com/OSGeo/proj.4/issues/956>`_)

* Java Native Interface bindings updated (`#957 <https://github.com/OSGeo/proj.4/issues/957>`_, `#969 <https://github.com/OSGeo/proj.4/issues/969>`_)

* Horizontal and vertical gridshift operations extended to
  the temporal domain (`#1015 <https://github.com/OSGeo/proj.4/issues/1015>`_)


BUG FIXES
------------

* Handle ``NaN`` float cast overflow in ``PJ_robin.c`` and ``nad_intr.c`` (`#887 <https://github.com/OSGeo/proj.4/issues/887>`_)

* Avoid overflow when Horner order is unreasonably large (`#893 <https://github.com/OSGeo/proj.4/issues/893>`_)

* Avoid unwanted NaN conversions in etmerc (`#899 <https://github.com/OSGeo/proj.4/issues/899>`_)

* Avoid memory failure in :program:`gie` when not specifying x,y,z in gie files (`#902 <https://github.com/OSGeo/proj.4/issues/902>`_)

* Avoid memory failure when :option:`+sweep` is initialized incorrectly in geos (`#908 <https://github.com/OSGeo/proj.4/issues/908>`_)

* Return ``HUGE_VAL`` on erroneous input in ortho (`#912 <https://github.com/OSGeo/proj.4/issues/912>`_)

* Handle commented lines correctly in cct (`#933 <https://github.com/OSGeo/proj.4/issues/933>`_)

* Avoid segmentation fault when transformation coordinates outside grid
  area in deformation (`#934 <https://github.com/OSGeo/proj.4/issues/934>`_)

* Avoid doing false easting/northing adjustments on cartesian
  coordinates (`#936 <https://github.com/OSGeo/proj.4/issues/936>`_)

* Thread-safe creation of proj mutex (`#954 <https://github.com/OSGeo/proj.4/issues/954>`_)

* Avoid errors when setting up geos with +lat_0!=0 (`#986 <https://github.com/OSGeo/proj.4/issues/986>`_)

* Reset errno when running :program:`proj` in verbose mode (`#988 <https://github.com/OSGeo/proj.4/issues/988>`_)

* Do not interpolate node values at nodata value in vertical
  grid shifts (`#1004 <https://github.com/OSGeo/proj.4/issues/1004>`_)

* Restrict Horner degrees to positive integer values to avoid
  memory allocation issues (`#1005 <https://github.com/OSGeo/proj.4/issues/1005>`_)



PROJ 5.0.1
++++++++++++++++++++++++++++++++++++++++
*March 1st 2018*

Bug fixes
---------

* Handle ellipsoid change correctly in pipelines when ``+towgs84=0,0,0`` is set (`#881 <https://github.com/OSGeo/proj.4/issues/881>`_)


* Handle the case where nad_ctable2_init returns NULL (`#883 <https://github.com/OSGeo/proj.4/issues/883>`_)


* Avoid shadowed declaration errors with old gcc (`#880 <https://github.com/OSGeo/proj.4/issues/880>`_)


* Expand ``+datum`` properly in pipelines (`#872 <https://github.com/OSGeo/proj.4/issues/872>`_)


* Fail gracefully when incorrect headers are encountered in grid files (`#875 <https://github.com/OSGeo/proj.4/issues/875>`_)


* Improve roundtrip stability in pipelines using ``+towgs84`` (`#871 <https://github.com/OSGeo/proj.4/issues/871>`_)


* Fixed typo in gie error codes (`#861 <https://github.com/OSGeo/proj.4/issues/861>`_)


* Numerical stability fixes to the geodesic package (`#826 <https://github.com/OSGeo/proj.4/issues/826>`_ & `#843 <https://github.com/OSGeo/proj.4/issues/843>`_)


* Make sure that transient errors are returned correctly (`#857 <https://github.com/OSGeo/proj.4/issues/857>`_)


* Make sure that locally installed header files are not used when building PROJ (`#849 <https://github.com/OSGeo/proj.4/issues/849>`_)


* Fix inconsistent parameter names in ``proj.h``/``proj_4D_api.c`` (`#842 <https://github.com/OSGeo/proj.4/issues/842>`_)


* Make sure ``+vunits`` is applied (`#833 <https://github.com/OSGeo/proj.4/issues/833>`_)


* Fix incorrect Web Mercator transformations (`#834 <https://github.com/OSGeo/proj.4/issues/834>`_)



PROJ 5.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*February 1st 2018*

This version of PROJ introduces some significant extensions and
improvements to (primarily) the geodetic functionality of the system.

The main driver for introducing the new features is the emergence of
dynamic reference frames, the increasing use of high accuracy GNSS,
and the related growing demand for accurate coordinate
transformations.  While older versions of PROJ included some geodetic
functionality, the new framework lays the foundation for turning PROJ
into a generic geospatial coordinate transformation engine.

The core of the library is still the well established projection code.
The new functionality is primarily exposed in a new programming
interface and a new command line utility, :ref:`cct<cct>`
(for "Coordinate Conversion and Transformation").  The old programming interface is
still available and can - to some extent - use the new geodetic
transformation features.

The internal architecture has also seen many changes and much
improvement.  So far, these improvements respect the existing
programming interface. But the process has revealed a need to simplify
and reduce the code base, in order to support sustained active
development.


**Therefore we have scheduled regular releases over the coming years**
**which will gradually remove the old programming interface.**

**This will cause breaking changes with the next two major version**
**releases, which will affect all projects that depend on PROJ**
**(cf. section "deprecations" below).**

The decision to break the existing API has not been easy, but has
ultimately been deemed necessary to ensure the long term survival of
the project. Not only by improving the maintainability immensely, but
also by extending the potential user (and hence developer) community.

The end goal is to deliver a generic coordinate transformation
software package with a clean and concise code base appealing to
both users and developers.


Versioning and naming
---------------------

For the first time in more than 25 years the major version number of
the software is changed. The decision to do this is based on the many
new features and new API. While backwards compatibility remains -
except in a few rare corner cases - the addition of a new and improved
programming interface warrants a new major release.

The new major version number unfortunately leaves the project in a bit
of a conundrum regarding the name. For the majority of the life-time
of the product it has been known as PROJ.4, but since we have now
reached version 5 the name is no longer aligned with the version
number.

Hence we have decided to decouple the name from the version number and
from this version and onwards the product will simply be called PROJ.

In recognition of the history of the software we are keeping PROJ.4 as
the *name of the organizing project*. The same project team also
produces the datum-grid package.

In summary:

* The PROJ.4 project provides the product PROJ, which is now at
  version 5.0.0.

* The foundational component of PROJ is the library libproj.

* Other PROJ components include the application proj, which provides
  a command line interface to libproj.

* The PROJ.4 project also distributes the datum-grid package,
  which at the time of writing is at version 1.6.0.

Updates
-------

* Introduced new API in ``proj.h``.

    - The new API is orthogonal to the existing ``proj_api.h`` API and the internally used ``projects.h`` API.

    - The new API adds the ability to transform spatiotemporal (4D) coordinates.

    - Functions in the new API use the ``proj_`` namespace.

    - Data types in the new API use the ``PJ_`` namespace.

*  Introduced the concept of "transformation pipelines" that makes possible to do complex geodetic transformations of coordinates by daisy chaining simple coordinate operations.

* Introduced :ref:`cct`, the Coordinate Conversion and Transformation application.

* Introduced :ref:`gie`, the Geospatial Integrity Investigation Environment.

  - Selftest invoked by ``-C`` flag in :ref:`proj` has been removed
  - Ported approx. 1300 built-in selftests to :ref:`gie` format
  - Ported approx. 1000 tests from the gigs test framework
  - Added approx. 200 new tests

* Adopted terminology from the OGC/ISO-19100 geospatial standards series. Key definitions are:

    - At the most generic level, a *coordinate operation* is a change of coordinates, based on a one-to-one relationship, from one coordinate reference system to another.

    - A *transformation* is a coordinate operation in which the two coordinate reference systems are based on different datums, e.g. a change from a global reference frame to a regional frame.

    - A *conversion* is a coordinate operation in which both coordinate reference systems are based on the same datum, e.g. change of units of coordinates.

    - A *projection* is a coordinate conversion from an ellipsoidal coordinate system to a plane. Although projections are simply conversions according to the standard, they are treated as separate entities in PROJ as they make up the vast majority of operations in the library.

* New operations

    - :ref:`The pipeline operator<pipeline>` (``pipeline``)

    - Transformations
        + :ref:`Helmert transform<helmert>` (``helmert``)
        + Horner real and complex polynomial evaluation (``horner``)
        + :ref:`Horizontal gridshift<hgridshift>` (``hgridshift``)
        + :ref:`Vertical gridshift<vgridshift>` (``vgridshift``)
        + :ref:`Molodensky transform<molodensky>` (``molodensky``)
        + :ref:`Kinematic gridshift with deformation model<deformation>` (``deformation``)

    - Conversions
        + :ref:`Unit conversion<unitconvert>` (``unitconvert``)
        + :ref:`Axis swap<axisswap>` (``axisswap``)

    - Projections
        + :ref:`Central Conic projection<ccon>` (``ccon``)

* Significant documentation updates, including

    - Overhaul of the structure of the documentation
    - A better introduction to the use of PROJ
    - :ref:`A complete reference to the new API<reference>`
    - a complete rewrite of the section on geodesic calculations
    - Figures for all projections

* New "free format" option for operation definitions, which permits separating tokens by whitespace when specifying key/value- pairs, e.g. ``proj = merc lat_0 = 45``.

* Added metadata to init-files that can be read with the :c:func:`proj_init_info` function in the new ``proj.h`` API.

* Added ITRF2000, ITRF2008 and ITRF2014 init-files with ITRF transformation parameters, including plate motion model parameters.

* Added ellipsoid parameters for GSK2011, PZ90 and "danish". The latter is similar to the already supported andrae ellipsoid, but has a slightly different semimajor axis.

* Added Copenhagen prime meridian.

* Updated EPSG database to version 9.2.0.

* Geodesic library updated to version 1.49.2-c.

* Support for analytical partial derivatives has been removed.

* Improved performance in Winkel Tripel and Aitoff.

* Introduced ``pj_has_inverse()`` function to ``proj_api.h``. Checks if an operation has an inverse. Use this instead of checking whether ``P->inv`` exists, since that can no longer be relied on.

* ABI version number updated to 13:0:0.

* Removed support for Windows CE.

* Removed the VB6 COM interface.

Bug fixes
---------

* Fixed incorrect convergence calculation in Lambert Conformal Conic. (`#16 <https://github.com/OSGeo/proj.4/issues/16>`_)


* Handle ellipsoid parameters correctly when using ``+nadgrids=@null``. (`#22 <https://github.com/OSGeo/proj.4/issues/22>`_)


* Return correct latitude when using negative northings in Transverse Mercator. (`#138 <https://github.com/OSGeo/proj.4/issues/138>`_)


* Return correct result at origin in inverse Mod. Stereographic of Alaska. (`#161 <https://github.com/OSGeo/proj.4/issues/161>`_)


* Return correct result at origin in inverse Mod. Stereographic of 48 U.S. (`#162 <https://github.com/OSGeo/proj.4/issues/162>`_)


* Return correct result at origin in inverse Mod. Stereographic of 50 U.S. (`#163 <https://github.com/OSGeo/proj.4/issues/163>`_)


* Return correct result at origin in inverse Lee Oblated Stereographic. (`#164 <https://github.com/OSGeo/proj.4/issues/164>`_)


* Return correct result at origin in inverse Miller Oblated Stereographic. (`#165 <https://github.com/OSGeo/proj.4/issues/165>`_)


* Fixed scaling and wrap-around issues in Oblique Cylindrical Equal Area. (`#166 <https://github.com/OSGeo/proj.4/issues/166>`_)


* Corrected a coefficient error in inverse Transverse Mercator. (`#174 <https://github.com/OSGeo/proj.4/issues/174>`_)

* Respect ``-r`` flag when calling :program:`proj` with ``-V``. (`#184 <https://github.com/OSGeo/proj.4/issues/184>`_)

* Remove multiplication by 2 at the equator error in Stereographic projection. (`#194 <https://github.com/OSGeo/proj.4/issues/194>`_)

* Allow +alpha=0 and +gamma=0 when using Oblique Mercator. (`#195 <https://github.com/OSGeo/proj.4/issues/195>`_)

* Return correct result of inverse Oblique Mercator when alpha is between 90 and 270. (`#331 <https://github.com/OSGeo/proj.4/issues/331>`_)

* Avoid segmentation fault when accessing point outside grid. (`#396 <https://github.com/OSGeo/proj.4/issues/396>`_)

* Avoid segmentation fault on NaN input in Robin inverse. (`#463 <https://github.com/OSGeo/proj.4/issues/463>`_)

* Very verbose use of :program:`proj` (``-V``) on Windows is fixed. (`#484 <https://github.com/OSGeo/proj.4/issues/484>`_)

* Fixed memory leak in General Oblique Transformation. (`#497 <https://github.com/OSGeo/proj.4/issues/497>`_)

* Equations for meridian convergence and partial derivatives have
  been corrected for non-conformal projections. (`#526 <https://github.com/OSGeo/proj.4/issues/526>`_)

* Fixed scaling of cartesian coordinates in ``pj_transform()``. (`#726 <https://github.com/OSGeo/proj.4/issues/726>`_)

* Additional bug fixes courtesy of `Google's OSS-Fuzz program <https://bugs.chromium.org/p/oss-fuzz/issues/list?can=1&q=proj4>`_
