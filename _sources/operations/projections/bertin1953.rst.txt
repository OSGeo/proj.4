.. _bertin1953:

********************************************************************************
Bertin 1953
********************************************************************************

.. versionadded:: 6.0.0

+---------------------+-------------------------------+
| **Classification**  | Miscellaneous                 |
+---------------------+-------------------------------+
| **Available forms** | Forward, spherical projection |
+---------------------+-------------------------------+
| **Defined area**    | Global                        |
+---------------------+-------------------------------+
| **Alias**           | bertin1953                    |
+---------------------+-------------------------------+
| **Domain**          | 2D                            |
+---------------------+-------------------------------+
| **Input type**      | Geodetic coordinates          |
+---------------------+-------------------------------+
| **Output type**     | Projected coordinates         |
+---------------------+-------------------------------+



.. figure:: ./images/bertin1953.png
   :width: 500 px
   :align: center
   :alt:   Bertin 1953

   proj-string: ``+proj=bertin1953``


The Bertin 1953 projection is intended for making world maps. Created by Jacques Bertin
in 1953, this projection was the go-to choice of the French cartographic school when they
wished to represent phenomena on a global scale. The projection was formulated in 2017
by Philippe Rivière for visionscarto.net.


Usage
###############################################################################

The Bertin 1953 projection has no special options. Its rotation parameters
are fixed. Here is an example of a forward projection with scale 1:

    $ echo 122 47 | src/proj +proj=bertin1953 +R=1
    0.72    0.73

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

Further reading
################################################################################

#. Philippe Rivière (2017). `Bertin Projection (1953) <https://visionscarto.net/bertin-projection-1953>`, Visionscarto.net.
