.. _macros:

================================================================================
Macros
================================================================================

.. c:macro:: PROJ_VERSION_MAJOR

    Major version number, e.g. 8 for PROJ 8.0.1

.. c:macro:: PROJ_VERSION_MINOR

    Minor version number, e.g. 0 for PROJ 8.0.1

.. c:macro:: PROJ_VERSION_PATCH

    Patch version number, e.g. 1 for PROJ 8.0.1

.. c:macro:: PROJ_COMPUTE_VERSION(maj,min,patch)

    .. versionadded:: 8.0.1

    Compute the version number from the major, minor and patch numbers.

.. c:macro:: PROJ_VERSION_NUMBER

    .. versionadded:: 8.0.1

    Total version number, equal to
    ``PROJ_COMPUTE_VERSION(PROJ_VERSION_MAJOR, PROJ_VERSION_MINOR, PROJ_VERSION_PATCH)``

.. c:macro:: PROJ_AT_LEAST_VERSION(maj,min,patch)

    .. versionadded:: 8.0.1

    Macro that returns true if the current PROJ version is at least the version
    specified by (maj,min,patch)

    Equivalent to ``PROJ_VERSION_NUMBER >= PROJ_COMPUTE_VERSION(maj,min,patch)``
