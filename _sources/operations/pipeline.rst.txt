.. _pipeline:

================================================================================
The pipeline operator
================================================================================

.. versionadded:: 5.0.0

Construct complex operations by daisy-chaining operations in a sequential pipeline.

+-----------------+--------------------------------------------------------------------+
| **Alias**       | pipeline                                                           |
+-----------------+--------------------------------------------------------------------+
| **Domain**      | 2D, 3D and 4D                                                      |
+-----------------+--------------------------------------------------------------------+
| **Input type**  | Any                                                                |
+-----------------+--------------------------------------------------------------------+
| **Output type** | Any                                                                |
+-----------------+--------------------------------------------------------------------+

.. note:: See the section on :ref:`transformation` for a more thorough introduction
          to the concept of transformation pipelines in PROJ.


With the pipeline operation it is possible to perform several operations after each
other on the same input data. This feature makes it possible to create transformations
that are made up of more than one operation, e.g. performing a datum shift and then
applying a suitable map projection. Theoretically any transformation between two
coordinate reference systems is possible to perform using the pipeline operation,
provided that the necessary coordinate operations in each step is available in PROJ.

A pipeline is made up of a number of steps, with each step being a coordinate operation
in itself. By connecting these individual steps sequentially we end up with a concatenated
coordinate operation. An example of this is a transformation from geodetic coordinates
on the GRS80 ellipsoid to a projected system where the east-west and north-east axes has
been swapped:

::

    +proj=pipeline +ellps=GRS80 +step +proj=merc +step +proj=axisswap +order=2,1

Here the first step is applying the :ref:`merc` projection and the second step is
applying the :ref:`axisswap` conversion. Note that the `+ellps=GRS80` is specified
before the first occurrence of `+step`. This means that the GRS80 ellipsoid is used
in both steps, since any parameter stated before the first occurrence of `+step` is
treated as a global parameter and is transferred to each individual steps.


Rules for pipelines
-------------------------------------------------------------------------------

**1. Pipelines must consist of at least one step.**

::

    +proj=pipeline

Will result in an error.

**2. Pipelines can only be nested if the nested pipeline is defined in an init file.**

::

    +proj=pipeline
    +step +proj=pipeline +step +proj=merc +step +proj=axisswap +order=2,1
    +step +proj=unitconvert +xy_in=m +xy_out=us-ft

Results in an error, while

::

    +proj=pipeline
    +step +init=predefined_pipelines:projectandswap
    +step +proj=unitconvert +xy_in=m +xy_out=us-ft

does not.

**3. Pipelines without a forward path can't be constructed.**

::

    +proj=pipeline +step +inv +proj=urm5

Will result in an error since :ref:`urm5` does not have an inverse operation defined.

.. _global-pipeline-parameter:

**4. Parameters added before the first `+step` are global and will be applied to all steps.**

In the following the GRS80 ellipsoid will be applied to all steps.

::

    +proj=pipeline +ellps=GRS80
    +step +proj=cart
    +step +proj=helmert +x=10 +y=3 +z=1
    +step +proj=cart +inv
    +step +proj=merc

**5. Units of operations must match between steps.**

.. versionadded:: 5.1.0

The output units of step *n* must match the expected input unit of step *n+1*. E.g., you can't
pass an operation that outputs projected coordinates to an operation that expects angular units
(degrees). An example of such a unit mismatch is displayed below.

::

    +proj=pipeline
    +step +proj=merc  # Mercator outputs projected coordinates
    +step +proj=robin # The Robinson projection expects angular input

Parameters
-------------------------------------------------------------------------------

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +step

    Separate each step in a pipeline.


Optional
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +inv

    Invert a step in a pipeline.

.. option:: +omit_fwd

    .. versionadded:: 6.3.0

    Skip a step of the pipeline when it is followed in the forward path.

    The following example shows a combined use of :ref:`push <push>` and :ref:`pop <pop>` operators,
    with ``omit_fwd`` and ``omit_inv`` options, to implement a vertical adjustment that must
    be done in a interpolation CRS that is different from the horizontal CRS
    used in input and output. +omit_fwd in the forward path avoid a useless
    inverse horizontal transformation and relies on the pop operator to restore
    initial horizontal coordinates. +omit_inv serves the similar purpose when
    the pipeline is executed in the reverse direction

    ::

        +proj=pipeline
        +step +proj=unitconvert +xy_in=deg +xy_out=rad
        +step +proj=push +v_1 +v_2
        +step +proj=hgridshift +grids=nvhpgn.gsb +omit_inv
        +step +proj=vgridshift +grids=g1999u05.gtx +multiplier=1
        +step +inv +proj=hgridshift +grids=nvhpgn.gsb +omit_fwd
        +step +proj=pop +v_1 +v_2
        +step +proj=unitconvert +xy_in=rad +xy_out=deg

.. option:: +omit_inv

    .. versionadded:: 6.3.0

    Skip a step of the pipeline when it is followed in the reverse path.
