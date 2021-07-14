.. _network:

================================================================================
Network capabilities
================================================================================

.. versionadded:: 7.0

PROJ 7.0 has introduced, per :ref:`rfc4`, the capability to work with grid files
that are not installed on the local machine where PROJ is executed.

This enables to transparently download the parts of grids that are needed to
perform a coordinate transformation.

.. _cdn_grids:

CDN of GeoTIFF grids
--------------------

Files are accessed by default through a CDN (Content Delivery Network),
accessible through https://cdn.proj.org, that contains :ref:`geodetictiffgrids`
datasets which are mirrored and managed by the
https://github.com/OSGeo/PROJ-data/ GitHub project.
Files in the CDN are designed to be used by PROJ 7 or later, but any software
project wishing to use the CDN for shifting support are encouraged to
participate in the project and leverage the CDN. 

How to enable network capabilities ?
------------------------------------

This capability assumes that PROJ has been build against `libcurl`, and that
the user authorizes network access.

Authorizing network access can be done in multiple ways:

  - enabling / uncommenting the ``network = on`` line of :ref:`proj-ini`
  - defining the :envvar:`PROJ_NETWORK` environment variable to ON
  - or using the :cpp:func:`proj_context_set_enable_network` with a
    ``enabled = TRUE`` value.

.. note::

    Instead of using the `libcurl` implementation, an application using the PROJ
    API can supply its own network implementation through C function callbacks
    with :cpp:func:`proj_context_set_network_callbacks`. Enabling network use
    must still be done with one of the above mentioned method.

Setting endpoint
----------------

When this is enabled, and a grid is not found in the various locations where
:ref:`resource files are looked for <resource_file_paths>`, PROJ will then
attempt at loading the file from a remote server, which defaults to
https://cdn.proj.org in :ref:`proj-ini`. This location can be changed with
the :envvar:`PROJ_NETWORK_ENDPOINT` environment variable or with
:cpp:func:`proj_context_set_url_endpoint`.

Caching
-------

To avoid repeated access to network, a local cache of downloaded chunks of grids
is implemented as SQLite3 database, :file:`cache.db`, stored in the
:ref:`PROJ user writable directory <user_writable_directory>`.

This local caching is enabled by default (can be changed in :ref:`proj-ini` or
with :cpp:func:`proj_grid_cache_set_enable`). The default maximum size of the
cache is 300 MB, which is more than half of the total size of grids available,
at time of writing. This size can also be customized in :ref:`proj-ini` or
with :cpp:func:`proj_grid_cache_set_max_size`

Download API
------------

When on-demand loading of grid is not desirable, the PROJ API also offers the
capability to download whole grids in the
:ref:`PROJ user writable directory <user_writable_directory>` by using the
:cpp:func:`proj_is_download_needed` and :cpp:func:`proj_download_file` functions.

Download utility
----------------

:ref:`projsync` is a tool for downloading resource files.


Mirroring
---------

If you are able, you are encouraged to mirror the grids via AWS S3 command line:

::

    aws s3 sync s3://cdn.proj.org .

If direct S3 access is not possible, you can also use wget to locally mirror the
data:

::

    wget --mirror https://cdn.proj.org/

Acknowledgments
---------------

The s3://cdn.proj.org bucket is hosted by the
`Amazon Public Datasets program <https://aws.amazon.com/opendata/public-datasets/>`_.
CDN services are provided by the AWS Public Dataset team via
`CloudFront <https://aws.amazon.com/cloudfront/>`_
