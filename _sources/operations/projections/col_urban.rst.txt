.. _col_urban:

********************************************************************************
Colombia Urban
********************************************************************************

.. versionadded:: 7.2

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse ellipsoidal projection               |
+---------------------+----------------------------------------------------------+
| **Alias**           | col_urban                                                |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+

From :cite:`IOGP2018`:

    The capital cites of each department in Colombia use an urban projection for large
    scale topographic mapping of the urban areas. It is based on a plane through
    the origin at an average height for the area, eliminating the need for corrections
    to engineering survey observations.

   proj-string: ``+proj=col_urban``

Parameters
################################################################################

.. include:: ../options/lon_0.rst

.. include:: ../options/lat_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. option:: +h_0=<value>

    Projection plane origin height (in metre)

    *Defaults to 0.0.*
