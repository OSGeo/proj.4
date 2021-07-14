.. _tmerc:

********************************************************************************
Transverse Mercator
********************************************************************************

The transverse Mercator projection in its various forms is the most widely used projected coordinate system for world topographical and offshore mapping.
It is a conformal projection in which a chosen meridian projects to a
straight line at constant scale.

+---------------------+----------------------------------------------------------+
| **Classification**  | Transverse and oblique cylindrical                       |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global, with full accuracy within 3900 km                |
|                     | of the central meridian                                  |
+---------------------+----------------------------------------------------------+
| **Alias**           | tmerc                                                    |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+



.. figure:: ./images/tmerc.png
   :width: 500 px
   :align: center
   :alt:   Transverse Mercator

   proj-string: ``+proj=tmerc``

Usage
#####


Prior to the development of the Universal Transverse Mercator coordinate system, several European nations demonstrated the utility of grid-based conformal maps by mapping their territory during the interwar period.
Calculating the distance between two points on these maps could be performed more easily in the field (using the Pythagorean theorem) than was possible using the trigonometric formulas required under the graticule-based system of latitude and longitude.
In the post-war years, these concepts were extended into the Universal Transverse Mercator/Universal Polar Stereographic (UTM/UPS) coordinate system, which is a global (or universal) system of grid-based maps.

The following table gives special cases of the Transverse Mercator projection.

+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| Projection Name                     | Areas                                               | Central meridian               | Zone width                                | Scale Factor |
+=====================================+=====================================================+================================+===========================================+==============+
| Transverse Mercator                 | World wide                                          | Various                        | less than 1000 km                         | Various      |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| Transverse Mercator south oriented  | Southern Africa                                     | 2° intervals E of 11°E         | 2°                                        | 1.000        |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| UTM North hemisphere                | World wide equator to 84°N                          | 6° intervals E & W of 3° E & W | Usually 6°, wider for Norway and Svalbard | 0.9996       |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| UTM South hemisphere                | World wide north of 80°S to equator                 | 6° intervals E & W of 3° E & W | Always 6°                                 | 0.9996       |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| Gauss-Kruger                        | Former USSR, Yugoslavia, Germany, S. America, China | Various, according to area     | Usually less than 6°, often less than 4°  | 1.0000       |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+
| Gauss Boaga                         | Italy                                               | Various, according to area     | 6°                                        | 0.9996       |
+-------------------------------------+-----------------------------------------------------+--------------------------------+-------------------------------------------+--------------+



Example using Gauss-Kruger on Germany area (aka EPSG:31467) ::

    $ echo 9 51 | proj +proj=tmerc +lat_0=0 +lon_0=9 +k_0=1 +x_0=3500000 +y_0=0 +ellps=bessel +units=m
    3500000.00  5651505.56

Example using Gauss Boaga on Italy area (EPSG:3004) ::

    $ echo 15 42 | proj +proj=tmerc +lat_0=0 +lon_0=15 +k_0=0.9996 +x_0=2520000 +y_0=0 +ellps=intl +units=m
    2520000.00  4649858.60

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. option:: +approx

    .. versionadded:: 6.0.0

    Use the Evenden-Snyder algorithm described below under "Legacy
    ellipsoidal form".  It is faster than the default algorithm, but is
    less accurate and diverges beyond 3° from the central meridian.

.. option:: +algo=auto/evenden_snyder/poder_engsager

    .. versionadded:: 7.1

    Selects the algorithm to use. The hardcoded value and the one defined in
    :ref:`proj-ini` default to ``poder_engsager``; that is the most precise
    one.

    When using auto, a heuristics based on the input coordinate to transform
    is used to determine if the faster Evenden-Snyder method can be used, for
    faster computation, without causing an error greater than 0.1 mm (for an
    ellipsoid of the size of Earth)

    Note that :option:`+approx` and :option:`+algo` are mutually exclusive.

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/k_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Mathematical definition
#######################

The formulation given here for the Transverse Mercator projection is due
to Krüger :cite:`Krueger1912` who gave the series expansions accurate to
:math:`n^4`, where :math:`n = (a-b)/(a+b)` is the third flattening.
These series were extended to sixth order by Engsager and Poder in
:cite:`Poder1998` and :cite:`Engsager2007`.  This gives full
double-precision accuracy within 3900 km of the central meridian (about
57% of the surface of the earth) :cite:`Karney2011tm`.  The error is
less than 0.1 mm within 7000 km of the central meridian (about 89% of
the surface of the earth).

This formulation consists of three steps: a conformal projection from
the ellipsoid to a sphere, the spherical transverse Mercator
projection, rectifying this projection to give constant scale on the
central meridian.

The scale on the central meridian is :math:`k_0` and is set by ``+k_0``.

Option :option:`+lon_0` sets the central meridian; in the formulation
below :math:`\lambda` is the longitude relative to the central meridian.

Options :option:`+lat_0`, :option:`+x_0`, and :option:`+y_0` serve to
translate the projected coordinates so that at :math:`(\phi, \lambda) =
(\phi_0, \lambda_0)`, the projected coordinates are :math:`(x,y) =
(x_0,y_0)`.  To simplify the formulas below, these options are set to
zero (their default values).

Because the projection is conformal, the formulation is most
conveniently given in terms of complex numbers.  In particular, the
unscaled projected coordinates :math:`\eta` (proportional to the
easting, :math:`x`) and :math:`\xi` (proportional to the northing,
:math:`y`) are combined into the single complex quantity :math:`\zeta =
\xi + i\eta`, where :math:`i=\sqrt{-1}`.  Then any analytic function
:math:`f(\zeta)` defines a conformal mapping (this follows from the
Cauchy-Riemann conditions).

Spherical form
**************

Because the full (ellipsoidal) projection includes the spherical
projection as one of the components, we present the spherical form first
with the coordinates tagged with primes, :math:`\phi'`,
:math:`\lambda'`, :math:`\zeta' = \xi' + i\eta'`, :math:`x'`,
:math:`y'`, so that they can be distinguished from the corresponding
ellipsoidal coordinates (without the primes).  The projected coordinates
for the sphere are given by

.. math::

   x' = k_0 R \eta';\qquad y' = k_0 R \xi'

Forward projection
==================

.. math::

   \xi' = \tan^{-1}\biggl(\frac{\tan\phi'}{\cos\lambda'}\biggr)

.. math::

   \eta' = \sinh^{-1}\biggl(\frac{\sin\lambda'}
   {\sqrt{\tan^2\phi' + \cos^2\lambda'}}\biggr)


Inverse projection
==================

.. math::

  \phi' = \tan^{-1}\biggl(\frac{\sin\xi'}
  {\sqrt{\sinh^2\eta' + \cos^2\xi'}}\biggr)

.. math::

  \lambda' = \tan^{-1}\biggl(\frac{\sinh\eta'}{\cos\xi'}\biggr)


Ellipsoidal form
****************

The projected coordinates are given by

.. math::

   \zeta = \xi + i\eta;\qquad x = k_0 A \eta;\qquad y = k_0 A \xi

.. math::

   A = \frac a{1+n}\biggl(1 + \frac14 n^2 + \frac1{64} n^4 +
   \frac1{256}n^6\biggr)

The series for conversion between ellipsoidal and spherical geographic
coordinates and ellipsoidal and spherical projected coordinates are
given in matrix notation where :math:`\mathbf S(\theta)` and
:math:`\mathbf N` are the row and column vectors of length 6

.. math::

   \mathbf S(\theta) = \begin{pmatrix}
   \sin  2\theta &
   \sin  4\theta &
   \sin  6\theta &
   \sin  8\theta &
   \sin 10\theta &
   \sin 12\theta
   \end{pmatrix}

.. math::

   \mathbf N = \begin{pmatrix}
   n \\ n^2 \\ n^3\\ n^4 \\ n^5 \\ n^6
   \end{pmatrix}

and :math:`\mathsf C_{\alpha,\beta}` are upper triangular
:math:`6\times6` matrices.

Relation between geographic coordinates
=======================================

.. math::

  \lambda' = \lambda

.. math::

   \phi' = \tan^{-1}\sinh\bigl(\sinh^{-1}\tan\phi
   - e \tanh^{-1}(e\sin\phi)\bigr)

Instead of using this analytical formula for :math:`\phi'`, the
conversions between :math:`\phi` and :math:`\phi'` use the series
approximations:

.. math::

  \phi' = \phi + \mathbf S(\phi) \cdot \mathsf C_{\chi,\phi} \cdot \mathbf N

.. math::

  \phi = \phi' + \mathbf S(\phi') \cdot \mathsf C_{\phi,\chi} \cdot \mathbf N

.. math::

  \mathsf C_{\chi,\phi} = \begin{pmatrix}
  -2& \frac{2}{3}& \frac{4}{3}& -\frac{82}{45}& \frac{32}{45}& \frac{4642}{4725} \\
  & \frac{5}{3}& -\frac{16}{15}& -\frac{13}{9}& \frac{904}{315}& -\frac{1522}{945} \\
  & & -\frac{26}{15}& \frac{34}{21}& \frac{8}{5}& -\frac{12686}{2835} \\
  & & & \frac{1237}{630}& -\frac{12}{5}& -\frac{24832}{14175} \\
  & & & & -\frac{734}{315}& \frac{109598}{31185} \\
  & & & & & \frac{444337}{155925} \\
  \end{pmatrix}

.. math::

  \mathsf C_{\phi,\chi} = \begin{pmatrix}
  2& -\frac{2}{3}& -2& \frac{116}{45}& \frac{26}{45}& -\frac{2854}{675} \\
  & \frac{7}{3}& -\frac{8}{5}& -\frac{227}{45}& \frac{2704}{315}& \frac{2323}{945} \\
  & & \frac{56}{15}& -\frac{136}{35}& -\frac{1262}{105}& \frac{73814}{2835} \\
  & & & \frac{4279}{630}& -\frac{332}{35}& -\frac{399572}{14175} \\
  & & & & \frac{4174}{315}& -\frac{144838}{6237} \\
  & & & & & \frac{601676}{22275} \\
  \end{pmatrix}

Here :math:`\phi'` is the conformal latitude (sometimes denoted by
:math:`\chi`) and :math:`\mathsf C_{\chi,\phi}` and :math:`\mathsf
C_{\phi,\chi}` are the coefficients in the trigonometric series for
converting between :math:`\phi` and :math:`\chi`.

Relation between projected coordinates
======================================

.. math::

  \zeta = \zeta' + \mathbf S(\zeta') \cdot \mathsf C_{\mu,\chi} \cdot \mathbf N

.. math::

  \zeta' = \zeta + \mathbf S(\zeta) \cdot \mathsf C_{\chi,\mu} \cdot \mathbf N

.. math::

  \mathsf C_{\mu,\chi} = \begin{pmatrix}
  \frac{1}{2}& -\frac{2}{3}& \frac{5}{16}& \frac{41}{180}& -\frac{127}{288}& \frac{7891}{37800} \\
  & \frac{13}{48}& -\frac{3}{5}& \frac{557}{1440}& \frac{281}{630}& -\frac{1983433}{1935360} \\
  & & \frac{61}{240}& -\frac{103}{140}& \frac{15061}{26880}& \frac{167603}{181440} \\
  & & & \frac{49561}{161280}& -\frac{179}{168}& \frac{6601661}{7257600} \\
  & & & & \frac{34729}{80640}& -\frac{3418889}{1995840} \\
  & & & & & \frac{212378941}{319334400} \\
  \end{pmatrix}

.. math::

  \mathsf C_{\chi,\mu} = \begin{pmatrix}
  -\frac{1}{2}& \frac{2}{3}& -\frac{37}{96}& \frac{1}{360}& \frac{81}{512}& -\frac{96199}{604800} \\
  & -\frac{1}{48}& -\frac{1}{15}& \frac{437}{1440}& -\frac{46}{105}& \frac{1118711}{3870720} \\
  & & -\frac{17}{480}& \frac{37}{840}& \frac{209}{4480}& -\frac{5569}{90720} \\
  & & & -\frac{4397}{161280}& \frac{11}{504}& \frac{830251}{7257600} \\
  & & & & -\frac{4583}{161280}& \frac{108847}{3991680} \\
  & & & & & -\frac{20648693}{638668800} \\
  \end{pmatrix}

On the central meridian (:math:`\lambda = \lambda' = 0`), :math:`\zeta'
= \phi'` is the conformal latitude :math:`\chi` and :math:`\zeta` plays
the role of the rectifying latitude (sometimes denoted by :math:`\mu`).
:math:`\mathsf C_{\mu,\chi}` and :math:`\mathsf C_{\chi,\mu}` are the
coefficients in the trigonometric series for converting between
:math:`\chi` and :math:`\mu`.

Legacy ellipsoidal form
***********************

The formulas below describe the algorithm used when giving the
:option:`+approx` option. They are originally from :cite:`Snyder1987`,
but here quoted from :cite:`Evenden1995` and :cite:`Evenden2005`.  These
are less accurate that the formulation above and are only valid within
about 5 degrees of the central meridian.  Here :math:`M(\phi)` is the
meridional distance.


Forward projection
==================

.. math::

  N = \frac{k_0}{(1 - e^2 \sin^2\phi)^{1/2}}

.. math::

  R = \frac{k_0(1-e^2)}{(1-e^2 \sin^2\phi)^{3/2}}

.. math::

  t = \tan\phi

.. math::

  \eta = \frac{e^2}{1-e^2} \cos^2\phi

.. math::

  x &= k_0 \lambda \cos \phi \\
    &+ \frac{k_0 \lambda^3 \cos^3\phi}{3!}(1-t^2+\eta^2) \\
    &+ \frac{k_0 \lambda^5 \cos^5\phi}{5!}(5-18t^2+t^4+14\eta^2-58t^2\eta^2) \\
    &+\frac{k_0 \lambda^7 \cos^7\phi}{7!}(61-479t^2+179t^4-t^6)

.. math::

  y &= M(\phi) \\
    &+ \frac{k_0 \lambda^2 \sin\phi \cos \phi}{2!} \\
    &+ \frac{k_0 \lambda^4 \sin\phi \cos^3\phi}{4!}(5-t^2+9\eta^2+4\eta^4) \\
    &+ \frac{k_0 \lambda^6 \sin\phi \cos^5\phi}{6!}(61-58t^2+t^4+270\eta^2-330t^2\eta^2) \\
    &+ \frac{k_0 \lambda^8 \sin\phi \cos^7\phi}{8!}(1385-3111t^2+543t^4-t^6)

Inverse projection
==================

.. math::

  \phi_1 = M^{-1}(y)

.. math::

  N_1 = \frac{k_0}{1 - e^2 \sin^2\phi_1)^{1/2}}

.. math::

  R_1 = \frac{k_0(1-e^2)}{(1-e^2 \sin^2\phi_1)^{3/2}}

.. math::

  t_1 = \tan(\phi_1)

.. math::

  \eta_1 = \frac{e^2}{1-e^2} \cos^2\phi_1

.. math::

  \phi &= \phi_1 \\
       &- \frac{t_1 x^2}{2! R_1 N_1} \\
       &+ \frac{t_1 x^4}{4! R_1 N_1^3}(5+3t_1^2+\eta_1^2-4\eta_1^4-9\eta_1^2t_1^2) \\
       &- \frac{t_1 x^6}{6! R_1 N_1^5}(61+90t_1^2+46\eta_1^2+45t_1^4-252t_1^2\eta_1^2) \\
       &+ \frac{t_1 x^8}{8! R_1 N_1^7}(1385+3633t_1^2+4095t_1^4+1575t_1^6)

.. math::

  \lambda &= \frac{x}{\cos \phi N_1} \\
          &- \frac{x^3}{3! \cos \phi N_1^3}(1+2t_1^2+\eta_1^2) \\
          &+ \frac{x^5}{5! \cos \phi N_1^5}(5+6\eta_1^2+28t_1^2-3\eta_1^2+8t_1^2\eta_1^2) \\
          &- \frac{x^7}{7! \cos \phi N_1^7}(61+662t_1^2+1320t_1^4+720t_1^6)

Further reading
###############

#. `Wikipedia <https://en.wikipedia.org/wiki/Transverse_Mercator_projection>`_
