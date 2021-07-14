.. _cea:

********************************************************************************
Equal Area Cylindrical
********************************************************************************

+---------------------+----------------------------------------------------------+
| **Classification**  | Cylindrical                                              |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and ellipsoidal           |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | cea                                                      |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/cea.png
   :width: 500 px
   :align: center
   :alt:   Equal Area Cylindrical

   proj-string: ``+proj=cea``

Parameters
################################################################################

.. note:: All parameters are optional for the Equal Area Cylindrical projection.

.. include:: ../options/lat_ts.rst

.. include:: ../options/lon_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst

.. include:: ../options/k_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. note::

    ``lat_ts`` and ``k_0`` are mutually exclusive. If ``lat_ts``
    is specified, it is equivalent to setting ``k_0`` to
    :math:`\frac{\cos \phi_{ts}}{\sqrt{1 - e^2 \sin^2 \phi_{ts}}}`