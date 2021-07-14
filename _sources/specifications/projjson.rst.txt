.. _projjson:

================================================================================
PROJJSON
================================================================================

PROJJSON is a JSON encoding of
`WKT2:2019 / ISO-19162:2019 <http://docs.opengeospatial.org/is/18-010r7/18-010r7.html>`_,
which itself implements the model of
`OGC Topic 2: Referencing by coordinates <http://docs.opengeospatial.org/as/18-005r4/18-005r4.html>`_.
Apart from the difference of encodings, the semantics is intended to be exactly
the same as WKT2:2019.

PROJJSON is available as input and output of PROJ since PROJ 6.2.

The current version is 0.2.

Schema
------

A JSON schema of its grammar is available at
https://proj.org/schemas/v0.2/projjson.schema.json

History
-------

* v0.2: addition of geoid_model in VerticalCRS object.
* v0.1: initial version for PROJ 6.2

Content
-------

The high level objects are:

* Coordinate Reference Systems (CRS):

  - Common ones:

    + ``GeographicCRS``
    + ``GeodeticCRS``
    + ``ProjectedCRS``
    + ``CompoundCRS``
    + ``BoundCRS``

  - More esoteric ones:

    + ``VerticalCRS``
    + ``EngineeringCRS``
    + ``TemporalCRS``
    + ``ParametricCRS``
    + ``DerivedGeographicCRS``
    + ``DerivedGeodeticCRS``
    + ``DerivedProjectedCRS``
    + ``DerivedVerticalCRS``
    + ``DerivedEngineeringCRS``
    + ``DerivedTemporalCRS``
    + ``DerivedParametricCRS``

* Coordinate operations:

  - ``Transformation``
  - ``Conversion``
  - ``ConcatenatedOperation``

* Others:

  - ``PrimeMeridian``
  - ``Ellipsoid``
  - ``Datum``
  - ``DatumEnsemble``

Examples
--------

GeographicCRS
+++++++++++++

The following invocation

::

    projinfo EPSG:4326 -o PROJJSON -q

will output:

.. code-block:: json

    {
        "$schema": "https://proj.org/schemas/v0.1/projjson.schema.json",
        "type": "GeographicCRS",
        "name": "WGS 84",
        "datum": {
            "type": "GeodeticReferenceFrame",
            "name": "World Geodetic System 1984",
            "ellipsoid": {
                "name": "WGS 84",
                "semi_major_axis": 6378137,
                "inverse_flattening": 298.257223563
            }
        },
        "coordinate_system": {
            "subtype": "ellipsoidal",
            "axis": [
            {
                "name": "Geodetic latitude",
                "abbreviation": "Lat",
                "direction": "north",
                "unit": "degree"
            },
            {
                "name": "Geodetic longitude",
                "abbreviation": "Lon",
                "direction": "east",
                "unit": "degree"
            }
            ]
        },
        "area": "World",
        "bbox": {
            "south_latitude": -90,
            "west_longitude": -180,
            "north_latitude": 90,
            "east_longitude": 180
        },
        "id": {
            "authority": "EPSG",
            "code": 4326
        }
    }


ProjectedCRS
++++++++++++

The following invocation

::

    projinfo EPSG:32631 -o PROJJSON -q

will output:

.. code-block:: json

    {
        "$schema": "https://proj.org/schemas/v0.1/projjson.schema.json",
        "type": "ProjectedCRS",
        "name": "WGS 84 / UTM zone 31N",
        "base_crs": {
            "name": "WGS 84",
            "datum": {
                "type": "GeodeticReferenceFrame",
                "name": "World Geodetic System 1984",
                "ellipsoid": {
                    "name": "WGS 84",
                    "semi_major_axis": 6378137,
                    "inverse_flattening": 298.257223563
                }
            },
            "coordinate_system": {
                "subtype": "ellipsoidal",
                "axis": [
                {
                    "name": "Geodetic latitude",
                    "abbreviation": "Lat",
                    "direction": "north",
                    "unit": "degree"
                },
                {
                    "name": "Geodetic longitude",
                    "abbreviation": "Lon",
                    "direction": "east",
                    "unit": "degree"
                }
                ]
            },
            "id": {
                "authority": "EPSG",
                "code": 4326
            }
        },
        "conversion": {
            "name": "UTM zone 31N",
            "method": {
                "name": "Transverse Mercator",
                "id": {
                    "authority": "EPSG",
                    "code": 9807
                }
            },
            "parameters": [
            {
                "name": "Latitude of natural origin",
                "value": 0,
                "unit": "degree",
                "id": {
                "authority": "EPSG",
                "code": 8801
                }
            },
            {
                "name": "Longitude of natural origin",
                "value": 3,
                "unit": "degree",
                "id": {
                "authority": "EPSG",
                "code": 8802
                }
            },
            {
                "name": "Scale factor at natural origin",
                "value": 0.9996,
                "unit": "unity",
                "id": {
                "authority": "EPSG",
                "code": 8805
                }
            },
            {
                "name": "False easting",
                "value": 500000,
                "unit": "metre",
                "id": {
                "authority": "EPSG",
                "code": 8806
                }
            },
            {
                "name": "False northing",
                "value": 0,
                "unit": "metre",
                "id": {
                "authority": "EPSG",
                "code": 8807
                }
            }
            ]
        },
        "coordinate_system": {
            "subtype": "Cartesian",
            "axis": [
            {
                "name": "Easting",
                "abbreviation": "E",
                "direction": "east",
                "unit": "metre"
            },
            {
                "name": "Northing",
                "abbreviation": "N",
                "direction": "north",
                "unit": "metre"
            }
            ]
        },
        "area": "World - N hemisphere - 0°E to 6°E - by country",
        "bbox": {
            "south_latitude": 0,
            "west_longitude": 0,
            "north_latitude": 84,
            "east_longitude": 6
        },
        "id": {
            "authority": "EPSG",
            "code": 32631
        }
    }
