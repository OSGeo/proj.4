.. _sch:

********************************************************************************
Spherical Cross-track Height
********************************************************************************
+---------------------+--------------------------------------------------------+
| **Classification**  | Miscellaneous                                          |
+---------------------+--------------------------------------------------------+
| **Available forms** | Forward and inverse.                                   |
+---------------------+--------------------------------------------------------+
| **Defined area**    | Global                                                 |
+---------------------+--------------------------------------------------------+
| **Alias**           | sch                                                    |
+---------------------+--------------------------------------------------------+
| **Domain**          | 3D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | 3D coordinates                                         |
+---------------------+--------------------------------------------------------+
| **Output type**     | Projected coordinates                                  |
+---------------------+--------------------------------------------------------+

   proj-string: ``+proj=sch +plat_0=XX +plon_0=XX +phdg_0=XX``

The SCH coordinate system is a sensor aligned coordinate system developed at
JPL (Jet Propulsion Laboratory) for radar mapping missions.

See :cite:`Hensley2002`


Parameters
################################################################################

Required
-------------------------------------------------------------------------------

.. option:: +plat_0=<value>

    Peg latitude (in degree)

.. option:: +plon_0=<value>

    Peg longitude (in degree)

.. option:: +phdg_0=<value>

    Peg heading (in degree)

Optional
-------------------------------------------------------------------------------

.. option:: +h_0=<value>

    Average height (in metre)

    *Defaults to 0.0.*

.. include:: ../options/lon_0.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst

.. include:: ../options/ellps.rst

.. include:: ../options/R.rst
