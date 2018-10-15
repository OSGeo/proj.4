--- Table structures

PRAGMA page_size = 4096;
PRAGMA foreign_keys = 1;

CREATE TABLE metadata(
    key TEXT NOT NULL PRIMARY KEY,
    value TEXT NOT NULL
);

CREATE TABLE unit_of_measure(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('length', 'angle', 'scale', 'time')),
    conv_factor FLOAT,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_unit_of_measure PRIMARY KEY (auth_name, code)
);

CREATE TABLE ellipsoid (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    semi_major_axis FLOAT NOT NULL CHECK (semi_major_axis > 0),
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    inv_flattening FLOAT CHECK (inv_flattening = 0 OR inv_flattening >= 1.0),
    semi_minor_axis FLOAT CHECK (semi_minor_axis > 0 AND semi_minor_axis <= semi_major_axis),
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_ellipsoid PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_ellipsoid_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER ellipsoid_insert_trigger
BEFORE INSERT ON ellipsoid
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on ellipsoid violates constraint: inv_flattening (exclusive) or semi_minor_axis should be defined')
        WHERE (NEW.inv_flattening IS NULL AND NEW.semi_minor_axis IS NULL) OR (NEW.inv_flattening IS NOT NULL AND NEW.semi_minor_axis IS NOT NULL);
    SELECT RAISE(ABORT, 'insert on ellipsoid violates constraint: uom should be of type ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE auth_name = NEW.uom_auth_name AND code = NEW.uom_code) != 'length';
END;

CREATE TABLE area(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    south_lat FLOAT CHECK (south_lat BETWEEN -90 AND 90),
    north_lat FLOAT CHECK (north_lat BETWEEN -90 AND 90),
    west_lon FLOAT CHECK (west_lon BETWEEN -180 AND 180),
    east_lon FLOAT CHECK (east_lon BETWEEN -180 AND 180),
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_area PRIMARY KEY (auth_name, code)
);

CREATE TRIGGER area_insert_trigger
BEFORE INSERT ON area
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on area violates constraint: south_lat <= north_lat')
        WHERE NEW.south_lat > NEW.north_lat;
    SELECT RAISE(ABORT, 'insert on area violates constraint: west_lon <= east_lon OR (east_lon + 360 - west_lon <= 200)')
        WHERE NOT(NEW.west_lon <= NEW.east_lon OR (NEW.east_lon + 360 - NEW.west_lon <= 200));
END;

CREATE TABLE prime_meridian(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    longitude FLOAT NOT NULL CHECK (longitude BETWEEN -180 AND 180),
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_prime_meridian PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_prime_meridian_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER prime_meridian_insert_trigger
BEFORE INSERT ON prime_meridian
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on prime_meridian violates constraint: uom should be of type ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE auth_name = NEW.uom_auth_name AND code = NEW.uom_code) != 'angle';
END;

CREATE TABLE geodetic_datum (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    ellipsoid_auth_name TEXT NOT NULL,
    ellipsoid_code TEXT NOT NULL,
    prime_meridian_auth_name TEXT NOT NULL,
    prime_meridian_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_geodetic_datum PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_geodetic_datum_ellipsoid FOREIGN KEY (ellipsoid_auth_name, ellipsoid_code) REFERENCES ellipsoid(auth_name, code),
    CONSTRAINT fk_geodetic_datum_prime_meridian FOREIGN KEY (prime_meridian_auth_name, prime_meridian_code) REFERENCES prime_meridian(auth_name, code),
    CONSTRAINT fk_geodetic_datum_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE vertical_datum (
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_vertical_datum PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_vertical_datum_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TABLE coordinate_system(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('Cartesian', 'vertical', 'ellipsoidal', 'spherical')),
    dimension SMALLINT NOT NULL CHECK (dimension BETWEEN 1 AND 3),
    CONSTRAINT pk_coordinate_system PRIMARY KEY (auth_name, code)
);

CREATE TRIGGER coordinate_system_insert_trigger
BEFORE INSERT ON coordinate_system
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on coordinate_system violates constraint: dimension must be equal to 1 for type = ''vertical''')
        WHERE NEW.type = 'vertical' AND NEW.dimension != 1;
    SELECT RAISE(ABORT, 'insert on coordinate_system violates constraint: dimension must be equal to 2 or 3 for type = ''Cartesian''')
        WHERE NEW.type = 'Cartesian' AND NEW.dimension NOT IN (2, 3);
    SELECT RAISE(ABORT, 'insert on coordinate_system violates constraint: dimension must be equal to 2 or 3 for type = ''ellipsoidal''')
        WHERE NEW.type = 'ellipsoidal' AND NEW.dimension NOT IN (2, 3);
END;

CREATE TABLE axis(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    abbrev TEXT NOT NULL,
    orientation TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    coordinate_system_order SMALLINT NOT NULL CHECK (coordinate_system_order BETWEEN 1 AND 3),
    uom_auth_name TEXT NOT NULL,
    uom_code TEXT NOT NULL,
    CONSTRAINT pk_axis PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_axis_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_axis_unit_of_measure FOREIGN KEY (uom_auth_name, uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER axis_insert_trigger
BEFORE INSERT ON axis
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on axis violates constraint: coordinate_system_order should be <= coordinate_system.dimension')
        WHERE NEW.coordinate_system_order > (SELECT dimension FROM coordinate_system WHERE auth_name = NEW.coordinate_system_auth_name AND code = NEW.coordinate_system_code);
END;

CREATE TABLE crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('geographic 2D', 'geographic 3D', 'geocentric', 'vertical', 'projected', 'compound')),
    CONSTRAINT pk_geodetic_crs PRIMARY KEY (auth_name, code)
);

CREATE TABLE geodetic_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('geographic 2D', 'geographic 3D', 'geocentric')),
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    datum_auth_name TEXT NOT NULL,
    datum_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_geodetic_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_geodetic_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_geodetic_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_geodetic_crs_datum FOREIGN KEY (datum_auth_name, datum_code) REFERENCES geodetic_datum(auth_name, code),
    CONSTRAINT fk_geodetic_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER geodetic_crs_insert_trigger
BEFORE INSERT ON geodetic_crs
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: type must be equal to crs.type')
        WHERE NEW.type != (SELECT type FROM crs WHERE crs.auth_name = NEW.auth_name AND crs.code = NEW.code);

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 3 for type = ''geocentric''')
        WHERE NEW.type = 'geocentric' AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 3;

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.type must be ''Cartesian'' for type = ''geocentric''')
        WHERE NEW.type = 'geocentric' AND (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'Cartesian';

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.type must be ''ellipsoidal''  for type = ''geographic 2D'' or ''geographic 3D''')
        WHERE NEW.type IN ('geographic 2D', 'geographic 3D') AND (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'ellipsoidal';

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 2 for type = ''geographic 2D''')
        WHERE NEW.type = 'geographic 2D' AND NEW.deprecated != 1 AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 2;

    SELECT RAISE(ABORT, 'insert on geodetic_crs violates constraint: coordinate_system.dimension must be 3 for type = ''geographic 3D''')
        WHERE NEW.type = 'geographic 3D' AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 3;
END;

CREATE TABLE vertical_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    datum_auth_name TEXT NOT NULL,
    datum_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_vertical_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_vertical_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_vertical_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_vertical_crs_datum FOREIGN KEY (datum_auth_name, datum_code) REFERENCES vertical_datum(auth_name, code),
    CONSTRAINT fk_vertical_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER vertical_crs_insert_trigger
BEFORE INSERT ON vertical_crs
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: crs.type must be equal to ''vertical''')
        WHERE (SELECT type FROM crs WHERE crs.auth_name = NEW.auth_name AND crs.code = NEW.code) != 'vertical';
    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: coordinate_system.type must be ''vertical''')
        WHERE (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'vertical';
    SELECT RAISE(ABORT, 'insert on vertical_crs violates constraint: coordinate_system.dimension must be 1')
        WHERE (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 1;
END;

CREATE TABLE coordinate_operation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('conversion', 'grid_transformation', 'helmert_transformation', 'other_transformation', 'concatenated_operation')),
    CONSTRAINT pk_coordinate_operation PRIMARY KEY (auth_name, code)
);

CREATE TABLE conversion(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    method_auth_name TEXT,
    method_code TEXT,
    method_name NOT NULL,

    param1_auth_name TEXT,
    param1_code TEXT,
    param1_name TEXT,
    param1_value FLOAT,
    param1_uom_auth_name TEXT,
    param1_uom_code TEXT,

    param2_auth_name TEXT,
    param2_code TEXT,
    param2_name TEXT,
    param2_value FLOAT,
    param2_uom_auth_name TEXT,
    param2_uom_code TEXT,

    param3_auth_name TEXT,
    param3_code TEXT,
    param3_name TEXT,
    param3_value FLOAT,
    param3_uom_auth_name TEXT,
    param3_uom_code TEXT,

    param4_auth_name TEXT,
    param4_code TEXT,
    param4_name TEXT,
    param4_value FLOAT,
    param4_uom_auth_name TEXT,
    param4_uom_code TEXT,

    param5_auth_name TEXT,
    param5_code TEXT,
    param5_name TEXT,
    param5_value FLOAT,
    param5_uom_auth_name TEXT,
    param5_uom_code TEXT,

    param6_auth_name TEXT,
    param6_code TEXT,
    param6_name TEXT,
    param6_value FLOAT,
    param6_uom_auth_name TEXT,
    param6_uom_code TEXT,

    param7_auth_name TEXT,
    param7_code TEXT,
    param7_name TEXT,
    param7_value FLOAT,
    param7_uom_auth_name TEXT,
    param7_uom_code TEXT,

    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),

    CONSTRAINT pk_conversion PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_conversion_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code),
    CONSTRAINT fk_conversion_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_conversion_param1_uom FOREIGN KEY (param1_uom_auth_name, param1_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param2_uom FOREIGN KEY (param2_uom_auth_name, param2_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param3_uom FOREIGN KEY (param3_uom_auth_name, param3_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param4_uom FOREIGN KEY (param4_uom_auth_name, param4_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param5_uom FOREIGN KEY (param5_uom_auth_name, param5_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param6_uom FOREIGN KEY (param6_uom_auth_name, param6_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_conversion_param7_uom FOREIGN KEY (param7_uom_auth_name, param7_uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER conversion_insert_trigger
BEFORE INSERT ON conversion
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on conversion violates constraint: coordinate_operation.type must be equal to ''conversion''')
        WHERE (SELECT type FROM coordinate_operation WHERE coordinate_operation.auth_name = NEW.auth_name AND coordinate_operation.code = NEW.code) != 'conversion';
    SELECT RAISE(ABORT, 'insert on conversion violates constraint: method should be known')
        WHERE ((CASE WHEN NEW.method_auth_name is NULL THEN '' ELSE NEW.method_auth_name END) || '_' || (CASE WHEN NEW.method_code is NULL THEN '' ELSE NEW.method_code END) || '_' || NEW.method_name) NOT IN (
            'EPSG_1024_Popular Visualisation Pseudo Mercator',
            'EPSG_1027_Lambert Azimuthal Equal Area (Spherical)',
            'EPSG_1028_Equidistant Cylindrical',
            'EPSG_1029_Equidistant Cylindrical (Spherical)',
            'EPSG_1041_Krovak (North Orientated)',
            'EPSG_1042_Krovak Modified',
            'EPSG_1043_Krovak Modified (North Orientated)',
            'EPSG_1051_Lambert Conic Conformal (2SP Michigan)',
            'EPSG_1052_Colombia Urban',
            'EPSG_1068_Height Depth Reversal',
            'EPSG_1069_Change of Vertical Unit',
            'EPSG_1078_Equal Earth',
            'EPSG_9602_Geographic/geocentric conversions',
            'EPSG_9659_Geographic3D to 2D conversion',
            'EPSG_9801_Lambert Conic Conformal (1SP)',
            'EPSG_9802_Lambert Conic Conformal (2SP)',
            'EPSG_9803_Lambert Conic Conformal (2SP Belgium)',
            'EPSG_9804_Mercator (variant A)',
            'EPSG_9805_Mercator (variant B)',
            'EPSG_9806_Cassini-Soldner',
            'EPSG_9807_Transverse Mercator',
            'EPSG_9808_Transverse Mercator (South Orientated)',
            'EPSG_9809_Oblique Stereographic',
            'EPSG_9810_Polar Stereographic (variant A)',
            'EPSG_9811_New Zealand Map Grid',
            'EPSG_9812_Hotine Oblique Mercator (variant A)',
            'EPSG_9813_Laborde Oblique Mercator',
            'EPSG_9815_Hotine Oblique Mercator (variant B)',
            'EPSG_9816_Tunisia Mining Grid',
            'EPSG_9817_Lambert Conic Near-Conformal',
            'EPSG_9818_American Polyconic',
            'EPSG_9819_Krovak',
            'EPSG_9820_Lambert Azimuthal Equal Area',
            'EPSG_9821_Lambert Azimuthal Equal Area (Spherical)',
            'EPSG_9822_Albers Equal Area',
            'EPSG_9823_Equidistant Cylindrical (Spherical)',
            'EPSG_9824_Transverse Mercator Zoned Grid System',
            'EPSG_9826_Lambert Conic Conformal (West Orientated)',
            'EPSG_9828_Bonne (South Orientated)',
            'EPSG_9829_Polar Stereographic (variant B)',
            'EPSG_9830_Polar Stereographic (variant C)',
            'EPSG_9831_Guam Projection',
            'EPSG_9832_Modified Azimuthal Equidistant',
            'EPSG_9833_Hyperbolic Cassini-Soldner',
            'EPSG_9834_Lambert Cylindrical Equal Area (Spherical)',
            'EPSG_9835_Lambert Cylindrical Equal Area',
            'EPSG_9836_Geocentric/topocentric conversions',
            'EPSG_9837_Geographic/topocentric conversions',
            'EPSG_9838_Vertical Perspective',
            'EPSG_9841_Mercator (1SP) (Spherical)',
            'EPSG_9842_Equidistant Cylindrical',
            'EPSG_9843_Axis Order Reversal (2D)',
            'EPSG_9844_Axis Order Reversal (Geographic3D horizontal)',
            '__Gauss Schreiber Transverse Mercator',
            '__PROJ mill');
END;

CREATE TABLE projected_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    coordinate_system_auth_name TEXT NOT NULL,
    coordinate_system_code TEXT NOT NULL,
    geodetic_crs_auth_name TEXT NOT NULL,
    geodetic_crs_code TEXT NOT NULL,
    conversion_auth_name TEXT NOT NULL,
    conversion_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_projected_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_projected_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_projected_crs_coordinate_system FOREIGN KEY (coordinate_system_auth_name, coordinate_system_code) REFERENCES coordinate_system(auth_name, code),
    CONSTRAINT fk_projected_crs_geodetic_crs FOREIGN KEY (geodetic_crs_auth_name, geodetic_crs_code) REFERENCES geodetic_crs(auth_name, code),
    CONSTRAINT fk_projected_crs_conversion FOREIGN KEY (conversion_auth_name, conversion_code) REFERENCES conversion(auth_name, code),
    CONSTRAINT fk_projected_crs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER projected_crs_insert_trigger
BEFORE INSERT ON projected_crs
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: crs.type must be equal to ''projected''')
        WHERE (SELECT type FROM crs WHERE crs.auth_name = NEW.auth_name AND crs.code = NEW.code) != 'projected';
    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: coordinate_system.type must be ''cartesian''')
        WHERE (SELECT type FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 'Cartesian';
    SELECT RAISE(ABORT, 'insert on projected_crs violates constraint: coordinate_system.dimension must be 2')
    -- EPSG:4461 is topocentric
        WHERE NOT(NEW.coordinate_system_auth_name = 'EPSG' AND NEW.coordinate_system_code = '4461') AND (SELECT dimension FROM coordinate_system WHERE coordinate_system.auth_name = NEW.coordinate_system_auth_name AND coordinate_system.code = NEW.coordinate_system_code) != 2;
END;

CREATE TABLE compound_crs(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    horiz_crs_auth_name TEXT NOT NULL,
    horiz_crs_code TEXT NOT NULL,
    vertical_crs_auth_name TEXT NOT NULL,
    vertical_crs_code TEXT NOT NULL,
    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,
    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),
    CONSTRAINT pk_compound_crs PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_compound_crs_crs FOREIGN KEY (auth_name, code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_compound_crs_horiz_crs FOREIGN KEY (horiz_crs_auth_name, horiz_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_compound_crs_vertical_crs FOREIGN KEY (vertical_crs_auth_name, vertical_crs_code) REFERENCES vertical_crs(auth_name, code),
    CONSTRAINT fk_compoundcrs_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER compound_crs_insert_trigger
BEFORE INSERT ON compound_crs
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: crs.type must be equal to ''compound''')
        WHERE (SELECT type FROM crs WHERE crs.auth_name = NEW.auth_name AND crs.code = NEW.code) != 'compound';
    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: horiz_crs must be equal to ''geographic 2D'' or ''projected''')
        WHERE (SELECT type FROM crs WHERE crs.auth_name = NEW.horiz_crs_auth_name AND crs.code = NEW.horiz_crs_code) NOT IN ('geographic 2D', 'projected');
    SELECT RAISE(ABORT, 'insert on compound_crs violates constraint: vertical_crs must be equal to ''vertical''')
        WHERE (SELECT type FROM crs WHERE crs.auth_name = NEW.vertical_crs_auth_name AND crs.code = NEW.vertical_crs_code) NOT IN ('vertical');
END;

CREATE TABLE helmert_transformation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT CHECK (accuracy >= 0),

    tx FLOAT NOT NULL,
    ty FLOAT NOT NULL,
    tz FLOAT NOT NULL,
    translation_uom_auth_name TEXT NOT NULL,
    translation_uom_code TEXT NOT NULL,
    rx FLOAT,
    ry FLOAT,
    rz FLOAT,
    rotation_uom_auth_name TEXT,
    rotation_uom_code TEXT,
    scale_difference FLOAT,
    scale_difference_uom_auth_name TEXT,
    scale_difference_uom_code TEXT,
    rate_tx FLOAT,
    rate_ty FLOAT,
    rate_tz FLOAT,
    rate_translation_uom_auth_name TEXT,
    rate_translation_uom_code TEXT,
    rate_rx FLOAT,
    rate_ry FLOAT,
    rate_rz FLOAT,
    rate_rotation_uom_auth_name TEXT,
    rate_rotation_uom_code TEXT,
    rate_scale_difference FLOAT,
    rate_scale_difference_uom_auth_name TEXT,
    rate_scale_difference_uom_code TEXT,
    epoch FLOAT,
    epoch_uom_auth_name TEXT,
    epoch_uom_code TEXT,

    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),

    CONSTRAINT pk_helmert_transformation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_helmert_transformation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES geodetic_crs(auth_name, code),
    CONSTRAINT fk_helmert_transformation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES geodetic_crs(auth_name, code),
    CONSTRAINT fk_helmert_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code),
    CONSTRAINT fk_helmert_transformation_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_helmert_translation_uom FOREIGN KEY (translation_uom_auth_name, translation_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_rotation_uom FOREIGN KEY (rotation_uom_auth_name, rotation_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_scale_difference_uom FOREIGN KEY (scale_difference_uom_auth_name, scale_difference_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_rate_translation_uom FOREIGN KEY (rate_translation_uom_auth_name, rate_translation_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_rate_rotation_uom FOREIGN KEY (rate_rotation_uom_auth_name, rate_rotation_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_rate_scale_difference_uom FOREIGN KEY (rate_scale_difference_uom_auth_name, rate_scale_difference_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_helmert_epoch_uom FOREIGN KEY (epoch_uom_auth_name, epoch_uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER helmert_transformation_insert_trigger
BEFORE INSERT ON helmert_transformation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: coordinate_operation.type must be equal to ''helmert_transformation''')
        WHERE (SELECT type FROM coordinate_operation WHERE coordinate_operation.auth_name = NEW.auth_name AND coordinate_operation.code = NEW.code) != 'helmert_transformation';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: translation_uom.type must be ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.translation_uom_auth_name AND unit_of_measure.code = NEW.translation_uom_code) != 'length';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rotation_uom.type must be ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rotation_uom_auth_name AND unit_of_measure.code = NEW.rotation_uom_code) != 'angle';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: scale_difference_uom.type must be ''scale''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.scale_difference_uom_auth_name AND unit_of_measure.code = NEW.scale_difference_uom_code) != 'scale';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_translation_uom.type must be ''length''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_translation_uom_auth_name AND unit_of_measure.code = NEW.rate_translation_uom_code) != 'length';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_rotation_uom.type must be ''angle''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_rotation_uom_auth_name AND unit_of_measure.code = NEW.rate_rotation_uom_code) != 'angle';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: rate_scale_difference_uom.type must be ''scale''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.rate_scale_difference_uom_auth_name AND unit_of_measure.code = NEW.rate_scale_difference_uom_code) != 'scale';
    SELECT RAISE(ABORT, 'insert on helmert_transformation violates constraint: epoch_uom.type must be ''time''')
        WHERE (SELECT type FROM unit_of_measure WHERE unit_of_measure.auth_name = NEW.epoch_uom_auth_name AND unit_of_measure.code = NEW.epoch_uom_code) != 'time';
END;

CREATE TABLE grid_transformation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT CHECK (accuracy >= 0),

    grid_param_auth_name TEXT NOT NULL,
    grid_param_code TEXT NOT NULL,
    grid_param_name TEXT NOT NULL,
    grid_name TEXT NOT NULL,

    grid2_param_auth_name TEXT,
    grid2_param_code TEXT,
    grid2_param_name TEXT,
    grid2_name TEXT,

    interpolation_crs_auth_name TEXT,
    interpolation_crs_code TEXT,

    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),

    CONSTRAINT pk_grid_transformation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_grid_transformation_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_grid_transformation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_interpolation_crs FOREIGN KEY (interpolation_crs_auth_name, interpolation_crs_code) REFERENCES geodetic_crs(auth_name, code),
    CONSTRAINT fk_grid_transformation_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER grid_transformation_insert_trigger
BEFORE INSERT ON grid_transformation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_transformation violates constraint: coordinate_operation.type must be equal to ''grid_transformation''')
        WHERE (SELECT type FROM coordinate_operation WHERE coordinate_operation.auth_name = NEW.auth_name AND coordinate_operation.code = NEW.code) != 'grid_transformation';
END;

-- Table that describe packages/archives that contain several grids
CREATE TABLE grid_packages(
    package_name TEXT NOT NULL NULL PRIMARY KEY,    -- package name that contains the file
    description TEXT,
    url TEXT,                                       -- optional URL where to download the PROJ grid
    direct_download BOOLEAN CHECK (direct_download IN (0, 1)), -- whether the URL can be used directly (if 0, authentication etc mightbe needed)
    open_license BOOLEAN CHECK (open_license IN (0, 1))
);

CREATE TRIGGER grid_packages_insert_trigger
BEFORE INSERT ON grid_packages
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_packages violates constraint: open_license must be set when url is not NULL')
        WHERE NEW.open_license IS NULL AND NEW.url IS NOT NULL;
    SELECT RAISE(ABORT, 'insert on grid_packages violates constraint: direct_download must be set when url is not NULL')
        WHERE NEW.direct_download IS NULL AND NEW.url IS NOT NULL;
END;

-- Table that contain alternative names for original grid names coming from the authority
CREATE TABLE grid_alternatives(
    original_grid_name TEXT NOT NULL PRIMARY KEY,   -- original grid name (e.g. Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree.gz). For LOS/LAS format, the .las files
    proj_grid_name TEXT NOT NULL,                   -- PROJ grid name (e.g egm08_25.gtx)
    proj_grid_format TEXT NOT NULL,                 -- one of 'CTable2', 'NTv1', 'NTv2', 'GTX'
    proj_method TEXT NOT NULL,                      -- hgridshift or vgridshift
    inverse_direction BOOLEAN NOT NULL CHECK (inverse_direction IN (0, 1)), -- whether the PROJ grid direction is reversed w.r.t to the authority one (TRUE in that case)
    package_name TEXT,                              -- package name that contains the file
    url TEXT,                                       -- optional URL where to download the PROJ grid
    direct_download BOOLEAN CHECK (direct_download IN (0, 1)), -- whether the URL can be used directly (if 0, authentication etc might be needed)
    open_license BOOLEAN CHECK (open_license IN (0, 1)),
    directory TEXT,                                 -- optional directory where the file might be located

    CONSTRAINT fk_grid_alternatives_grid_packages FOREIGN KEY (package_name) REFERENCES grid_packages(package_name)
);

CREATE TRIGGER grid_alternatives_insert_trigger
BEFORE INSERT ON grid_alternatives
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: proj_grid_format must be one of ''CTable2'', ''NTv1'', ''NTv2'', ''GTX''')
        WHERE NEW.proj_grid_format NOT IN ('CTable2', 'NTv1', 'NTv2', 'GTX');
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: proj_method must be one of ''hgridshift'', ''vgridshift''')
        WHERE NEW.proj_method NOT IN ('hgridshift', 'vgridshift');
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: proj_method must be ''hgridshift'' when proj_grid_format is ''CTable2'', ''NTv1'', ''NTv2''')
        WHERE NEW.proj_method != 'hgridshift' AND NEW.proj_grid_format IN ('CTable2', 'NTv1', 'NTv2');
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: proj_method must be ''vridshift'' when proj_grid_format is ''GTX''')
        WHERE NEW.proj_method != 'vgridshift' AND NEW.proj_grid_format IN ('GTX');
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: original_grid_name must be referenced in grid_transformation.grid_name')
        WHERE NEW.original_grid_name NOT IN ('null') AND NEW.original_grid_name NOT IN (SELECT grid_name FROM grid_transformation);
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: NEW.inverse_direction must be 0 when original_grid_name = proj_grid_name')
        WHERE NEW.original_grid_name = NEW.proj_grid_name AND NEW.inverse_direction != 0;
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: package_name must be NULL when url is not NULL')
        WHERE NEW.package_name IS NOT NULL AND NEW.url IS NOT NULL;
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: direct_download must be set when url is not NULL')
        WHERE NEW.direct_download IS NULL AND NEW.url IS NOT NULL;
    SELECT RAISE(ABORT, 'insert on grid_alternatives violates constraint: open_license must be set when url is not NULL')
        WHERE NEW.open_license IS NULL AND NEW.url IS NOT NULL;
END;

CREATE TABLE other_transformation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    method_auth_name TEXT NOT NULL,
    method_code TEXT NOT NULL,
    method_name NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT CHECK (accuracy >= 0),

    param1_auth_name TEXT,
    param1_code TEXT,
    param1_name TEXT,
    param1_value FLOAT,
    param1_uom_auth_name TEXT,
    param1_uom_code TEXT,

    param2_auth_name TEXT,
    param2_code TEXT,
    param2_name TEXT,
    param2_value FLOAT,
    param2_uom_auth_name TEXT,
    param2_uom_code TEXT,

    param3_auth_name TEXT,
    param3_code TEXT,
    param3_name TEXT,
    param3_value FLOAT,
    param3_uom_auth_name TEXT,
    param3_uom_code TEXT,

    param4_auth_name TEXT,
    param4_code TEXT,
    param4_name TEXT,
    param4_value FLOAT,
    param4_uom_auth_name TEXT,
    param4_uom_code TEXT,

    param5_auth_name TEXT,
    param5_code TEXT,
    param5_name TEXT,
    param5_value FLOAT,
    param5_uom_auth_name TEXT,
    param5_uom_code TEXT,

    param6_auth_name TEXT,
    param6_code TEXT,
    param6_name TEXT,
    param6_value FLOAT,
    param6_uom_auth_name TEXT,
    param6_uom_code TEXT,

    param7_auth_name TEXT,
    param7_code TEXT,
    param7_name TEXT,
    param7_value FLOAT,
    param7_uom_auth_name TEXT,
    param7_uom_code TEXT,

    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),

    CONSTRAINT pk_other_transformation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_other_transformation_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_other_transformation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_other_transformation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_other_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
    CONSTRAINT fk_other_transformation_param1_uom FOREIGN KEY (param1_uom_auth_name, param1_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param2_uom FOREIGN KEY (param2_uom_auth_name, param2_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param3_uom FOREIGN KEY (param3_uom_auth_name, param3_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param4_uom FOREIGN KEY (param4_uom_auth_name, param4_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param5_uom FOREIGN KEY (param5_uom_auth_name, param5_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param6_uom FOREIGN KEY (param6_uom_auth_name, param6_uom_code) REFERENCES unit_of_measure(auth_name, code),
    CONSTRAINT fk_other_transformation_param7_uom FOREIGN KEY (param7_uom_auth_name, param7_uom_code) REFERENCES unit_of_measure(auth_name, code)
);

CREATE TRIGGER other_transformation_insert_trigger
BEFORE INSERT ON other_transformation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on other_transformation violates constraint: coordinate_operation.type must be equal to ''other_transformation''')
        WHERE (SELECT type FROM coordinate_operation WHERE coordinate_operation.auth_name = NEW.auth_name AND coordinate_operation.code = NEW.code) != 'other_transformation';
END;

-- Note: in EPSG, the steps might be to be chained in reverse order, so we cannot
-- enforce that source_crs_code == step1.source_crs_code etc
CREATE TABLE concatenated_operation(
    auth_name TEXT NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,

    source_crs_auth_name TEXT NOT NULL,
    source_crs_code TEXT NOT NULL,
    target_crs_auth_name TEXT NOT NULL,
    target_crs_code TEXT NOT NULL,

    area_of_use_auth_name TEXT NOT NULL,
    area_of_use_code TEXT NOT NULL,

    accuracy FLOAT CHECK (accuracy >= 0),

    step1_auth_name TEXT NOT NULL,
    step1_code TEXT NOT NULL,

    step2_auth_name TEXT NOT NULL,
    step2_code TEXT NOT NULL,

    step3_auth_name TEXT,
    step3_code TEXT,

    deprecated BOOLEAN NOT NULL CHECK (deprecated IN (0, 1)),

    CONSTRAINT pk_concatenated_operation PRIMARY KEY (auth_name, code),
    CONSTRAINT fk_concatenated_operation_coordinate_operation FOREIGN KEY (auth_name, code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_source_crs FOREIGN KEY (source_crs_auth_name, source_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_concatenated_operation_target_crs FOREIGN KEY (target_crs_auth_name, target_crs_code) REFERENCES crs(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step1 FOREIGN KEY (step1_auth_name, step1_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step2 FOREIGN KEY (step2_auth_name, step2_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_step3 FOREIGN KEY (step3_auth_name, step3_code) REFERENCES coordinate_operation(auth_name, code),
    CONSTRAINT fk_concatenated_operation_transformation_area FOREIGN KEY (area_of_use_auth_name, area_of_use_code) REFERENCES area(auth_name, code)
);

CREATE TRIGGER concatenated_operation_insert_trigger
BEFORE INSERT ON concatenated_operation
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: coordinate_operation.type must be equal to ''concatenated_operation''')
        WHERE (SELECT type FROM coordinate_operation WHERE coordinate_operation.auth_name = NEW.auth_name AND coordinate_operation.code = NEW.code) != 'concatenated_operation';
    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: step1 should not be a concatenated_operation')
        WHERE EXISTS(SELECT 1 FROM concatenated_operation WHERE auth_name = NEW.step1_auth_name AND code = NEW.step1_code);
    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: step2 should not be a concatenated_operation')
        WHERE EXISTS(SELECT 1 FROM concatenated_operation WHERE auth_name = NEW.step2_auth_name AND code = NEW.step2_code);
    SELECT RAISE(ABORT, 'insert on concatenated_operation violates constraint: step3 should not be a concatenated_operation')
        WHERE EXISTS(SELECT 1 FROM concatenated_operation WHERE auth_name = NEW.step3_auth_name AND code = NEW.step3_code);
END;

CREATE VIEW coordinate_operation_view AS
    SELECT 'grid_transformation' AS table_name, auth_name, code, name,
           method_auth_name, method_code, method_name, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM grid_transformation
    UNION ALL
    SELECT 'helmert_transformation' AS table_name, auth_name, code, name,
           method_auth_name, method_code, method_name, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM helmert_transformation
    UNION ALL
    SELECT 'other_transformation' AS table_name, auth_name, code, name,
           method_auth_name, method_code, method_name, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM other_transformation
    UNION ALL
    SELECT 'concatenated_operation' AS table_name, auth_name, code, name,
           NULL, NULL, NULL, source_crs_auth_name,
           source_crs_code, target_crs_auth_name, target_crs_code,
           area_of_use_auth_name, area_of_use_code,
           accuracy, deprecated FROM concatenated_operation
;

CREATE VIEW crs_view AS
    SELECT 'geodetic_crs' AS table_name, auth_name, code, name, type,
           area_of_use_auth_name, area_of_use_code,
           deprecated FROM geodetic_crs
    UNION ALL
    SELECT 'projected_crs' AS table_name, auth_name, code, name, 'projected',
           area_of_use_auth_name, area_of_use_code,
           deprecated FROM projected_crs
    UNION ALL
    SELECT 'vertical_crs' AS table_name, auth_name, code, name, 'vertical',
           area_of_use_auth_name, area_of_use_code,
           deprecated FROM vertical_crs
    UNION ALL
    SELECT 'compound_crs' AS table_name, auth_name, code, name, 'compound',
           area_of_use_auth_name, area_of_use_code,
           deprecated FROM compound_crs
;

CREATE VIEW object_view AS
    SELECT 'unit_of_measure' AS table_name, auth_name, code, name, NULL as area_of_use_auth_name, NULL as area_of_use_code, deprecated FROM unit_of_measure
    UNION ALL
    SELECT 'ellipsoid', auth_name, code, name, NULL, NULL, deprecated FROM ellipsoid
    UNION ALL
    SELECT 'area', auth_name, code, name, NULL, NULL, deprecated FROM area
    UNION ALL
    SELECT 'prime_meridian', auth_name, code, name, NULL, NULL, deprecated FROM prime_meridian
    UNION ALL
    SELECT 'geodetic_datum', auth_name, code, name, area_of_use_auth_name, area_of_use_code, deprecated FROM geodetic_datum
    UNION ALL
    SELECT 'vertical_datum', auth_name, code, name, area_of_use_auth_name, area_of_use_code, deprecated FROM vertical_datum
    UNION ALL
    SELECT 'axis', auth_name, code, name, NULL, NULL, 0 as deprecated FROM axis
    UNION ALL
    SELECT table_name, auth_name, code, name, area_of_use_auth_name, area_of_use_code, deprecated FROM crs_view
    UNION ALL
    SELECT 'conversion', auth_name, code, name, area_of_use_auth_name, area_of_use_code, deprecated FROM conversion
    UNION ALL
    SELECT table_name, auth_name, code, name, area_of_use_auth_name, area_of_use_code, deprecated FROM coordinate_operation_view
;

CREATE VIEW authority_list AS
    SELECT DISTINCT auth_name FROM unit_of_measure
    UNION
    SELECT DISTINCT auth_name FROM ellipsoid
    UNION
    SELECT DISTINCT auth_name FROM area
    UNION
    SELECT DISTINCT auth_name FROM prime_meridian
    UNION
    SELECT DISTINCT auth_name FROM geodetic_datum
    UNION
    SELECT DISTINCT auth_name FROM vertical_datum
    UNION
    SELECT DISTINCT auth_name FROM axis
    UNION
    SELECT DISTINCT auth_name FROM crs
    UNION
    SELECT DISTINCT auth_name FROM coordinate_operation
;
