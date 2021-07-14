.. _projsync:

================================================================================
projsync
================================================================================

.. Index:: projsync

.. only:: html

    .. versionadded:: 7.0.0

    Tool for synchronizing PROJ datum and transformation support data.

Synopsis
********

    | **projsync**
    |      [--endpoint URL]
    |      [--local-geojson-file FILENAME]
    |      ([--user-writable-directory] | [--system-directory] | [--target-dir DIRNAME])
    |      [--bbox west_long,south_lat,east_long,north_lat]
    |      [--spatial-test contains|intersects]
    |      [--source-id ID] [--area-of-use NAME]
    |      [--file NAME]
    |      [--all] [--exclude-world-coverage]
    |      [--quiet | --verbose] [--dry-run] [--list-files]
    |      [--no-version-filtering]

Description
***********

:program:`projsync` is a program that downloads remote resource files
into a local directory. This is an alternative to downloading a proj-data-X.Y.Z
archive file, or using the on-demand :ref:`networking capabilities <network>` of PROJ.

The following control parameters can appear in any order:

.. program:: projsync

.. option:: --endpoint URL

    Defines the URL where to download the master :file:`files.geojson` file and then
    the resource files. Defaults to the value set in :ref:`proj-ini`

.. option:: --local-geojson-file FILENAME

    Defines the filename for the master GeoJSON files that references resources.
    Defaults to ``${endpoint}/files.geojson``

.. option:: --user-writable-directory

    Specifies that resource files must be downloaded in the
    :ref:`user writable directory <user_writable_directory>`. This is the default.

.. option:: --system-directory

    Specifies that resource files must be downloaded in the
    ${installation_prefix}/share/proj directory. The user launching projsync
    should make sure it has writing rights in that directory.

.. option:: --target-dir DIRNAME

    Directory into which resource files must be downloaded.

.. option:: --bbox west_long,south_lat,east_long,north_lat

    Specify an area of interest to restrict the resources to download.
    The area of interest is specified as a
    bounding box with geographic coordinates, expressed in degrees in a
    unspecified geographic CRS.
    `west_long` and `east_long` should be in the [-180,180] range, and
    `south_lat` and `north_lat` in the [-90,90]. `west_long` is generally lower than
    `east_long`, except in the case where the area of interest crosses the antimeridian.

.. option:: --spatial-test contains|intersects

    Specify how the extent of the resource files
    are compared to the area of use specified explicitly with :option:`--bbox`.
    By default, any resource files whose extent intersects the value specified
    by :option:`--bbox` will be selected.
    If using the ``contains`` strategy, only resource files whose extent is
    contained in the value specified by :option:`--bbox` will be selected.

.. option:: --source-id ID

    Restrict resource files to be downloaded to those whose source_id property
    contains the ID value. Specifying ``?`` as ID will list all possible values.

.. option:: --area-of-use NAME

    Restrict resource files to be downloaded to those whose area_of_use property
    contains the NAME value. Specifying ``?`` as NAME will list all possible values.

.. option:: --file NAME

    Restrict resource files to be downloaded to those whose name property
    contains the NAME value. Specifying ``?`` as NAME will list all possible values.

.. option:: --all

    Ask to download all files.

.. option:: --exclude-world-coverage

    Exclude files which have world coverage.

.. option:: -q / --quiet

    Quiet mode

.. option:: --verbose

    .. versionadded:: 8.1

    Verbose mode (more than default)

.. option:: --dry-run

    Simulate the behavior of the tool without downloading resource files.

.. option:: --list-files

    List file names, with the source_id and area_of_use properties.

.. option:: --no-version-filtering

    .. versionadded:: 8.1

    By default, projsync only downloads files that are compatible of
    the PROJ_DATA.VERSION metadata of :file:`proj.db`, taking into account the
    ``version_added`` and ``version_removed`` properties of entries in :file:`files.geojson`.
    When specifying this switch, all files referenced in :file:`files.geojson`
    will be candidate (combined with other filters).


At least one of  :option:`--list-files`,  :option:`--file`,  :option:`--source-id`,
:option:`--area-of-use`,  :option:`--bbox` or  :option:`--all` must be specified.

Options :option:`--file`,  :option:`--source-id`, :option:`--area-of-use` and
:option:`--bbox` are combined with a AND logic.

Examples
********

1. Download all resource files

.. code-block:: console

      projsync --all

2. Download resource files covering specified point and attributing to an agency

.. code-block:: console

      projsync --source-id fr_ign --bbox 2,49,2,49


.. only:: man

    See also
    ********

    **cs2cs(1)**, **cct(1)**, **geod(1)**, **gie(1)**, **proj(1)**, **projinfo(1)**

    Bugs
    ****

    A list of known bugs can be found at https://github.com/OSGeo/PROJ/issues
    where new bug reports can be submitted to.
    Bugs specific to resource files should be submitted to
    https://github.com/OSGeo/PROJ-data/issues

    Home page
    *********

    https://proj.org/
