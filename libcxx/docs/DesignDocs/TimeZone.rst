=================
Time Zone Support
=================

.. warning::
   The implementation of time zone support is under development. The
   implementation has not implemented the Standard wording nor is it ABI
   stable.

Introduction
============

Starting with C++20 the ``<chrono>`` library has support for time zones
available in the
`IANA database <https://data.iana.org/time-zones/tz-link.html>`_.
This page describes the design decisions and trade-offs made to implement this
feature. This page contains several links with more information regarding the
contents of the IANA database, this page assumes the reader is familiar with
this information.


Why use the Database files instead of a system API?
===================================================

Howard Hinnant, the author of the time zone support papers in the C++ Standard,
has written his own implementation and decided to write a parser for the IANA
database. His `motivations <https://howardhinnant.github.io/date/tz.html>`_
were:

- version will have the value "unknown" on Linux.
- links is missing as this version of the database makes no distinction
  between links and time zones.
- leaps will be missing on those platforms that do not ship with this
  information, which includes macOS and iOS.

Based on these observations it seems not all required information is available
on all platforms libc++ supports. Using a custom parser has a few more advantages:

- It is possible to write the code portable on all supported platforms. When
  using a system API every platform needs its own code. Platform dependent code
  is harder to maintain, especially when the maintainer does not have access to
  all supported platforms. It also makes it harder to support new platforms.
- The Standard `suggests <http://eel.is/c++draft/time.zone#db.remote-1>`_
  implementations to have a way to update the database run-time. This requires
  parsing a IANA database `Updating runtime`_.


Database
========

The IANA databases can be `downloaded
<https://data.iana.org/time-zones/releases/>`_. The files are public domain.

Format
------

There are some sources of information regarding the database format

 - `IANA documentation <https://data.iana.org/time-zones/tz-how-to.html>`_
 - The `man page <https://man7.org/linux/man-pages/man8/zic.8.html>`_ of zic

However there's no formal specification. Libc++'s parser does not implement all
allowed features, but it implements all features used in the database files.
Some of the missing features are:

- English words are allowed to be case insensitive and abbreviated to the
  minimal prefix. For example all valid lower case spellings of maximum are:
  ``ma``, ``max``, ``maxi``, ``maxim``, ``maximu``, and ``maximum``. In the
  database only ``max`` is used so that is what is implemented.
- There is not strict ordering in the file, except for continuation lines. This
  means a ``Zone`` entry can refer to a ``Rule`` entry not defined. This
  feature is not used, all ``Rule`` entries are defined before being used in a
  ``Zone`` entry. So the parser does not implement this feature.
- A large set of white space characters are allowed to separate the ``fields``
  on a ``line``. The set includes the carriage return and line feed. In
  practice tabs are used as separator and alignment is done with spaces. The
  parser only accepts these two characters.

Supported versions
------------------

On the IANA website the earliest database still available is from 2016.
Implementation of the time zone in ``<chrono>`` started on September 23th 2022,
which coincidentally was the date of the ``2022d`` database was released.
Older release might work, but this has not been tested. The libc++ team will
monitor newer releases of the database and ensure these releases keep working
with libc++.

Updating runtime
----------------

The Standard `suggests <http://eel.is/c++draft/time.zone#db.remote-1>`_ to have
a way to update the database runtime. There are few ways to update the database:

- Use a newer system version of the database. This means when an application
  wants to use a newer version of the database they depend on their operating
  system vendor to publish a new version and on their system administrator to
  update the database.
- Use a new IANA database as input. This avoids the above dependencies and
  makes it possible for applications to use a new database as soon as it
  becomes available.

The latter method seems easier to use so that is the approach libc++ uses.

The Standard does not specify how an implementation updates their remote
database. Howard's implementation uses cURL to download the files from the IANA
website. Using this approach would mean libc++ has a direct way to connect
itself with the Internet. When libc++ is the system library it means
applications running with system administrator privilege are now Internet
capable. This does not sound good from a security point of view. Instead libc++
will rely on the application to download a new database and load it as a remote
database.

.. note::
   Updating the database has not been implemented yet. The proper update
   procedure will be documented in the
   :ref:`user documentation <using-libcxx>`.

Distribution
------------

.. note:: How to distribute the files has not been determined yet.

Implementation
==============

.. note::
   The implementation is in a preliminary stage and some of the details may
   change. When that happens the documentation should be updated. The current
   status means:

   * part of the implementation deviates from the Standard, by omitting class
     members.
   * part of the parser uses stubs and store the contents in a string instead
     of properly parsing them.
   * some valid records are omitted, due to lack of support for that specific
     variant.

Database entries
----------------

The databases contains the following types of files:

- ``version`` contains the version number of the database.
- ``time zone files`` these are several files containing time zone information.
  The exact names of all time zone files are found in the implementation. These
  files contain the following record types:

  * ``Rule`` contains a rule entry.
  * ``Zone`` contains a time zone entry and has zero or more continuation entries.
  * ``Link`` contains a time zone link entry.

- ``leapseconds`` contains the leap second table. The file contains the
  following record types:

   * ``Leap`` contains a leap seconds entry.
   * ``Expires`` contains the file's expiration date.

Rule
~~~~

The usage of the ``Rule`` entries is not specified in the Standard. A ``Zone``
can refer to a ``Rule``. A ``Rule`` is a set of one or more ``Rule`` entries
sharing the same same. This name is used in the ``Zone``. The rules are stored
in a ``vector of vector`` where the outer ``vector`` has one entry per name.
The second vector contains all entries with the same name. The data is stored
in order of appearance. The ``vector`` s are not sorted, this means their index
is stable. During parsing the link between the name and index are stored in a
temporary ``map`` and the ``Zone`` uses the index instead of the name. This
saves memory space and makes to lookup of the entries O(1).

These entries are stored in a ``std::chrono::__time_zone_rule`` record.

Zone
~~~~

A ``Zone`` has a unique name for a time zone. A ``Zone`` has zero or more
continuation elements. All these elements are stored in the same record.

These entries are stored in a ``std::chrono::time_zone`` record.

Link
~~~~

A ``Link`` has a unique name and contains aliases for a ``Zone``. Typically a
``Link`` refers to an already parsed ``Zone``. However this is not always the
case. The file ``backward`` is currently parsed before ``europe`` but has the
alias ``Atlantic/Jan_Mayen`` for ``Europe/Berlin``.

These entries are stored in a ``std::chrono::time_zone_link`` record.

.. note::
   It seems the continental files contain no "forward" links. So maybe it is
   better to parse the special files later.

Leap
~~~~

A ``Leap`` contains a leap second entry. The entries in the input are sorted on
the date of occurrence. The records support negative leap seconds, however these
have not occurred as of yet (September 2022). They allow leap seconds to occur
are any given date, however the
`rules governing <https://en.wikipedia.org/wiki/Leap_seconds#Insertion_of_leap_seconds>`_
leap seconds have stricter requirements.

These entries are stored in a ``std::chrono::leap_second`` record.

Expires
~~~~~~~

In the current version of the database (2022d) there is a commented out
``Expires`` entry. The Standard does not specify what to do when the expiration
date has passed.

.. note::
   The current parser cannot handle this entry yet.


Undocumented extensions
-----------------------

There are some extension which are not documented. These extensions may become
documented in the future.

__make_tzdb
~~~~~~~~~~~

There are two overloads of this function:

- One taking a single ``std::string_view``. This version expects the path to a
  directory containing an unpacked IANA database. This function has a hard
  coded list of the files to parse. This function can be used to load a
  downloaded database. This makes the function useful to update a remote
  database.
- The other taking several ``istream``. This function is unaware of the exact
  details of the files in the database and just loads the data. By using an
  abstract ``istream`` it is easy to use a custom database. This is used in the
  low-level tests. Depending on how the database is shipped with libc++ it
  might be useful to load the shipped database.
