.. _geodesic:

Geodesic calculations
=====================

Introduction
------------

Consider an ellipsoid of revolution with equatorial radius :math:`a`, polar
semi-axis :math:`b`, and flattening :math:`f=(a-b)/a`.  Points on
the surface of the ellipsoid are characterized by their latitude :math:`\phi`
and longitude :math:`\lambda`.  (Note that latitude here means the
*geographical latitude*, the angle between the normal to the ellipsoid
and the equatorial plane).

The shortest path between two points on the ellipsoid at
:math:`(\phi_1,\lambda_1)` and :math:`(\phi_2,\lambda_2)`
is called the geodesic.  Its length is
:math:`s_{12}` and the geodesic from point 1 to point 2 has forward
azimuths :math:`\alpha_1` and :math:`\alpha_2` at the two end
points.  In this figure, we have :math:`\lambda_{12}=\lambda_2-\lambda_1`.

    .. raw:: html

       <center>
         <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/cb/Geodesic_problem_on_an_ellipsoid.svg/320px-Geodesic_problem_on_an_ellipsoid.svg.png"
              alt="Figure from wikipedia"
              width="320">
       </center>

A geodesic can be extended indefinitely by requiring that any
sufficiently small segment is a shortest path; geodesics are also the
straightest curves on the surface.

Solution of geodesic problems
-----------------------------

Traditionally two geodesic problems are considered:

* the direct problem — given :math:`\phi_1`,
  :math:`\lambda_1`, :math:`\alpha_1`, :math:`s_{12}`,
  determine :math:`\phi_2`, :math:`\lambda_2`, :math:`\alpha_2`.

* the inverse problem — given  :math:`\phi_1`,
  :math:`\lambda_1`,  :math:`\phi_2`, :math:`\lambda_2`,
  determine :math:`s_{12}`, :math:`\alpha_1`,
  :math:`\alpha_2`.

PROJ incorporates `C library for Geodesics
<https://geographiclib.sourceforge.io/1.52/C/>`_ from `GeographicLib
<https://geographiclib.sourceforge.io>`_.  This library provides
routines to solve the direct and inverse geodesic problems.  Full double
precision accuracy is maintained provided that
:math:`\lvert f\rvert<\frac1{50}`.  Refer to the `application programming interface
<https://geographiclib.sourceforge.io/1.52/C/geodesic_8h.html>`_
for full documentation.  A brief summary of the routines is given in
geodesic(3).

The interface to the geodesic routines differ in two respects from the
rest of PROJ:

* angles (latitudes, longitudes, and azimuths) are in degrees (instead
  of in radians);
* the shape of ellipsoid is specified by the flattening :math:`f`; this can
  be negative to denote a prolate ellipsoid; setting :math:`f=0` corresponds
  to a sphere, in which case the geodesic becomes a great circle.

PROJ also includes a command line tool, :ref:`geod`\ (1), for performing
simple geodesic calculations.

Additional properties
---------------------

The routines also calculate several other quantities of interest

* :math:`S_{12}` is the area between the geodesic from point 1 to
  point 2 and the equator; i.e., it is the area, measured
  counter-clockwise, of the quadrilateral with corners
  :math:`(\phi_1,\lambda_1)`, :math:`(0,\lambda_1)`,
  :math:`(0,\lambda_2)`, and
  :math:`(\phi_2,\lambda_2)`.  It is given in
  meters\ :sup:`2`.
* :math:`m_{12}`, the reduced length of the geodesic is defined such
  that if the initial azimuth is perturbed by :math:`d\alpha_1`
  (radians) then the second point is displaced by :math:`m_{12}\,d\alpha_1`
  in the direction perpendicular to the
  geodesic.  :math:`m_{12}` is given in meters.  On a curved surface
  the reduced length obeys a symmetry relation, :math:`m_{12}+m_{21}=0`.
  On a flat surface, we have :math:`m_{12}=s_{12}`.
* :math:`M_{12}` and :math:`M_{21}` are geodesic scales.  If two
  geodesics are parallel at point 1 and separated by a small distance
  :math:`dt`, then they are separated by a distance :math:`M_{12}\,dt` at
  point 2.  :math:`M_{21}` is defined similarly (with the geodesics
  being parallel to one another at point 2).  :math:`M_{12}` and
  :math:`M_{21}` are dimensionless quantities.  On a flat surface,
  we have :math:`M_{12}=M_{21}=1`.
* :math:`\sigma_{12}` is the arc length on the auxiliary sphere.
  This is a construct for converting the problem to one in spherical
  trigonometry.  The spherical arc length from one equator crossing to
  the next is always :math:`180^\circ`.

If points 1, 2, and 3 lie on a single geodesic, then the following
addition rules hold:

* :math:`s_{13}=s_{12}+s_{23}`,
* :math:`\sigma_{13}=\sigma_{12}+\sigma_{23}`,
* :math:`S_{13}=S_{12}+S_{23}`,
* :math:`m_{13}=m_{12}M_{23}+m_{23}M_{21}`,
* :math:`M_{13}=M_{12}M_{23}-(1-M_{12}M_{21})m_{23}/m_{12}`,
* :math:`M_{31}=M_{32}M_{21}-(1-M_{23}M_{32})m_{12}/m_{23}`.

Multiple shortest geodesics
---------------------------

The shortest distance found by solving the inverse problem is
(obviously) uniquely defined.  However, in a few special cases there are
multiple azimuths which yield the same shortest distance.  Here is a
catalog of those cases:

* :math:`\phi_1=-\phi_2` (with neither point at
  a pole).  If :math:`\alpha_1=\alpha_2`, the geodesic
  is unique.  Otherwise there are two geodesics and the second one is
  obtained by setting
  :math:`[\alpha_1,\alpha_2]\leftarrow[\alpha_2,\alpha_1]`,
  :math:`[M_{12},M_{21}]\leftarrow[M_{21},M_{12}]`,
  :math:`S_{12}\leftarrow-S_{12}`.
  (This occurs when the longitude difference is near :math:`\pm180^\circ`
  for oblate ellipsoids.)
* :math:`\lambda_2=\lambda_1\pm180^\circ` (with
  neither point at a pole).  If :math:`\alpha_1=0^\circ` or
  :math:`\pm180^\circ`, the geodesic is unique.  Otherwise there are two
  geodesics and the second one is obtained by setting
  :math:`[\alpha_1,\alpha_2]\leftarrow[-\alpha_1,-\alpha_2]`,
  :math:`S_{12}\leftarrow-S_{12}`.  (This occurs when
  :math:`\phi_2` is near :math:`-\phi_1` for prolate
  ellipsoids.)
* Points 1 and 2 at opposite poles.  There are infinitely many
  geodesics which can be generated by setting
  :math:`[\alpha_1,\alpha_2]\leftarrow[\alpha_1,\alpha_2]+[\delta,-\delta]`,
  for arbitrary :math:`\delta`.
  (For spheres, this prescription applies when points 1 and 2 are
  antipodal.)
* :math:`s_{12}=0` (coincident points).  There are infinitely many
  geodesics which can be generated by setting
  :math:`[\alpha_1,\alpha_2]\leftarrow[\alpha_1,\alpha_2]+[\delta,\delta]`,
  for arbitrary :math:`\delta`.

Background
----------

The algorithms implemented by this package are given in :cite:`Karney2013`
(`addenda <https://geographiclib.sourceforge.io/geod-addenda.html>`_)
and are based on :cite:`Bessel1825` and :cite:`Helmert1880`; the algorithm for
areas is based on :cite:`Danielsen1989`.  These improve on the work of
:cite:`Vincenty1975` in the following respects:

* The results are accurate to round-off for terrestrial ellipsoids (the
  error in the distance is less than 15 nanometers, compared to 0.1 mm
  for Vincenty).
* The solution of the inverse problem is always found.  (Vincenty's
  method fails to converge for nearly antipodal points.)
* The routines calculate differential and integral properties of a
  geodesic.  This allows, for example, the area of a geodesic polygon to
  be computed.

Additional background material is provided in GeographicLib's `geodesic
bibliography <https://geographiclib.sourceforge.io/geodesic-papers/biblio.html>`_,
Wikipedia's article "`Geodesics on an ellipsoid
<https://en.wikipedia.org/wiki/Geodesics_on_an_ellipsoid>`_", and :cite:`Karney2011`
(`errata <https://geographiclib.sourceforge.io/geod-addenda.html#geod-errata>`_).
