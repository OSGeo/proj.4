.. _gie:

================================================================================
gie
================================================================================

.. Index:: gie

.. only:: html

    The Geospatial Integrity Investigation Environment

Synopsis
********

    **gie** [ **-hovql** [ args ] ] file[s]

Description
***********

:program:`gie`, the Geospatial Integrity Investigation Environment, is a
regression testing environment for the PROJ transformation library. Its primary
design goal is to be able to perform regression testing of code that are a part
of PROJ, while not requiring any other kind of tooling than the same C compiler
already employed for compiling the library.

.. option:: -h, --help

    Print usage information

.. option:: -o <file>, --output <file>

    Specify output file name

.. option:: -v, --verbose

    Verbose: Provide non-essential informational output. Repeat :option:`-v` for
    more verbosity (e.g. ``-vv``)

.. option:: -q, --quiet

    Quiet: Opposite of verbose. In quiet mode not even errors are
    reported. Only interaction is through the return code (0 on success,
    non-zero indicates number of FAILED tests)

.. option:: -l, --list

    List the PROJ internal system error codes

.. option:: --version

    Print version number

Tests for :program:`gie` are defined in simple text files. Usually having the
extension ``.gie``. Test for :program:`gie` are written in the purpose-build command language for gie.
The basic functionality of the gie command language is implemented through just
3 command verbs: ``operation``, which defines the PROJ operation to test,
``accept``, which defines the input coordinate to read, and ``expect``, which
defines the result to expect.

A sample test file for :program:`gie` that uses the three above basic commands looks
like:

.. code-block:: console

    <gie>

    --------------------------------------------
    Test output of the UTM projection
    --------------------------------------------
    operation  +proj=utm  +zone=32  +ellps=GRS80
    --------------------------------------------
    accept     12  55
    expect     691_875.632_14   6_098_907.825_05

    </gie>

Parsing of a :program:`gie` file starts at ``<gie>`` and ends when ``</gie>``
is reached. Anything before ``<gie>`` and after ``</gie>`` is not considered.
Test cases are created by defining an :option:`operation` which
:option:`accept` an input coordinate and :option:`expect` an output
coordinate.

Because :program:`gie` tests are wrapped in the ``<gie>``/``</gie>`` tags it is
also possible to add test cases to custom made :ref:`init files <init_files>`.
The tests will be ignore by PROJ when reading the init file with *+init* and
:program:`gie` ignores anything not wrapped in ``<gie>``/``</gie>``.

:program:`gie` tests are defined by a set of commands like :option:`operation`,
:option:`accept` and :option:`expect` in the example above. Together the
commands make out the :program:`gie` command language. Any line in a
:program:`gie` file that does not start with a command is ignored. In the
example above it is seen how this can be used to add comments and styling to
:program:`gie` test files in order to make them more readable as well as
documenting what the purpose of the various tests are.

Below the :ref:`gie_commands` is explained in details.

Examples
*********

1. Run all tests in a file with all debug information turned on

.. code-block:: console

    gie -vvvv corner-cases.gie

2. Run all tests in several files

.. code-block:: console

    gie foo bar

.. _gie_commands:

gie command language
********************


.. option:: operation <+args>

    Define a PROJ operation to test. Example:

    .. code-block:: console

        operation proj=utm zone=32 ellps=GRS80
        # test 4D function
        accept    12 55 0 0
        expect    691875.63214  6098907.82501  0  0

        # test 2D function
        accept    12 56
        expect    687071.4391   6210141.3267

.. option:: accept <x y [z [t]]>

    Define the input coordinate to read. Takes test coordinate. The coordinate
    can be defined by either 2, 3 or 4 values, where the first two values are
    the x- and y-components, the 3rd is the z-component and the 4th is the time
    component. The number of components in the coordinate determines which
    version of the operation is tested (2D, 3D or 4D). Many coordinates can be
    accepted for one :option:`operation`. For each :option:`accept` an
    accompanying :option:`expect` is needed.

    Note that :program:`gie` accepts the underscore (``_``) as a thousands
    separator. It is not required (in fact, it is entirely ignored by the
    input routine), but it significantly improves the readability of the very
    long strings of numbers typically required in projected coordinates.


    See :option:`operation` for an example.

.. option:: expect <x y [z [t]]> | <error code>

    Define the expected coordinate that will be returned from accepted
    coordinate passed though an operation. The expected coordinate can be
    defined by either 2, 3 or 4 components, similarly to :option:`accept`.
    Many coordinates can be expected for one :option:`operation`. For each
    :option:`expect` an accompanying :option:`accept` is needed.

    See :option:`operation` for an example.

    In addition to expecting a coordinate it is also possible to expect a
    PROJ error code in case an operation can't be created. This is useful when
    testing that errors are caught and handled correctly. Below is an example of
    that tests that the pipeline operator fails correctly when a non-invertible
    pipeline is constructed.

    .. code-block:: console

        operation   proj=pipeline step
                    proj=urm5 n=0.5 inv
        expect      failure pjd_err_malformed_pipeline

    See :option:`gie --list<--list>` for a list of error codes that can be expected.

.. option:: tolerance <tolerance>

    The :option:`tolerance` command controls how much accepted coordinates
    can deviate from the expected coordinate. This is handy to test that an
    operation meets a certain numerical tolerance threshold. Some operations
    are expected to be accurate within millimeters where others might only be
    accurate within a few meters. :option:`tolerance` should

    .. code-block:: console

        operation       proj=merc
        # test coordinate as returned by ```echo 12 55 | proj +proj=merc``
        tolerance       1 cm
        accept          12 55
        expect          1335833.89 7326837.72

        # test that the same coordinate with a 50 m false easting as determined
        # by ``echo 12 55 |proj +proj=merc +x_0=50`` is still within a 100 m
        # tolerance of the unaltered coordinate from proj=merc
        tolerance       100 m
        accept          12 55
        expect          1335883.89  7326837.72

    The default tolerance is 0.5 mm. See :option:`proj -lu` for a list of possible
    units.

.. option:: roundtrip <n> <tolerance>

    Do a roundtrip test of an operation. :option:`roundtrip` needs a
    :option:`operation` and a :option:`accept` command
    to function. The accepted coordinate is passed to the operation first in
    it's forward mode, then the output from the forward operation is passed
    back to the inverse operation. This procedure is done ``n`` times. If the
    resulting coordinate is within the set tolerance of the initial coordinate,
    the test is passed.

    Example with the default 100 iterations and the default tolerance:

    .. code-block:: console

        operation       proj=merc
        accept          12 55
        roundtrip

    Example with count and default tolerance:

    .. code-block:: console

        operation       proj=merc
        accept          12 55
        roundtrip       10000

    Example with count and tolerance:

    .. code-block:: console

        operation       proj=merc
        accept          12 55
        roundtrip       10000 5 mm

.. option:: direction <direction>

    The :option:`direction` command specifies in which direction an operation
    is performed. This can either be ``forward`` or ``inverse``. An example of
    this is seen below where it is tested that a symmetrical transformation
    pipeline returns the same results in both directions.

    .. code-block:: console

        operation proj=pipeline zone=32 step
                  proj=utm  ellps=GRS80 step
                  proj=utm  ellps=GRS80 inv
        tolerance 0.1 mm

        accept 12 55 0 0
        expect 12 55 0 0

        # Now the inverse direction (still same result: the pipeline is symmetrical)

        direction inverse
        expect 12 55 0 0

    The default direction is "forward".

.. option:: ignore <error code>

    This is especially
    useful in test cases that rely on a grid that is not guaranteed to be
    available. Below is an example of that situation.

    .. code-block:: console

        operation proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80
        tolerance 1 mm
        ignore    pjd_err_failed_to_load_grid
        accept    172.999892181021551 -45.001620431954613
        expect    173                 -45


    See :option:`gie --list<--list>` for a list of error codes that can be ignored.

.. option:: require_grid <grid_name>

    Checks the availability of the grid <grid_name>. If it is not found, then
    all :option:`accept`/:option:`expect` pairs until the next
    :option:`operation` will be skipped.
    :option:`require_grid` can be repeated several times to specify several grids whose
    presence is required.

.. option:: echo <text>

    Add user defined text to the output stream. See the example below.

    .. code-block:: console

        <gie>
        echo ** Mercator projection tests **
        operation +proj=merc
        accept  0   0
        expect  0   0
        </gie>

    which returns

    .. code-block:: console

        -------------------------------------------------------------------------------
        Reading file 'test.gie'
        ** Mercator projection test **
        -------------------------------------------------------------------------------
        total:  1 tests succeeded,  0 tests skipped,  0 tests failed.
        -------------------------------------------------------------------------------

.. option:: skip

    Skip any test after the first occurrence of :option:`skip`. In the example below only
    the first test will be performed. The second test is skipped. This feature is mostly
    relevant for debugging when writing new test cases.

    .. code-block:: console

        <gie>
        operation proj=merc
        accept  0   0
        expect  0   0
        skip
        accept  0   1
        expect  0   110579.9
        </gie>


Strict mode
***********

.. versionadded:: 7.1

A stricter variant of normal gie syntax can be used by wrapping gie commands
between ``<gie-strict>`` and ``</gie-strict>``. In strict mode, comment lines
must start with a sharp character. Unknown commands will be considered as an error.
A command can still be split on several lines, but intermediate lines must
end with the space character followed by backslash to mark the continuation.

    .. code-block:: console

        <gie-strict>
        # This is a comment. The following line with multiple repeated characters too
        -------------------------------------------------
        # A command on several lines must use " \" continuation
        operation proj=hgridshift +grids=nzgd2kgrid0005.gsb \
                  ellps=GRS80
        tolerance 1 mm
        ignore    pjd_err_failed_to_load_grid
        accept    172.999892181021551 -45.001620431954613
        expect    173                 -45
        </gie-strict>


Background
**********

More importantly than being an acronym for "Geospatial Integrity Investigation
Environment", gie were also the initials, user id, and USGS email address of
Gerald Ian Evenden (1935--2016), the geospatial visionary, who, already in the
1980s, started what was to become the PROJ of today.

Gerald's clear vision was that map projections are *just special functions*.
Some of them rather complex, most of them of two variables, but all of them
*just special functions*, and not particularly more special than the :c:func:`sin()`,
:c:func:`cos()`, :c:func:`tan()`, and :c:func:`hypot()` already available in the C standard library.

And hence, according to Gerald, *they should not be particularly much harder
to use*, for a programmer, than the :c:func:`sin()`'s, :c:func:`tan()`'s and
:c:func:`hypot()`'s so readily available.

Gerald's ingenuity also showed in the implementation of the vision, where
he devised a comprehensive, yet simple, system of key-value pairs for
parameterising a map projection, and the highly flexible :c:type:`PJ` struct, storing
run-time compiled versions of those key-value pairs, hence making a map
projection function call, ``pj_fwd(PJ, point)``, as easy as a traditional function
call like ``hypot(x,y)``.

While today, we may have more formally well defined metadata systems (most
prominent the OGC WKT2 representation), nothing comes close being as easily
readable ("human compatible") as Gerald's key-value system. This system in
particular, and the PROJ system in general, was Gerald's great gift to anyone
using and/or communicating about geodata.

It is only reasonable to name a program, keeping an eye on the
integrity of the PROJ system, in honour of Gerald.

So in honour, and hopefully also in the spirit, of Gerald Ian Evenden
(1935--2016), this is the Geospatial Integrity Investigation Environment.


.. only:: man

    See also
    ********

    **proj(1)**, **cs2cs(1)**, **cct(1)**, **geod(1)**, **projinfo(1)**, **projsync(1)**

    .. include:: common_man.rst
