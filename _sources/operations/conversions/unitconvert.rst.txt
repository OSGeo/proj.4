.. _unitconvert:

================================================================================
Unit conversion
================================================================================

.. versionadded:: 5.0.0

Convert between various distance, angular and time units.

+---------------------+--------------------------------------------------------+
| **Alias**           | unitconvert                                            |
+---------------------+--------------------------------------------------------+
| **Domain**          | 2D, 3D or 4D                                           |
+---------------------+--------------------------------------------------------+
| **Input type**      | Any                                                    |
+---------------------+--------------------------------------------------------+
| **Output type**     | Any                                                    |
+---------------------+--------------------------------------------------------+

There are many examples of coordinate reference systems that are expressed in
other units than the meter. There are also many cases where temporal data
has to be translated to different units. The `unitconvert` operation takes care
of that.

Many North American systems are defined with coordinates in feet. For example
in Vermont::

    +proj=pipeline
    +step +proj=tmerc +lat_0=42.5 +lon_0=-72.5 +k_0=0.999964286 +x_0=500000.00001016 +y_0=0
    +step +proj=unitconvert +xy_in=m +xy_out=us-ft

Often when working with GNSS data the timestamps are presented in GPS-weeks,
but when the data transformed with the `helmert` operation timestamps are
expected to be in units of decimalyears. This can be fixed with `unitconvert`::

    +proj=pipeline
    +step +proj=unitconvert +t_in=gps_week +t_out=decimalyear
    +step +proj=helmert +epoch=2000.0 +t_obs=2017.5 ...

Parameters
################################################################################

.. option:: +xy_in=<unit> or <conversion_factor>

    Horizontal input units. See :ref:`distance_units` and :ref:`angular_units`
    for a list of available units. `<conversion_factor>` is the conversion factor
    from the input unit to metre for linear units, or to radian for angular
    units.

.. option:: +xy_out=<unit> or <conversion_factor>

    Horizontal output units. See :ref:`distance_units` and :ref:`angular_units`
    for a list of available units. `<conversion_factor>` is the conversion factor
    from the output unit to metre for linear units, or to radian for angular
    units.

.. option:: +z_in=<unit> or <conversion_factor>

    Vertical output units. See :ref:`distance_units` and :ref:`angular_units`
    for a list of available units. `<conversion_factor>` is the conversion factor
    from the input unit to metre for linear units, or to radian for angular
    units.

.. option:: +z_out=<unit> or <conversion_factor>

    Vertical output units. See :ref:`distance_units` and :ref:`angular_units`
    for a list of available units.  `<conversion_factor>` is the conversion factor
    from the output unit to metre for linear units, or to radian for angular
    units.

.. option:: +t_in=<unit>

    Temporal input units. See :ref:`time_units` for a list of available units.

.. option:: +t_out=<unit>

    Temporal output units. See :ref:`time_units` for a list of available units.

.. _distance_units:

Distance units
###############################################################################

In the table below all distance units supported by PROJ are listed.
The same list can also be produced on the command line with :program:`proj` or
:program:`cs2cs`, by adding the `-lu` flag when calling the utility.

+----------+---------------------------------+
| Label    | Name                            |
+==========+=================================+
|     km   | Kilometer                       |
+----------+---------------------------------+
|      m   | Meter                           |
+----------+---------------------------------+
|     dm   | Decimeter                       |
+----------+---------------------------------+
|     cm   | Centimeter                      |
+----------+---------------------------------+
|     mm   | Millimeter                      |
+----------+---------------------------------+
|    kmi   | International Nautical Mile     |
+----------+---------------------------------+
|     in   | International Inch              |
+----------+---------------------------------+
|     ft   | International Foot              |
+----------+---------------------------------+
|     yd   | International Yard              |
+----------+---------------------------------+
|     mi   | International Statute Mile      |
+----------+---------------------------------+
|   fath   | International Fathom            |
+----------+---------------------------------+
|     ch   | International Chain             |
+----------+---------------------------------+
|   link   | International Link              |
+----------+---------------------------------+
|  us-in   | U.S. Surveyor's Inch            |
+----------+---------------------------------+
|  us-ft   | U.S. Surveyor's Foot            |
+----------+---------------------------------+
|  us-yd   | U.S. Surveyor's Yard            |
+----------+---------------------------------+
|  us-ch   | U.S. Surveyor's Chain           |
+----------+---------------------------------+
|  us-mi   | U.S. Surveyor's Statute Mile    |
+----------+---------------------------------+
| ind-yd   | Indian Yard                     |
+----------+---------------------------------+
| ind-ft   | Indian Foot                     |
+----------+---------------------------------+
| ind-ch   | Indian Chain                    |
+----------+---------------------------------+

.. _angular_units:

Angular units
###############################################################################

.. versionadded:: 5.2.0

In the table below all angular units supported by PROJ `unitconvert` are listed.

+----------+---------------------------------+
| Label    | Name                            |
+==========+=================================+
|    deg   | Degree                          |
+----------+---------------------------------+
|   grad   | Grad                            |
+----------+---------------------------------+
|    rad   | Radian                          |
+----------+---------------------------------+

.. _time_units:

Time units
###############################################################################

In the table below all time units supported by PROJ are listed.

+--------------+-----------------------------+
| Label        | Name                        |
+==============+=============================+
| mjd          | Modified Julian date        |
+--------------+-----------------------------+
| decimalyear  | Decimal year                |
+--------------+-----------------------------+
| gps_week     | GPS Week                    |
+--------------+-----------------------------+
| yyyymmdd     | Date in yyyymmdd format     |
+--------------+-----------------------------+

