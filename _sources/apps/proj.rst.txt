.. _proj:

================================================================================
proj
================================================================================

.. only:: html

    Cartographic projection filter.

.. Index:: proj

Synopsis
********
    **proj** [**-beEfiIlmorsStTvVwW**] [args]] [*+opt[=arg]* ...] file ...

    **invproj** [**-beEfiIlmorsStTvVwW**] [args]] [*+opt[=arg]* ...] file ...


Description
***********
:program:`proj` and :program:`invproj` perform respective forward and inverse
conversion of cartographic data to or from cartesian data with a wide
range of selectable projection functions.

:program:`invproj` may not be available on all platforms; in this case
use :option:`proj -I` instead.

The following control parameters can appear in any order

.. program:: proj

.. option:: -b

    Special option for binary coordinate data input and output through standard
    input and standard output. Data is assumed to be in system type double
    floating point words. This option is to be used when :program:`proj` is a child process
    and allows bypassing formatting operations.

.. option:: -d <n>

.. versionadded:: 5.2.0

    Specify the number of decimals in the output.

.. option:: -i

    Selects binary input only (see :option:`-b`).

.. option:: -I

    Alternate method to specify inverse projection. Redundant when used with
    :program:`invproj`.

.. option:: -o

    Selects binary output only (see :option:`-b`).

.. option:: -t<a>

    Where *a* specifies a character employed as the first character to denote a
    control line to be passed through without processing. This option
    applicable to ASCII input only. (# is the default value).

.. option:: -e <string>

    Where *string* is an arbitrary string to be output if an error is detected during
    data transformations. The default value is a three character string: ``*\t*``.
    Note that if the :option:`-b`, :option:`-i` or :option:`-o` options are employed, an error
    is returned as HUGE_VAL value for both return values.

.. option:: -E

    Causes the input coordinates to be copied to the output line prior to
    printing the converted values.

.. option:: -l<[=id]>

    List projection identifiers that can be selected with *+proj*. ``proj -l=id``
    gives expanded description of projection *id*, e.g. ``proj -l=merc``.

.. option:: -lp

    List of all projection id that can be used with the *+proj* parameter.
    Equivalent to ``proj -l``.

.. option:: -lP

    Expanded description of all projections that can be used with the *+proj*
    parameter.

.. option:: -le

    List of all ellipsoids that can be selected with the *+ellps* parameters.

.. option:: -lu

    List of all distance units that can be selected with the *+units* parameter.

.. option:: -r

    This options reverses the order of the expected input from
    longitude-latitude or x-y to latitude-longitude or y-x.

.. option:: -s

    This options reverses the order of the output from x-y or longitude-latitude
    to y-x or latitude-longitude.

.. option:: -S

    Causes estimation of meridional and parallel scale factors, area scale
    factor and angular distortion, and maximum and minimum scale factors to be
    listed between <> for each input point. For conformal projections meridional
    and parallel scales factors will be equal and angular distortion zero. Equal
    area projections will have an area factor of 1.

.. option:: -m <mult>

    The cartesian data may be scaled by the *mult* parameter. When processing data
    in a forward projection mode the cartesian output values are multiplied by
    *mult* otherwise the input cartesian values are divided by *mult* before inverse
    projection. If the first two characters of *mult* are 1/ or 1: then the
    reciprocal value of *mult* is employed.

.. option:: -f <format>

    Where *format* is a printf format string to control the form of the output values.
    For inverse projections, the output will be in degrees when this option is
    employed. The default format is ``"%.2f"`` for forward projection and DMS for
    inverse.

.. option:: -w<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output (when the option is not specified, ``-w3`` is assumed).

.. option:: -W<n>

    Where *n* is the number of significant fractional digits to employ for seconds
    output. When ``-W`` is employed the fields will be constant width
    with leading zeroes.

.. option:: -v

    Causes a listing of cartographic control parameters tested for and used by
    the program to be printed prior to input data.

.. option:: -V

    This option causes an expanded annotated listing of the characteristics of
    the projected point. :option:`-v` is implied with this option.


The *+opt* run-line arguments are associated with cartographic parameters.
Additional projection control parameters may be contained in two auxiliary
control files: the first is optionally referenced with the
*+init=file:id* and the second is always processed after the name of the
projection has been established from either the run-line or the contents of
+init file. The environment parameter :envvar:`PROJ_LIB` establishes the
default directory for a file reference without an absolute path. This is
also used for supporting files like datum shift files.

.. only:: html

    Usage of *+opt* varies with projection and for a complete description
    consult the :ref:`projection pages <projections>`.


One or more files (processed in left to right order) specify the source of
data to be converted. A ``-`` will specify the location of processing standard
input. If no files are specified, the input is assumed to be from stdin.
For ASCII input data the two data values must be in the first two white space
separated fields and when both input and output are ASCII all trailing
portions of the input line are appended to the output line.

Input geographic data (longitude and latitude) must be in DMS or decimal degrees format and input
cartesian data must be in units consistent with the ellipsoid major axis or
sphere radius units. Output geographic coordinates will be in DMS (if the
``-w`` switch is not employed) and precise to 0.001" with trailing, zero-valued
minute-second fields deleted.

Example
*******
The following script

.. code-block:: console

    proj +proj=utm +lon_0=112w +ellps=clrk66 -r <<EOF
    45d15'33.1" 111.5W
    45d15.551666667N -111d30
    +45.25919444444 111d30'000w
    EOF

will perform UTM forward projection with a standard UTM central meridian
nearest longitude 112W. The geographic values of this example are equivalent
and meant as examples of various forms of DMS input. The x-y output
data will appear as three lines of::

    460769.27     5011648.45

.. only:: man

    Other programs
    **************

    The :program:`proj` program is limited to converting between geographic and
    projected coordinates within one datum.

    The :program:`cs2cs` program operates similarly, but allows translation
    between any pair of definable coordinate reference systems, including
    support for datum translation.

    See also
    ********

    **cs2cs(1)**, **cct(1)**, **geod(1)**, **gie(1)**, **projinfo(1)**, **projsync(1)**

    .. include:: common_man.rst
