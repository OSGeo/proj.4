.. _apps:

================================================================================
Applications
================================================================================

Bundled with PROJ comes a set of small command line utilities. The :program:`proj`
program is limited to converting between geographic and projection coordinates
within one datum. The :program:`cs2cs` program operates similarly, but allows
translation between any pair of definable coordinate systems, including support
for datum transformation. The :program:`geod` program provides the ability to do
geodesic (great circle) computations. :program:`gie` is the program used for
regression tests in PROJ. :program:`cct`, a 4D equivalent to the :program:`proj`
program, performs transformation coordinate systems on a set of input points.
:program:`projinfo` performs queries for geodetic objects and coordinate
operations. :program:`projsync` is a tool for synchronizing PROJ datum and
transformation support data.

.. toctree::
   :maxdepth: 1

   cct
   cs2cs
   geod
   gie
   proj
   projinfo
   projsync
