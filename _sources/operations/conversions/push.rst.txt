.. _push:

================================================================================
Push coordinate value to pipeline stack
================================================================================

.. versionadded:: 6.0.0

Save components of a coordinate from one step of a pipeline and make it
available for retrieving in another pipeline step.

+---------------------+--------------------------------------------------------+
| **Alias**           | push                                                   |
+---------------------+--------------------------------------------------------+
| **Domain**          | 4D                                                     |
+---------------------+--------------------------------------------------------+
| **Input type**      | Any                                                    |
+---------------------+--------------------------------------------------------+
| **Output type**     | Any                                                    |
+---------------------+--------------------------------------------------------+

This operations allows for components of coordinates to be saved for
application in a later step. A saved coordinate component is moved, or
*pushed*, to a memory stack that is part of a :ref:`pipeline<pipeline>`. The
pipeline coordinate stack is inspired by the stack data structure that is
commonly used in computer science. There's four stacks available: One four each
coordinate dimension. The dimensions, or coordinate components, are numbered
1--4. It is only possible to move data to and from the stack within the same
coordinate component number. Values can be moved off the stack again by using
the :ref:`pop operation<pop>`.

If the push operation is used by itself, e.g. not in a pipeline, it will
function as a no-operation that passes the coordinate through unchanged.

Examples
################################################################################

A common use of the push and :ref:`pop<pop>` operations is in 3D
:ref:`Helmert<helmert>` transformations where only the horizontal components
are needed. This is often the case when combining heights from a legacy
vertical reference with a modern geocentric reference. Below is a an example of
such a transformation, where the horizontal part is transformed with a Helmert
operation but the vertical part is kept exactly as the input was.

::

   $ echo 12 56 12.3 2020 | cct +proj=pipeline  \
   +step +proj=push +v_3                        \
   +step +proj=cart +ellps=GRS80                \
   +step +proj=helmert +x=3000 +y=1000 +z=2000  \
   +step +proj=cart +ellps=GRS80 +inv           \
   +step +proj=pop +v_3                         \

    12.0056753463   55.9866540552       12.3000     2000.0000

Note that the third coordinate component in the output is the same as the input.

The same transformation without the push and pop operations would look like this::

   $ echo 12 56 12.3 2020 | cct +proj=pipeline  \
   +step +proj=cart +ellps=GRS80                \
   +step +proj=helmert +x=3000 +y=1000 +z=2000  \
   +step +proj=cart +ellps=GRS80 +inv           \

         12.0057        55.9867     3427.7404     2000.0000

Here the vertical component is adjusted significantly.

Parameters
################################################################################

.. option:: +v_1

   Stores the first coordinate component on the pipeline stack

.. option:: +v_2

   Stores the second coordinate component on the pipeline stack

.. option:: +v_3

   Stores the third coordinate component on the pipeline stack

.. option:: +v_4

   Stores the fourth coordinate component on the pipeline stack


Further reading
################################################################################

#. `Stack data structure on Wikipedia <https://en.wikipedia.org/wiki/Stack_(abstract_data_type)>`_

