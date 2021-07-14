.. _rfc3:

====================================================================
PROJ RFC 3: Dependency management
====================================================================

:Author: Kristian Evers
:Contact: kreve@sdfe.dk
:Status: Adopted
:Last Updated: 2019-01-16

Summary
-------------------------------------------------------------------------------

This document defines a set of guidelines for dependency management in PROJ.
With PROJ being a core component in many downstream software packages clearly
stating which dependencies the library has is of great value. This document
concern both programming language standards as well as minimum required
versions of library dependencies and build tools.

It is proposed to adopt a rolling update scheme that ensures that PROJ is
sufficiently accessible, even on older systems, as well as keeping up with the
technological evolution. The scheme is divided in two parts, one concerning
versions of used programming languages within PROJ and the other concerning
software packages that PROJ depend on.

With adoption of this RFC, versions used for

1. programming languages will always be at least two revisions behind the most
   recent standard
2. software packages will always be at least two years old
   (patch releases are exempt)

A change in programming language standard can only be introduced with a new
major version release of PROJ. Changes for software package dependencies can be
introduced with minor version releases of PROJ. Changing the version
requirements for a dependency needs to be approved by the PSC.

Following the above rule set will ensure that all but the most conservative
users of PROJ will be able to build and use the most recent version of the
library.

In the sections below details concerning programming languages and software
dependencies are outlined. The RFC is concluded with a bootstrapping section
that details the state of dependencies after the accept of the RFC.


Background
-------------------------------------------------------------------------------

PROJ has traditionally been written in C89. Until recently, no formal
requirements of e.g. build systems has been defined and formally accepted by
the project. :ref:`RFC2 <rfc2>` formally introduces dependencies on C++11 and
SQLite 3.7.

In this RFC a rolling update of version or standard requirements is described.
The reasoning behind a rolling update scheme is that it has become increasingly
evident that C89 is becoming outdated and creating a less than optimal
development environment for contributors. It has been noted that the most
commonly used compilers all now support more recent versions of C, so the
strict usage of C89 is no longer as critical as it used to be.

Similarly, rolling updates to other tools and libraries that PROJ depend on
will ensure that the code base can be kept modern and in line with the rest of
the open source software ecosphere.


C and C++
-------------------------------------------------------------------------------

Following :ref:`RFC2 <rfc2>` PROJ is written in both C and C++. At the time of
writing the core library is C based and the code described in RFC2 is written
in C++. While the core library is mostly written in C it is compiled as C++.
Minor sections of PROJ, like the geodesic algorithms are still compiled as C
since there is no apparent benefit of compiling with a C++ compiler. This may
change in the future.

Both the C and C++ standards are updated with regular intervals. After an
update of a standard it takes time for compiler manufacturers to implement the
standards fully, which makes adaption of new standards potentially troublesome
if done too soon. On the other hand, waiting too long to adopt new standards
will eventually make the code base feel old and new contributors are more
likely to stay away because they don't want to work using tools of the past.
With a rolling update scheme both concerns can be managed by always staying
behind the most recent standard, but not so far away that potential
contributors are scared away. Keeping a policy of always lagging behind be two
iterations of the standard is thought to be the best comprise between the two
concerns.

C comes in four ISO standardised varieties: C89, C99, C11, C18. In this
document we refer to their informal names for ease of reading. C++ exists in
five varieties: C++98, C++03, C++11, C++14, C++17. Before adoption of this RFC
PROJ uses C89 and C++11. For C, that means that the used standard is three
iterations behind the most recent standard. C++ is two iterations behind.
Following the rules in this RFC the required C standard used in PROJ is at
allowed to be two iterations behind the most recent standard. That means that a
change to C99 is possible, as long as the PROJ PSC acknowledges such a change.

When a new standard for either C or C++ is released PROJ should consider
changing its requirement to the next standard in the line. For C++ that means a
change in standard roughly every three years, for C the periods between
standard updates is expected to be longer. Adaptation of new programming
language standards should be coordinated with a major version release of PROJ.


Software dependencies
-------------------------------------------------------------------------------

At the time of writing PROJ is dependent on very few external packages. In
fact only one runtime dependency is present: SQLite. Building PROJ also
requires one of two external dependencies for configuration: Autotools or
CMake.

As with programming language standards it is preferable that software
dependencies are a bit behind the most recent development. For this reason it
is required that the minimum version supported in PROJ dependencies is at least
two years old, preferably more. It is not a requirement that the minimum
version of dependencies is always kept strictly two years behind current
development, but it is allowed in case future development of PROJ warrants an
update. Changes in minimum version requirements are allowed to happen with
minor version releases of PROJ.

At the time of writing the minimum version required for SQLite it 3.7 which was
released in 2010. CMake currently is required to be at least at version 2.8.3
which was also released in 2010.


Bootstrapping
-------------------------------------------------------------------------------

This RFC comes with a set of guidelines for handling dependencies for PROJ in
the future. Up until now dependencies hasn't been handled consistently,  with
some dependencies not being approved formally by the projects governing body.
Therefore minimum versions of PROJ dependencies is proposed so that at the
acception of this RFC PROJ will have the following external requirements:

* C99 (was C89)
* C++11 (already approved in :ref:`RFC2 <rfc2>`)
* SQLite 3.7 (already approved in :ref:`RFC2 <rfc2>`)
* CMake 3.5 (was 2.8.3)


Adoption status
-------------------------------------------------------------------------------

The RFC was adopted on 2018-01-19 with +1's from the following PSC members

* Kristian Evers
* Even Rouault
* Thomas Knudsen
* Howard Butler
