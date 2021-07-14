.. _horner:

================================================================================
Horner polynomial evaluation
================================================================================

.. versionadded:: 5.0.0

+-----------------+-------------------------------------------------------------------+
| **Alias**       | horner                                                            |
+-----------------+-------------------------------------------------------------------+
| **Domain**      | 2D and 3D                                                         |
+-----------------+-------------------------------------------------------------------+
| **Input type**  | Geodetic and projected coordinates                                |
+-----------------+-------------------------------------------------------------------+
| **Output type** | Geodetic and projected coordinates                                |
+-----------------+-------------------------------------------------------------------+

The Horner polynomial evaluation scheme is used for transformations between reference
frames where one or both are inhomogeneous or internally distorted. This will typically
be reference frames created before the introduction of space geodetic techniques such
as GPS.

Horner polynomials, or Multiple Regression Equations as they are also known as, have
their strength in being able to create complicated mappings between coordinate
reference frames while still being lightweight in both computational cost and disk
space used.

PROJ implements two versions of the Horner evaluation scheme: Real and complex
polynomial evaluation. Below both are briefly described. For more details consult
:cite:`Ruffhead2016` and :cite:`IOGP2018`.

The polynomial evaluation in real number space is defined by the following equations:

.. math::
    :label: real_poly

    \Delta X = \sum_{i,j} u_{i,j} U^i V^j

    \Delta Y = \sum_{i,j} v_{i,j} U^i V^j

where

.. math::
    :label: UV

    U = X_{in} - X_{origin}

    V = Y_{in} - Y_{origin}

and :math:`u_{i,j}` and :math:`v_{i,j}` are coefficients that make up
the polynomial.

The final coordinates are determined as

.. math::
    :label: xy_out

    X_{out} = X_{in} + \Delta X

    Y_{out} = Y_{in} + \Delta Y

The inverse transform is the same as the above but requires a different set of
coefficients.

Evaluation of the complex polynomials are defined by the following equations:

.. math::
    :label: complex_poly

    \Delta X + i \Delta Y = \sum_{j=1}^n (c_{2j-1} + i c_{2j}) (U + i V)^j

Where :math:`n` is the degree of the polynomial. :math:`U` and :math:`V` are
defined as in :eq:`UV` and the resulting coordinates are again determined
by :eq:`xy_out`.

Examples
################################################################################

Mapping between Danish TC32 and ETRS89/UTM zone 32 using polynomials in real
number space::

    +proj=horner
    +ellps=intl
    +range=500000
    +fwd_origin=877605.269066,6125810.306769
    +inv_origin=877605.760036,6125811.281773
    +deg=4
    +fwd_v=6.1258112678e+06,9.9999971567e-01,1.5372750011e-10,5.9300860915e-15,2.2609497633e-19,4.3188227445e-05,2.8225130416e-10,7.8740007114e-16,-1.7453997279e-19,1.6877465415e-10,-1.1234649773e-14,-1.7042333358e-18,-7.9303467953e-15,-5.2906832535e-19,3.9984284847e-19
    +fwd_u=8.7760574982e+05,9.9999752475e-01,2.8817299305e-10,5.5641310680e-15,-1.5544700949e-18,-4.1357045890e-05,4.2106213519e-11,2.8525551629e-14,-1.9107771273e-18,3.3615590093e-10,2.4380247154e-14,-2.0241230315e-18,1.2429019719e-15,5.3886155968e-19,-1.0167505000e-18
    +inv_v=6.1258103208e+06,1.0000002826e+00,-1.5372762184e-10,-5.9304261011e-15,-2.2612705361e-19,-4.3188331419e-05,-2.8225549995e-10,-7.8529116371e-16,1.7476576773e-19,-1.6875687989e-10,1.1236475299e-14,1.7042518057e-18,7.9300735257e-15,5.2881862699e-19,-3.9990736798e-19
    +inv_u=8.7760527928e+05,1.0000024735e+00,-2.8817540032e-10,-5.5627059451e-15,1.5543637570e-18,4.1357152105e-05,-4.2114813612e-11,-2.8523713454e-14,1.9109017837e-18,-3.3616407783e-10,-2.4382678126e-14,2.0245020199e-18,-1.2441377565e-15,-5.3885232238e-19,1.0167203661e-18


Mapping between Danish System Storebælt and ETRS89/UTM zone 32 using complex
polynomials::

    +proj=horner
    +ellps=intl
    +range=500000
    +fwd_origin=4.94690026817276e+05,6.13342113183056e+06
    +inv_origin=6.19480258923588e+05,6.13258568148837e+06
    +deg=3
    +fwd_c=6.13258562111350e+06,6.19480105709997e+05,9.99378966275206e-01,-2.82153291753490e-02,-2.27089979140026e-10,-1.77019590701470e-09,1.08522286274070e-14,2.11430298751604e-15
    +inv_c=6.13342118787027e+06,4.94690181709311e+05,9.99824464710368e-01,2.82279070814774e-02,7.66123542220864e-11,1.78425334628927e-09,-1.05584823306400e-14,-3.32554258683744e-15

Parameters
################################################################################

Setting up Horner polynomials requires many coefficients being explicitly
written, even for polynomials of low degree. For this reason it is recommended
to store the polynomial definitions in an :ref:`init file <init_files>` for
easier writing and reuse.

Required
-------------------------------------------------------------------------------

Below is a list of required parameters that can be set for the Horner polynomial
transformation. As stated above, the transformation takes to forms, either using
real or complex polynomials. These are divided into separate sections below.
Parameters from the two sections are mutually exclusive, that is parameters
describing real and complex polynomials can't be mixed.

.. include:: ../options/ellps.rst

.. option:: +deg=<value>

    Degree of polynomial

.. option:: +fwd_origin=<northing,easting>

    Coordinate of origin for the forward mapping

.. option:: +inv_origin=<northing,easting>

    Coordinate of origin for the inverse mapping

Real polynomials
..............................................................................

The following parameters has to be set if the transformation consists of
polynomials in real space. Each parameter takes a comma-separated list of
coefficients. The number of coefficients is governed by the degree, :math:`d`,
of the polynomial:

.. math::

    N = \frac{(d + 1)(d + 2)}{2}

.. option:: +fwd_u=<u_11,u_12,...,u_ij,..,u_mn>

    Coefficients for the forward transformation i.e. latitude to northing
    as described in :eq:`real_poly`.

.. option:: +fwd_v=<v_11,v_12,...,v_ij,..,v_mn>

    Coefficients for the forward transformation i.e. longitude to easting
    as described in :eq:`real_poly`.

.. option:: +inv_u=<u_11,u_12,...,u_ij,..,u_mn>

    Coefficients for the inverse transformation i.e. latitude to northing
    as described in :eq:`real_poly`.

.. option:: +inv_v=<v_11,v_12,...,v_ij,..,v_mn>

    Coefficients for the inverse transformation i.e. longitude to easting
    as described in :eq:`real_poly`.



Complex polynomials
..............................................................................

The following parameters has to be set if the transformation consists of
polynomials in complex space. Each parameter takes a comma-separated list of
coefficients. The number of coefficients is governed by the degree, :math:`d`,
of the polynomial:

.. math::

    N = 2d + 2

.. option:: +fwd_c=<c_1,c_2,...,c_N>

    Coefficients for the complex forward transformation
    as described in :eq:`complex_poly`.

.. option:: +inv_c=<c_1,c_2,...,c_N>

    Coefficients for the complex inverse transformation
    as described in :eq:`complex_poly`.

Optional
-------------------------------------------------------------------------------

.. option:: +range=<value>

    Radius of the region of validity.

.. option:: +uneg

    Express latitude as southing. Only applies for complex polynomials.

.. option:: +vneg

    Express longitude as westing. Only applies for complex polynomials.


Further reading
################################################################################

#. `Wikipedia <https://en.wikipedia.org/wiki/Horner%27s_method>`_
