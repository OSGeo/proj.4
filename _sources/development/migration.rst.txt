.. _API_migration:

================================================================================
Version 4 to 6 API Migration
================================================================================

This is a transition guide for developers wanting to migrate their code to use
PROJ version 6.

Code example
###############################################################################

The difference between the old and new API is shown here with a few examples. Below
we implement the same program with the two different API's. The program reads
input longitude and latitude from the command line and convert them to
projected coordinates with the Mercator projection.

We start by writing the program for PROJ 4:

.. code-block:: C

    #include <proj_api.h>

    main(int argc, char **argv) {
        projPJ pj_merc, pj_longlat;
        double x, y;
        int p;

        if (!(pj_longlat = pj_init_plus("+proj=longlat +ellps=clrk66")) )
            return 1;
        if (!(pj_merc = pj_init_plus("+proj=merc +datum=clrk66 +lat_ts=33")) )
            return 1;

        while (scanf("%lf %lf", &x, &y) == 2) {
            x *= DEG_TO_RAD; /* longitude */
            y *= DEG_TO_RAD; /* latitude */
            p = pj_transform(pj_longlat, pj_merc, 1, 1, &x, &y, NULL);
            printf("%.2f\t%.2f\n", x, y);
        }

        pj_free(pj_longlat);
        pj_free(pj_merc);

        return 0;
    }

The same program implemented using PROJ 6:

.. code-block:: C

    #include <proj.h>

    main(int argc, char **argv) {
        PJ *P;
        PJ_COORD c, c_out;

        /* NOTE: the use of PROJ strings to describe CRS is strongly discouraged */
        /* in PROJ 6, as PROJ strings are a poor way of describing a CRS, and */
        /* more precise its geodetic datum. */
        /* Use of codes provided by authorities (such as "EPSG:4326", etc...) */
        /* or WKT strings will bring the full power of the "transformation */
        /* engine" used by PROJ to determine the best transformation(s) between */
        /* two CRS. */
        P = proj_create_crs_to_crs(PJ_DEFAULT_CTX,
                                   "+proj=longlat +ellps=clrs66",
                                   "+proj=merc +ellps=clrk66 +lat_ts=33",
                                   NULL);
        if (P==0)
            return 1;

        {
            /* For that particular use case, this is not needed. */
            /* proj_normalize_for_visualization() ensures that the coordinate */
            /* order expected and returned by proj_trans() will be longitude, */
            /* latitude for geographic CRS, and easting, northing for projected */
            /* CRS. If instead of using PROJ strings as above, "EPSG:XXXX" codes */
            /* had been used, this might had been necessary. */
            PJ* P_for_GIS = proj_normalize_for_visualization(PJ_DEFAULT_CTX, P);
            if( 0 == P_for_GIS )  {
                proj_destroy(P);
                return 1;
            }
            proj_destroy(P);
            P = P_for_GIS;
        }

        /* For reliable geographic <--> geocentric conversions, z shall not */
        /* be some random value. Also t shall be initialized to HUGE_VAL to */
        /* allow for proper selection of time-dependent operations if one of */
        /* the CRS is dynamic. */
        c.lpzt.z = 0.0;
        c.lpzt.t = HUGE_VAL;

        while (scanf("%lf %lf", &c.lpzt.lam, &c.lpzt.phi) == 2) {
            /* No need to convert to radian */
            c_out = proj_trans(P, PJ_FWD, c);
            printf("%.2f\t%.2f\n", c_out.xy.x, c_out.xy.y);
        }

        proj_destroy(P);

        return 0;
    }


Function mapping from old to new API
###############################################################################

+---------------------------------------+-------------------------------------------------+
| Old API functions                     | New API functions                               |
+=======================================+=================================================+
| ``pj_fwd``                            | :c:func:`proj_trans`                            |
+---------------------------------------+-------------------------------------------------+
| ``pj_inv``                            | :c:func:`proj_trans`                            |
+---------------------------------------+-------------------------------------------------+
| ``pj_fwd3``                           | :c:func:`proj_trans`                            |
+---------------------------------------+-------------------------------------------------+
| ``pj_inv3``                           | :c:func:`proj_trans`                            |
+---------------------------------------+-------------------------------------------------+
| ``pj_transform``                      | :c:func:`proj_create_crs_to_crs`                |
|                                       | or :c:func:`proj_create_crs_to_crs_from_pj` +   |
|                                       | (:c:func:`proj_normalize_for_visualization` +)  |
|                                       | :c:func:`proj_trans`,                           |
|                                       | :c:func:`proj_trans_array` or                   |
|                                       | :c:func:`proj_trans_generic`                    |
+---------------------------------------+-------------------------------------------------+
| ``pj_init``                           | :c:func:`proj_create` /                         |
|                                       | :c:func:`proj_create_crs_to_crs`                |
+---------------------------------------+-------------------------------------------------+
| ``pj_init``                           | :c:func:`proj_create` /                         |
|                                       | :c:func:`proj_create_crs_to_crs`                |
+---------------------------------------+-------------------------------------------------+
| ``pj_free``                           | :c:func:`proj_destroy`                          |
+---------------------------------------+-------------------------------------------------+
| ``pj_is_latlong``                     | :c:func:`proj_get_type`                         |
+---------------------------------------+-------------------------------------------------+
| ``pj_is_geocent``                     | :c:func:`proj_get_type`                         |
+---------------------------------------+-------------------------------------------------+
| ``pj_get_def``                        | :c:func:`proj_pj_info`                          |
+---------------------------------------+-------------------------------------------------+
| ``pj_latlong_from_proj``              | *No direct equivalent*, but can be accomplished |
|                                       | by chaining :c:func:`proj_create`,              |
|                                       | :c:func:`proj_crs_get_horizontal_datum` and     |
|                                       | :c:func:`proj_create_geographic_crs_from_datum` |
+---------------------------------------+-------------------------------------------------+
| ``pj_set_finder``                     | :c:func:`proj_context_set_file_finder`          |
+---------------------------------------+-------------------------------------------------+
| ``pj_set_searchpath``                 | :c:func:`proj_context_set_search_paths`         |
+---------------------------------------+-------------------------------------------------+
| ``pj_deallocate_grids``               | *No equivalent*                                 |
+---------------------------------------+-------------------------------------------------+
| ``pj_strerrno``                       | *No equivalent*                                 |
+---------------------------------------+-------------------------------------------------+
| ``pj_get_errno_ref``                  | :c:func:`proj_errno`                            |
+---------------------------------------+-------------------------------------------------+
| ``pj_get_release``                    | :c:func:`proj_info`                             |
+---------------------------------------+-------------------------------------------------+


Backward incompatibilities
###############################################################################

Access to the :file:`proj_api.h` is still possible but requires to define the
``ACCEPT_USE_OF_DEPRECATED_PROJ_API_H`` macro.

The emulation of the now deprecated ``+init=epsg:XXXX`` syntax in PROJ 6 is not
fully compatible with previous versions.

In particular, when used with the ``pj_transform()`` function, no datum shift term
(``towgs84``, ``nadgrids``, ``geoidgrids``) will be added during the expansion of the
``+init=epsg:XXXX`` string to ``+proj=YYYY ....``. If you still uses ``pj_transform()``
and want datum shift to be applied, then you need to provide a fully expanded
string with appropriate ``towgs84``, ``nadgrids`` or ``geoidgrids`` terms to ``pj_init()``.

To use the ``+init=epsg:XXXX`` syntax with :c:func:`proj_create` and then
:c:func:`proj_create_crs_to_crs`, ``proj_context_use_proj4_init_rules(ctx, TRUE)``
or the ``PROJ_USE_PROJ4_INIT_RULES=YES`` environment variable must have been
previously set. In that context, datum shift will be researched. However they
might be different than with PROJ 4 or PROJ 5, since a "late-binding" approach
will be used (that is trying to find as much as possible the most direct
transformation between the source and target datum), whereas PROJ 4 or PROJ 5
used an "early-binding" approach consisting in always going to EPSG:4326 / WGS 84.


Feedback from downstream projects on the PROJ 6 migration
###############################################################################

* `PROJ 6 adoption by Spatialite <https://www.gaia-gis.it/fossil/libspatialite/wiki?name=PROJ.6>`_

* `On GDA2020, PROJ 6 and QGIS: Lessons learnt and recommendations for handling GDA2020 within geospatial software development <https://www.icsm.gov.au/sites/default/files/North%20Road%20Handling%20GDA2020%20within%20Geospatial%20Software%20Development.pdf>`_

================================================================================
Version 4 to 5 API Migration
================================================================================

This is a transition guide for developers wanting to migrate their code to use
PROJ version 5.


Background
###############################################################################

Before we go on, a bit of background is needed. The new API takes a different
view of the world than the old because it is needed in order to obtain high
accuracy transformations. The old API is constructed in such a way that any transformation
between two coordinate reference systems *must* pass through the ill-defined
WGS84 reference frame, using it as a hub. The new API does away with this limitation to
transformations in PROJ. It is still possible to do that type of transformations
but in many cases there will be a better alternative.

The world view represented by the old API is always sufficient if all you care about is
meter level accuracy - and in many cases it will provide much higher accuracy
than that. But the view that “WGS84 is the *true* foundation of the world, and
everything else can be transformed natively to and from WGS84” is inherently flawed.

First and foremost because any time WGS84 is mentioned, you should ask yourself
“Which of the six WGS84 realizations are we talking about here?”.

Second, because for many (especially legacy) systems, it may not be straightforward
to transform to WGS84 (or actually ITRF-something, ETRS-something or NAD-something
which appear to be the practical meaning of the term WGS84 in everyday PROJ related
work), while centimeter-level accurate transformations may exist between pairs of
older systems.

The concept of a hub reference frame (“datum”) is not inherently bad, but in many
cases you need to handle and select that datum with more care than the old API allows.
The primary aim of the new API is to allow just that. And to do that, you must realize
that the world is inherently 4 dimensional. You may in many cases assume one or more of
the coordinates to be constant, but basically, to obtain geodetic accuracy transformations,
you need to work in 4 dimensions.

Now, having described the background for introducing the new API, let's try to show
how to use it. First note that in order to go from system A to system B, the old API
starts by doing an **inverse** transformation from system A to WGS84, then does a
**forward** transformation from WGS84 to system B.

With :program:`cs2cs` being the command line interface to the old API, and
:program:`cct` being the same for the new, this example of doing the same
thing in both world views will should give an idea of the differences:

::

    $ echo 300000 6100000 | cs2cs +proj=utm +zone=33 +ellps=GRS80 +to +proj=utm +zone=32 +ellps=GRS80
    683687.87       6099299.66 0.00


    $ echo 300000 6100000 0 0 | cct +proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=utm +zone=32 +ellps=GRS80
    683687.8667   6099299.6624    0.0000    0.0000

Lookout for the ``+inv`` in the first ``+step``, indicating an inverse transform.


Code example
###############################################################################

The difference between the old and new API is shown here with a few examples. Below
we implement the same program with the two different API's. The program reads
input longitude and latitude from the command line and convert them to
projected coordinates with the Mercator projection.

We start by writing the program for PROJ v. 4:

.. code-block:: C

    #include <proj_api.h>

    main(int argc, char **argv) {
        projPJ pj_merc, pj_longlat;
        double x, y;

        if (!(pj_longlat = pj_init_plus("+proj=longlat +ellps=clrk66")) )
            return 1;
        if (!(pj_merc = pj_init_plus("+proj=merc +ellps=clrk66 +lat_ts=33")) )
            return 1;

        while (scanf("%lf %lf", &x, &y) == 2) {
            x *= DEG_TO_RAD; /* longitude */
            y *= DEG_TO_RAD; /* latitude */
            p = pj_transform(pj_longlat, pj_merc, 1, 1, &x, &y, NULL );
            printf("%.2f\t%.2f\n", x, y);
        }

        pj_free(pj_longlat);
        pj_free(pj_merc);

        return 0;
    }

The same program implemented using PROJ v. 5:

.. code-block:: C

    #include <proj.h>

    main(int argc, char **argv) {
        PJ *P;
        PJ_COORD c;

        P = proj_create(PJ_DEFAULT_CTX, "+proj=merc +ellps=clrk66 +lat_ts=33");
        if (P==0)
            return 1;

        while (scanf("%lf %lf", &c.lp.lam, &c.lp.phi) == 2) {
            c.lp.lam = proj_torad(c.lp.lam);
            c.lp.phi = proj_torad(c.lp.phi);
            c = proj_trans(P, PJ_FWD, c);
            printf("%.2f\t%.2f\n", c.xy.x, c.xy.y);
        }

        proj_destroy(P);
    }

Looking at the two different programs, there's a few immediate
differences that catches the eye. First off, the included header file describing
the API has changed from ``proj_api.h`` to simply ``proj.h``. All functions in ``proj.h``
belongs to the ``proj_`` namespace.

With the new API also comes new datatypes. E.g. the transformation object ``projPJ``
which has been changed to a pointer of type ``PJ``. This is done to highlight the
actual nature of the object, instead of hiding it away behind a typedef. New data
types for handling coordinates have also been introduced. In the above example we
use the ``PJ_COORD``, which is a union of various types. The benefit of this is that
it is possible to use the various structs in the union to communicate what state
the data is in at different points in the program. For instance as in the above
example where the coordinate is read from STDIN as a geodetic coordinate,
communicated to the reader of the code by using the ``c.lp`` struct.
After it has been projected we print it to STDOUT by accessing the individual
elements in ``c.xy`` to illustrate that the coordinate is now in projected space.
Data types are prefixed with `PJ_`.

The final, and perhaps biggest, change is that the fundamental concept of
transformations in PROJ are now handled in a single transformation object (``PJ``)
and not by stating the source and destination systems as previously. It is of
course still possible to do just that, but the transformation object now
captures the whole transformation from source to destination in one. In the
example with the old API the source system is described as
``+proj=latlon +ellps=clrk66`` and the destination system is described as
``+proj=merc +ellps=clrk66 +lat_ts=33``. Since the Mercator projection accepts
geodetic coordinates as its input, the description of the source in this case
is superfluous. We use that to our advantage in the new API and simply state
the destination. This is simple at a glance, but is actually a big conceptual
change. We are now focused on the path between two systems instead of what the
source and destination systems are.

Function mapping from old to new API
###############################################################################

+---------------------------------------+---------------------------------------+
| Old API functions                     | New API functions                     |
+=======================================+=======================================+
| ``pj_fwd``                            | :c:func:`proj_trans`                  |
+---------------------------------------+---------------------------------------+
| ``pj_inv``                            | :c:func:`proj_trans`                  |
+---------------------------------------+---------------------------------------+
| ``pj_fwd3``                           | :c:func:`proj_trans`                  |
+---------------------------------------+---------------------------------------+
| ``pj_inv3``                           | :c:func:`proj_trans`                  |
+---------------------------------------+---------------------------------------+
| ``pj_transform``                      | :c:func:`proj_trans_array` or         |
|                                       | :c:func:`proj_trans_generic`          |
+---------------------------------------+---------------------------------------+
| ``pj_init``                           | :c:func:`proj_create`                 |
+---------------------------------------+---------------------------------------+
| ``pj_init_plus``                      | :c:func:`proj_create`                 |
+---------------------------------------+---------------------------------------+
| ``pj_free``                           | :c:func:`proj_destroy`                |
+---------------------------------------+---------------------------------------+
| ``pj_is_latlong``                     | :c:func:`proj_angular_output`         |
+---------------------------------------+---------------------------------------+
| ``pj_is_geocent``                     | :c:func:`proj_angular_output`         |
+---------------------------------------+---------------------------------------+
| ``pj_get_def``                        | :c:func:`proj_pj_info`                |
+---------------------------------------+---------------------------------------+
| ``pj_latlong_from_proj``              | *No equivalent*                       |
+---------------------------------------+---------------------------------------+
| ``pj_set_finder``                     | *No equivalent*                       |
+---------------------------------------+---------------------------------------+
| ``pj_set_searchpath``                 | *No equivalent*                       |
+---------------------------------------+---------------------------------------+
| ``pj_deallocate_grids``               | *No equivalent*                       |
+---------------------------------------+---------------------------------------+
| ``pj_strerrno``                       | *No equivalent*                       |
+---------------------------------------+---------------------------------------+
| ``pj_get_errno_ref``                  | :c:func:`proj_errno`                  |
+---------------------------------------+---------------------------------------+
| ``pj_get_release``                    | :c:func:`proj_info`                   |
+---------------------------------------+---------------------------------------+
