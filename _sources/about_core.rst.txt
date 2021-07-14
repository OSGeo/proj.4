PROJ is a generic coordinate transformation software that transforms geospatial
coordinates from one coordinate reference system (CRS) to another. This
includes cartographic projections as well as geodetic transformations. PROJ
is released under the :ref:`X/MIT open source license <license>`

PROJ includes :ref:`command line applications<apps>` for easy conversion of
coordinates from text files or directly from user input. In addition to the
command line utilities PROJ also exposes an
:ref:`application programming interface<reference>`, or API in short. The API
lets developers use the functionality of PROJ in their own software without having
to implement similar functionality themselves.

PROJ started purely as a cartography application letting users convert geodetic
coordinates into projected coordinates using a number of different cartographic
projections. Over the years, as the need has become apparent, support for datum
shifts has slowly worked its way into PROJ as well. Today PROJ supports more
than a hundred different map projections and can transform coordinates between
datums using all but the most obscure geodetic techniques.