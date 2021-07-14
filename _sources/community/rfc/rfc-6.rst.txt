.. _rfc6:

====================================================================
PROJ RFC 6: Triangulation-based transformations
====================================================================

:Author: Even Rouault
:Contact: even.rouault@spatialys.com
:Status: Adopted
:Implementation target: PROJ 7.2
:Last Updated: 2020-09-02

Summary
-------------------------------------------------------------------------------

This RFC adds a new transformation method, ``tinshift`` (TIN stands for
Triangulated Irregular Network)

The motivation for this work is to be able to handle the official transformations
created by National Land Survey of Finland, for:

- horizontal transformation between the KKJ and ETRS89 horizontal datums
- vertical transformations between N43 and N60 heights, and N60 and N2000 heights.

Such transformations are somehow related to traditional grid-based transformations,
except that the correction values are hold by the vertices of the triangulation,
instead of being at nodes of a grid.

Triangulation are in a number of cases the initial product of a geodesic adjustment,
with grids being a derived product. The Swiss grids have for example
derived products of an original triangulation.

Grid-based transformations remain very convenient to use because accessing
correction values is really easy and efficient, so triangulation-based transformations
are not meant as replacing them, but more about it being a complement, that is
sometimes necessary to be able to replicate the results of a officially vetted
transformation to a millimetric or better precision (speaking here about reproducibility
of numeric results, rather than the physical accuracy of the transformation that
might rather be centimetric). It is always possible to approach the result of
the triangulation with a grid, but that may require to adopt a small grid step,
and thus generate a grid that can be much larger than the original triangulation.

Details
-------------------------------------------------------------------------------

Transformation
++++++++++++++

A new transformation method, ``tinshift``, is added. It takes one mandatory
argument, ``file``, that points to a JSON file, which contains the triangulation
and associated metadata. Input and output coordinates must be geographic or projected.
Depending on the content
of the JSON file, horizontal, vertical or both components of the coordinates may
be transformed.

The transformation is used like:

::

    $ echo 3210000.0000 6700000.0000 0 2020 | cct +proj=tinshift +file=./triangulation_kkj.json

    209948.3217     6697187.0009    0.0000     2020

The transformation is invertible, with the same computational complexity than
the forward transformation.

Algorithm
+++++++++

Internally, ``tinshift`` ingest the whole file into memory. It is considered that
triangulation should be small enough for that. The above mentioned KKJ to ETRS89
triangulation fits into 65 KB of JSON, for 1449 triangles and 767 vertices.

When a point is transformed, one must find the triangle into which it falls into.
Instead of iterating over all triangles, we build a in-memory quadtree to speed-up
the identification of candidates triangles. On the above mentioned KKJ -> ETRS89
triangulation, this speeds up the whole transformation by a factor of 10. The
quadtree structure is a very good compromise between the performance gain it brings
and the simplicity of its implementation (we have ported the implementation coming
from GDAL, inherit from the one used for shapefile .spx spatial indices).

To determine if a point falls into a triangle, one computes its 3
`barycentric coordinates <https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates>`_
from its projected coordinates, :math:`\lambda_i` for :math:`i=1,2,3`.
They are real values (in the [0,1] range for a point inside the triangle),
giving the weight of each of the 3 vertices of the triangles.

Once those weights are known, interpolating the target horizontal
coordinate is a matter of doing the linear combination of those weights with
the target horizontal coordinates at the 3 vertices of the triangle (:math:`Xt_i` and :math:`Yt_i`):

.. math::

    X_{target} = Xt_1 * \lambda_1 + Xt_2 * \lambda_2 + Xt_3 * \lambda_3

    Y_{target} = Yt_1 * \lambda_1 + Yt_2 * \lambda_2 + Yt_3 * \lambda_3

This interpolation is exact at the vertices of the triangulation, and has linear properties
inside each triangle. It is completely equivalent to other formulations of
triangular interpolation, such as

.. math::

    X_{target} = A + X_{source} * B  + Y_{source} * C

    Y_{target} = D + X_{source} * E  + Y_{source} * F

where the A, B, C, D, E, F constants (for a given triangle) are found by solving
the 2 systems of 3 linear equations, constraint by the source and target coordinate pairs
of the 3 vertices of the triangle:

.. math::

    Xt_i = A + Xs_i * B + Ys_i * C

    Yt_i = D + Xs_i * E + Ys_i * F


.. note:: 

    From experiments, the interpolation using barycentric coordinates is slightly
    more numerically robust when interpolating projected coordinates of amplitude of the
    order of 1e5 / 1e6, due to computations involving differences of coordinates.
    Whereas the formulation with the A, B, C, D, E, F tends to have big values for
    the A and D constants, and values clause to 0 for C and E, and close to 1 for
    B and F. However, the difference between the two approaches is negligible for
    practical purposes (below micrometre precision)

Similarly for a vertical coordinate transformation, where :math:`Zoff_i` is the vertical
offset at each vertex of the triangle:

.. math::

    Z_{target} = Z_{source} + Zoff_1 * \lambda_1 + Zoff_2 * \lambda_2 + Zoff_3 * \lambda_3

Constraints on the triangulation
++++++++++++++++++++++++++++++++

No check is done on the consistence of the triangulation. It is highly
recommended that triangles do not overlap each other (when considering the
source coordinates or the forward transformation, or the target coordinates for
the inverse transformation), otherwise which triangle will be selected is
unspecified. Besides that, the triangulation does not need to have particular
properties (like being a Delaunay triangulation)

File format
+++++++++++

To the best of our knowledge, there are no established file formats to convey
geodetic transformations as triangulations. Potential similar formats to store TINs
are `ITF <http://vterrain.org/Implementation/Formats/ITF.html>`_ or
`XMS <https://www.xmswiki.com/wiki/TIN_Files>`_.
Both of them would need to be extended in order to handle datum shift information,
since they are both intended for mostly DEM use.

We thus propose a text-based format, using JSON as a serialization. Using a text-based
format could potentially be thought as a limitation performance-wise compared to
binary formats, but for the size of triangulations considered (a few thousands triangles / vertices),
there is no issue. Loading such file is a matter of 20 milliseconds or so. For reference,
loading a triangulation of about 115 000 triangles and 71 000 vertices takes 450 ms.

Using JSON provides generic formatting and parsing rules, and convenience to
create it from Python script for examples. This could also be easily generated "at hand"
by non-JSON aware writers.

For generic metadata, we reuse closely what has been used for the
`Deformation model master file <https://github.com/linz/deformation-model-format>`_

Below a minimal example, from the KKJ to ETRS89 transformation, with just a
single triangle:

.. code-block:: json

    {
        "file_type": "triangulation_file",
        "format_version": "1.0",
        "name": "Name",
        "version": "Version",
        "publication_date": "2018-07-01T00:00:00Z",
        "license": "Creative Commons Attribution 4.0 International",
        "description": "Test triangulation",
        "authority": {
            "name": "Authority name",
            "url": "http://example.com",
            "address": "Address",
            "email": "test@example.com"
        },
        "links": [
        {
            "href": "https://example.com/about.html",
            "rel": "about",
            "type": "text/html",
            "title": "About"
        },
        {
            "href": "https://example.com/download",
            "rel": "source",
            "type": "application/zip",
            "title": "Authoritative source"
        },
        {
            "href": "https://creativecommons.org/licenses/by/4.0/",
            "rel": "license",
            "type": "text/html",
            "title": "Creative Commons Attribution 4.0 International license"
        },
        {
            "href": "https://example.com/metadata.xml",
            "rel": "metadata",
            "type": "application/xml",
        "title": " ISO 19115 XML encoded metadata regarding the triangulation"
            }
        ],
        "input_crs": "EPSG:2393",
        "target_crs": "EPSG:3067",
        "transformed_components": [ "horizontal" ],
        "vertices_columns": [ "source_x", "source_y", "target_x", "target_y" ],
        "triangles_columns": [ "idx_vertex1", "idx_vertex2", "idx_vertex3" ],
        "vertices": [ [3244102.707, 6693710.937, 244037.137, 6690900.686],
                      [3205290.722, 6715311.822, 205240.895, 6712492.577],
                      [3218328.492, 6649538.429, 218273.648, 6646745.973] ],
        "triangles": [ [0, 1, 2] ]
    }

So after the generic metadata, we define the input and output CRS (informative
only), and that the transformation affects horizontal components of
coordinates. We name the columns of the ``vertices`` and ``triangles`` arrays.
We defined the source and target coordinates of each vertex, and define a
triangle by referring to the index of its vertices in the ``vertices`` array.

More formally, the specific items for the triangulation file are:

input_crs
  String identifying the CRS of source coordinates
  in the vertices. Typically ``EPSG:XXXX``. If the transformation is for vertical
  component, this should be the code for a compound CRS (can be EPSG:XXXX+YYYY
  where XXXX is the code of the horizontal CRS and YYYY the code of the vertical CRS).
  For example, for the KKJ->ETRS89 transformation, this is EPSG:2393
  (``KKJ / Finland Uniform Coordinate System``). The input coordinates are assumed
  to be passed in the "normalized for visualisation" / "GIS friendly" order,
  that is longitude, latitude for geographic coordinates and
  easting, northing for projected coordinates.


output_crs
  String identifying the CRS of target coordinates in the vertices.
  Typically ``EPSG:XXXX``. If the transformation is for vertical component,
  this should be the code for a compound CRS (can be EPSG:XXXX+YYYY where
  XXXX is the code of the horizontal CRS and YYYY the code of the vertical CRS).
  For example, for the KKJ->ETRS89 transformation, this is EPSG:3067
  (\"ETRS89 / TM35FIN(E,N)\"). The output coordinates will be returned in
  the "normalized for visualisation" / "GIS friendly" order,
  that is longitude, latitude for geographic coordinates and
  easting, northing for projected coordinates.


transformed_components
  Array which may contain one or two strings: "horizontal" when horizontal
  components of the coordinates are transformed and/or "vertical" when the
  vertical component is transformed.


vertices_columns
  Specify the name of the columns of the rows in the ``vertices``
  array. There must be exactly as many elements in ``vertices_columns`` as in a
  row of ``vertices``. The following names have a special meaning: ``source_x``,
  ``source_y``, ``target_x``, ``target_y``, ``source_z``, ``target_z`` and
  ``offset_z``.  ``source_x`` and ``source_y`` are compulsory.
  ``source_x`` is for the source longitude (in degree) or easting.
  ``source_y`` is for the source latitude (in degree) or northing. 
  ``target_x`` and ``target_y`` are compulsory when ``horizontal`` is specified
  in ``transformed_components``. (``source_z`` and ``target_z``) or
  ``offset_z`` are compulsory when ``vertical`` is specified in ``transformed_components``


triangles_columns
  Specify the name of the columns of the rows in the
  ``triangles`` array. There must be exactly as many elements in ``triangles_columns``
  as in a row of ``triangles``. The following names have a special meaning:
  ``idx_vertex1``, ``idx_vertex2``, ``idx_vertex3``. They are compulsory.


vertices
  An array whose items are themselves arrays with as many columns as
  described in ``vertices_columns``.


triangles
  An array whose items are themselves arrays with as many columns as
  described in ``triangles_columns``.
  The value of the ``idx_vertexN`` columns must be indices
  (between 0 and len(``vertices``-1) of items of the ``vertices`` array.

Code impacts
++++++++++++

The following new files are added in src/transformations:

- tinshift.cpp: PROJ specific code for defining the new operation. Takes care
  of the input and output coordinate conversions (between input_crs and triangulation_source_crs,
  and triangulation_target_crs and output_crs), when needed.
- tinshift.hpp: Header-based implementation. This file contains the API.
- tinshift_exceptions.hpp: Exceptions that can be raised during file parsing
- tinshift_impl.hpp: Implementation of file loading, triangle search and interpolation.

This is the approach that has been followed for the deformation model implementation,
and which makes it easier to do unit test.

src/quadtree.hpp contains a quadtree implementation.

Performance indications
+++++++++++++++++++++++

Tested on Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz, transforming 4 million points

For the KKJ to ETRS89 transformation (1449 triangles and 767 vertices),
4.4 million points / sec can be transformed.

For comparison, the Helmert-based KKJ to ETRS89 transformation operates at
1.6 million points / sec.

A triangulation with about 115 000 triangles and 71 000 vertices
operates at 2.2 million points / sec
(throughput on more points would be better since the initial loading of the
triangulation is non-negligible here)

Backward compatibility
-------------------------------------------------------------------------------

New functionality fully backward compatible.

Testing
-------------------------------------------------------------------------------

The PROJ test suite will be enhanced to test the new transformation, with a
new .gie file, and a C++ unit test to test at a lower level.

Documentation
-------------------------------------------------------------------------------

- The tinshift method will be documented.
- The JSON format will be documented under https://proj.org/specifications/
- A JSON schema will also be provided.

Proposed implementation
-------------------------------------------------------------------------------

An initial implementation is available at https://github.com/rouault/PROJ/tree/tinshift

References
-------------------------------------------------------------------------------

`Finnish coordinate transformation (automated translation to English) <https://translate.google.fr/translate?sl=auto&tl=en&u=https%3A%2F%2Fwww.maanmittauslaitos.fi%2Fkartat-ja-paikkatieto%2Fasiantuntevalle-kayttajalle%2Fkoordinaattimuunnokset>`_

Adoption status
-------------------------------------------------------------------------------

The RFC was adopted on 2020-09-02 with +1's from the following PSC members

* Kristian Evers
* Charles Karney
* Thomas Knudsen
* Even Rouault

Funding
-------------------------------------------------------------------------------

This work is funded by `National Land Survey of Finland <https://www.maanmittauslaitos.fi/en>`_
