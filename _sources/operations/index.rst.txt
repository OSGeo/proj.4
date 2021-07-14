.. _operations:

================================================================================
Coordinate operations
================================================================================

Coordinate operations in PROJ are divided into three groups:
Projections, conversions  and transformations.
Projections are purely cartographic mappings of the sphere onto the plane.
Technically projections are conversions (according to ISO standards), though in
PROJ projections are distinguished from conversions. Conversions are coordinate
operations that do not exert a change in reference frame. Operations that do
exert a change in reference frame are called transformations.

.. toctree::
   :maxdepth: 1

   projections/index
   conversions/index
   transformations/index
   pipeline
   operations_computation
