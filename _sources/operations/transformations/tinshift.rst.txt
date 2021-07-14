.. _tinshift:

================================================================================
Triangulated Irregular Network based transformation
================================================================================

.. versionadded:: 7.2.0

+---------------------+--------------------------------------------------------------------+
| **Alias**           | tinshift                                                           |
+---------------------+--------------------------------------------------------------------+
| **Input type**      | Projected or geographic coordinates (horizontal), meters (vertical)|
+---------------------+--------------------------------------------------------------------+
| **Output type**     | Projected or geographic coordinates (horizontal), meters (vertical)|
+---------------------+--------------------------------------------------------------------+
| **Domain**          | 2D or 3D                                                           |
+---------------------+--------------------------------------------------------------------+
| **Available forms** | Forward and inverse                                                |
+---------------------+--------------------------------------------------------------------+


The ``tinshift`` transformation takes one mandatory
argument, ``file``, that points to a JSON file, which contains the
triangulation and associated metadata. Input and output coordinates must be
geographic or projected coordinates.
Depending on the content of the JSON file, horizontal, vertical or both
components of the coordinates may be transformed.

The transformation is invertible, with the same computational complexity than
the forward transformation.

Parameters
-------------------------------------------------------------------------------

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +file=<filename>

    Filename to the JSON file for the TIN.


Example
-------------------------------------------------------------------------------

Transforming a point with the Finland EPSG:2393 ("KKJ / Finland Uniform
Coordinate System") projected CRS to EPSG:3067 ("ETRS89 / TM35FIN(E,N)")

::

    $ echo 3210000.0000 6700000.0000 0 2020 | cct +proj=tinshift +file=./triangulation_kkj.json

    209948.3217     6697187.0009    0.0000     2020


Algorithm
+++++++++

Internally, ``tinshift`` ingest the whole file into memory. It is considered that
triangulation should be small enough for that.

When a point is transformed, one must find the triangle into which it falls into.
Instead of iterating over all triangles, we build a in-memory quadtree to speed-up
the identification of candidates triangles.

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

The triangulation is stored in a text-based format, using JSON as a serialization.

Below a minimal example, from the KKJ to ETRS89 transformation, with just a
single triangle:

.. literalinclude:: ../../../../data/tests/tinshift_crs_implicit.json
  :language: json


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

A `JSON schema <https://proj.org/schemas/triangulation.schema.json>`_ is available
for this file format.
