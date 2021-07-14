.. _set:

================================================================================
Set coordinate value
================================================================================

.. versionadded:: 7.0.0

Set component(s) of a coordinate to a fixed value.

+---------------------+--------------------------------------------------------+
| **Alias**           | set                                                    |
+---------------------+--------------------------------------------------------+
| **Domain**          | 4D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Any                                                    |
+---------------------+--------------------------------------------------------+
| **Output type**     | Any                                                    |
+---------------------+--------------------------------------------------------+

This operations allows for components of coordinates to be set to a fixed value.
This may be useful in :ref:`pipeline<pipeline>` when a step requires some
component, typically an elevation or a date, to be set to a fixed value.

Example
################################################################################

In the ETRS89 to Dutch RD with NAP height transformation, the used ellipsoidal
height for the Helmert transformation is not the NAP height, but the height is
set to 0 m. This is an unconventional trick to get the same results as when the
effect of the Helmert transformation is included in the horizontal NTv2 grid.
For the forward transformation from ETRS89 to RD with NAP height, we need to set
the ellipsoidal ETRS89 height for the Helmert transformation to the equivalent
of 0 m NAP. This is 43 m for the centre of the Netherlands and this value can
be used as an approximation elsewhere (the effect of this approximation is
below 1 mm for the horizontal coordinates, in an area up to hundreds of km
outside the Netherlands).

The ``+proj=set +v_3=0`` close to the end of the pipeline is to make it usable in
the reverse direction.

::

   $ cct -t 0 -d 4 +proj=pipeline \
        +step +proj=unitconvert +xy_in=deg +xy_out=rad \
        +step +proj=axisswap +order=2,1 \
        +step +proj=vgridshift +grids=nlgeo2018.gtx \
        +step +proj=push +v_3 \
        +step +proj=set +v_3=43 \
        +step +proj=cart +ellps=GRS80 \
        +step +proj=helmert +x=-565.7346 +y=-50.4058 +z=-465.2895 +rx=-0.395023 +ry=0.330776 +rz=-1.876073 +s=-4.07242 +convention=coordinate_frame +exact \
        +step +proj=cart +inv +ellps=bessel \
        +step +proj=hgridshift +inv +grids=rdcorr2018.gsb,null \
        +step +proj=sterea +lat_0=52.156160556 +lon_0=5.387638889 +k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel \
        +step +proj=set +v_3=0 \
        +step +proj=pop +v_3

Parameters
################################################################################

.. option:: +v_1=value

   Set the first coordinate component to the specified value

.. option:: +v_2=value

   Set the second coordinate component to the specified value

.. option:: +v_3=value

   Set the third coordinate component to the specified value

.. option:: +v_4=value

   Set the fourth coordinate component to the specified value

