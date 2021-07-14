.. _hatano:

********************************************************************************
Hatano Asymmetrical Equal Area
********************************************************************************




+---------------------+----------------------------------------------------------+
| **Classification**  | :term:`Pseudocylindrical Projection`                     |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical projection                |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | hatano                                                   |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+



.. figure:: ./images/hatano.png
   :width: 500 px
   :align: center
   :alt:   Hatano Asymmetrical Equal Area

   proj-string: ``+proj=hatano``

Parameters
################################################################################

.. note:: All parameters for the projection are optional.

.. include:: ../options/lon_0.rst

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst


Mathematical Definition
--------------------------------------------------------------------------------

Forward
................................................................................

.. math::

   x &= 0.85\lambda \cos \theta

   y &= C_y \sin \theta

   P(\theta) &= 2\theta + \sin 2\theta - C_p \sin \phi

   P'(\theta) &= 2(1 + \cos 2\theta)

   \theta_0 &= 2\phi


====================================  ==================  ===================
Condition                                    :math:`C_y`          :math:`C_p`
====================================  ==================  ===================
For :math:`\phi > 0`                      1.75859              2.67595
For :math:`\phi < 0`                      1.93052              2.43763
====================================  ==================  ===================

For :math:`\phi = 0`, :math:`y \leftarrow 0`, and :math:`x \leftarrow 0.85\lambda`.

Further reading
--------------------------------------------------------------------------------

#. `Compare Map Projections <http://map-projections.net/single-view/hatano>`__
#. `Mathworks <http://www.mathworks.com/help/map/hatano.html>`__



