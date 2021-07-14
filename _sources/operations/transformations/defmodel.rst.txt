.. _defmodel:

================================================================================
Multi-component time-based deformation model
================================================================================

.. versionadded:: 7.1.0

+---------------------+--------------------------------------------------------------------+
| **Alias**           | defmodel                                                           |
+---------------------+--------------------------------------------------------------------+
| **Input type**      | Geodetic or projected coordinates (horizontal), meters (vertical), |
|                     | decimalyear (temporal)                                             |
+---------------------+--------------------------------------------------------------------+
| **Output type**     | Geodetic or projected coordinates (horizontal), meters (vertical), |
|                     | decimalyear (temporal)                                             |
+---------------------+--------------------------------------------------------------------+
| **Domain**          | 4D                                                                 |
+---------------------+--------------------------------------------------------------------+
| **Available forms** | Forward and inverse                                                |
+---------------------+--------------------------------------------------------------------+

The defmodel transformation can be used to represent most deformation models
currently in use, in particular for areas subject to complex deformation, including
large scale secular crustal deformation near plate boundaries and vertical deformation
due to Glacial Isostatic Adjustment (GIA). These can often be represented by a constant
velocity model. Additionally, many areas suffer episodic deformation events such as
earthquakes and aseismic slow slip event.

The transformation relies on a "master" JSON file, describing general metadata on
the deformation model, its spatial and temporal extent, and listing spatial
components whose values are stored in :ref:`Geodetic TIFF grids (GTG) <geodetictiffgrids>`.
The valuation of each component is modulated by a time function (constant, step,
reverse step, velocity, piecewise, exponential).

All details on the content of this JSON file are given in the `Proposal for encoding
of a Deformation Model <https://github.com/linz/deformation-model-format/blob/master/doc/JsonGeotiffDeformationModelFormat_20200501.pdf>`__

If input coordinates are given in the geographic domain (resp. projected domain),
the output will also be in the geographic domain (resp. projected domain).
The domain should be the corresponding to the source_crs metadata of the model.

This transformation is a generalization of the :ref:`Kinematic datum shifting utilizing a deformation model <deformation>` transformation.

Parameters
-------------------------------------------------------------------------------

Required
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +model=<filename>

    Filename to the JSON master file for the deformation model.


Example
-------------------------------------------------------------------------------

Transforming a point with the LINZ NZGD2000 deformation model:

::

    echo 166.7133850980 -44.5105886020 293.3700 2007.689 | cct +proj=defmodel +model=nzgd2000-20180701.json
