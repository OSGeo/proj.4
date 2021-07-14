.. _glossary:

================================================================================
Glossary
================================================================================

.. glossary::

    Ballpark transformation

        For a transformation between two geographic CRS, a ballpark
        transformation is a coordinate operation that only takes into account
        potential difference of axis orders (long-lat vs lat-long),
        units (degree vs grads) and  prime meridian (Greenwich vs Paris/Rome/other
        historic prime meridians). It does not attempt any datum shift, hence
        the "ballpark" qualifier in its name. Its accuracy is unknown, and could
        lead in some cases to errors of a few hundreds of metres.

        For a transformation between two vertical CRS or a vertical CRS and
        a geographic CRS, a ballpark transformation only takes into account
        potential different in units (e.g. metres vs feet). Its accuracy is
        unknown, and could lead in some cases to errors of a few tens of metres.

        .. note::

            The term "Ballpark transformation" is specific to PROJ.

    Pseudocylindrical Projection

        Pseudocylindrical projections have the mathematical characteristics of

        .. math::

            x &= f(\lambda,\phi)

            y &= g(\phi)

        where the parallels of latitude are straight lines, like cylindrical
        projections, but the meridians are curved toward the center as they
        depart from the equator. This is an effort to minimize the distortion
        of the polar regions inherent in the cylindrical projections.

        Pseudocylindrical projections are almost exclusively used for small
        scale global displays and, except for the Sinusoidal projection, only
        derived for a spherical Earth. Because of the basic definition none of
        the pseudocylindrical projections are conformal but many are equal
        area.

        To further reduce distortion, pseudocylindrical are often presented in
        interrupted form that are made by joining several regions with
        appropriate central meridians and false easting and clipping
        boundaries. Interrupted Homolosine constructions are suited for showing
        respective global land and oceanic regions, for example. To reduce the
        lateral size of the map, some uses remove an irregular, North-South
        strip of the mid-Atlantic region so that the western tip of Africa is
        plotted north of the eastern tip of South America.
