.. _messagepack: https://msgpack.org/
.. |MessagePack| replace:: `MessagePack <messagepack_>`_

.. _mpack:       https://github.com/ludocode/mpack
.. |MPack|       replace:: `MPack <mpack_>`_
   
.. |Stroll|      replace:: :external+stroll:doc:`Stroll <index>`
   
.. |lvstr|       replace:: :external+stroll:ref:`sect-api-lvstr`

Overview
========

What follows here provides a thorough description of how to use DPack library.

Basically, DPack library is a C library wrapping the |MPack| library

C framework that provides definitions
usefull to carry out common C/C++ applicative tasks.
The library is implemented to run on GNU Linux / glibc platforms only (although
porting to alternate C library such as `musl libc <https://www.musl-libc.org/>`_
should not be much of a hassle).

Stroll library API is organized around the following functional areas which
you can refer to for further details :

* `Common definitions`_,
* Assertions_,
* `Bit operations`_,
* `Bitmaps`_,
* `Length-value strings`_.

.. index:: build configuration, configuration macros
   
Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final DPack build. From client code, you may
eventually refer to the corresponding C macros listed below:

* :c:macro:`CONFIG_DPACK_ASSERT_API`
* :c:macro:`CONFIG_DPACK_ASSERT_INTERN`
* :c:macro:`CONFIG_DPACK_DEBUG`
* :c:macro:`CONFIG_DPACK_SCALAR`
* :c:macro:`CONFIG_DPACK_FLOAT`
* :c:macro:`CONFIG_DPACK_DOUBLE`
* :c:macro:`CONFIG_DPACK_STRING`
* :c:macro:`CONFIG_DPACK_LVSTR`
* :c:macro:`CONFIG_DPACK_BIN`
* :c:macro:`CONFIG_DPACK_ARRAY`
* :c:macro:`CONFIG_DPACK_MAP`
* :c:macro:`CONFIG_DPACK_UTEST`
* :c:macro:`CONFIG_DPACK_VALGRIND`
* :c:macro:`CONFIG_DPACK_SAMPLE`

.. index:: common definitions, cdefs

Common definitions
==================

Stroll library exposes various C preprocessor macros used to implement Stroll
internals and meant for application development purposes. These are:

.. hlist::

   * Compile time logic :

      * :c:macro:`STROLL_CONCAT`
      * :c:macro:`STROLL_CONST_ABS`
      * :c:macro:`STROLL_CONST_MAX`
      * :c:macro:`STROLL_CONST_MIN`
      * :c:macro:`STROLL_STRING`
      * :c:macro:`STROLL_UNIQ`
      * :c:macro:`compile_assert`
      * :c:macro:`compile_choose`
      * :c:macro:`compile_eval`

   * Various

      * :c:macro:`array_nr`
      * :c:macro:`stroll_abs`
      * :c:macro:`stroll_min`
      * :c:macro:`stroll_max`

   * Attribute wrappers :

      * :c:macro:`__align`
      * :c:macro:`__const`
      * :c:macro:`__ctor`
      * :c:macro:`__dtor`
      * :c:macro:`__export_public`
      * :c:macro:`__export_protect`
      * :c:macro:`__leaf`
      * :c:macro:`__nonull`
      * :c:macro:`__noreturn`
      * :c:macro:`__nothrow`
      * :c:macro:`__packed`
      * :c:macro:`__printf`
      * :c:macro:`__pure`
      * :c:macro:`__returns_nonull`
      * :c:macro:`__unused`
      * :c:macro:`__warn_result`

.. index:: assertions

Assertions
==========

When compiled with the :c:macro:`CONFIG_STROLL_ASSERT` build configuration
option enabled, the Stroll library exposes the :c:macro:`stroll_assert` macro so
that developper may perform standard assertion checking.

.. index:: bit operations, bitops

Bit operations
==============

When compiled with the :c:macro:`CONFIG_STROLL_BOPS` build configuration
option enabled, the Stroll library provides support for bit manipulation
operations. These are:

.. hlist::

   * Find First bit Set:

      * :c:func:`stroll_bops_ffs`
      * :c:func:`stroll_bops32_ffs`
      * :c:func:`stroll_bops64_ffs`

   * Find Last bit Set:

      * :c:func:`stroll_bops_fls`
      * :c:func:`stroll_bops32_fls`
      * :c:func:`stroll_bops64_fls`

   * Find First bit Cleared:

      * :c:func:`stroll_bops_ffc`
      * :c:func:`stroll_bops32_ffc`
      * :c:func:`stroll_bops64_ffc`

   * Find number of set bits (:index:`Hammimg weight`):

      * :c:func:`stroll_bops_hweight`
      * :c:func:`stroll_bops32_hweight`
      * :c:func:`stroll_bops64_hweight`

.. index:: bitmaps, bmap

Bitmaps
=======

When compiled with the :c:macro:`CONFIG_STROLL_BMAP` build configuration
option enabled, the Stroll library provides support for bitmap operations.
These are:

.. hlist::

   * Initialization:

      * :c:macro:`STROLL_BMAP_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP_INIT_SET`
      * :c:macro:`STROLL_BMAP32_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP32_INIT_SET`
      * :c:macro:`STROLL_BMAP64_INIT_CLEAR`
      * :c:macro:`STROLL_BMAP64_INIT_SET`
      * :c:func:`stroll_bmap_setup_clear`
      * :c:func:`stroll_bmap32_setup_clear`
      * :c:func:`stroll_bmap64_setup_clear`
      * :c:func:`stroll_bmap_setup_set`
      * :c:func:`stroll_bmap32_setup_set`
      * :c:func:`stroll_bmap64_setup_set`

   * Iteration:

      * :c:macro:`stroll_bmap_foreach_clear`
      * :c:macro:`stroll_bmap32_foreach_clear`
      * :c:macro:`stroll_bmap64_foreach_clear`
      * :c:macro:`stroll_bmap_foreach_set`
      * :c:macro:`stroll_bmap32_foreach_set`
      * :c:macro:`stroll_bmap64_foreach_set`

   * Compute masks:

      * :c:func:`stroll_bmap_mask`
      * :c:func:`stroll_bmap32_mask`
      * :c:func:`stroll_bmap64_mask`

   * Compute number of bits set (:index:`Hammimg weight`):

      * :c:func:`stroll_bmap_hweight`
      * :c:func:`stroll_bmap32_hweight`
      * :c:func:`stroll_bmap64_hweight`

   * Perform bitwise AND operation:

      * :c:func:`stroll_bmap_and`
      * :c:func:`stroll_bmap_and_range`
      * :c:func:`stroll_bmap32_and`
      * :c:func:`stroll_bmap32_and_range`
      * :c:func:`stroll_bmap64_and`
      * :c:func:`stroll_bmap64_and_range`

   * Perform bitwise OR operation:

      * :c:func:`stroll_bmap_or`
      * :c:func:`stroll_bmap_or_range`
      * :c:func:`stroll_bmap32_or`
      * :c:func:`stroll_bmap32_or_range`
      * :c:func:`stroll_bmap64_or`
      * :c:func:`stroll_bmap64_or_range`

   * Perform bitwise XOR operation:

      * :c:func:`stroll_bmap_xor`
      * :c:func:`stroll_bmap_xor_range`
      * :c:func:`stroll_bmap32_xor`
      * :c:func:`stroll_bmap32_xor_range`
      * :c:func:`stroll_bmap64_xor`
      * :c:func:`stroll_bmap64_xor_range`

   * Test set bit(s):

      * :c:func:`stroll_bmap_test`
      * :c:func:`stroll_bmap_test_all`
      * :c:func:`stroll_bmap_test_mask`
      * :c:func:`stroll_bmap_test_range`
      * :c:func:`stroll_bmap32_test`
      * :c:func:`stroll_bmap32_test_all`
      * :c:func:`stroll_bmap32_test_mask`
      * :c:func:`stroll_bmap32_test_range`
      * :c:func:`stroll_bmap64_test`
      * :c:func:`stroll_bmap64_test_all`
      * :c:func:`stroll_bmap64_test_mask`
      * :c:func:`stroll_bmap64_test_range`

   * Set bit(s):

      * :c:func:`stroll_bmap_set`
      * :c:func:`stroll_bmap_set_mask`
      * :c:func:`stroll_bmap_set_range`
      * :c:func:`stroll_bmap_set_all`
      * :c:func:`stroll_bmap32_set`
      * :c:func:`stroll_bmap32_set_mask`
      * :c:func:`stroll_bmap32_set_range`
      * :c:func:`stroll_bmap32_set_all`
      * :c:func:`stroll_bmap64_set`
      * :c:func:`stroll_bmap64_set_mask`
      * :c:func:`stroll_bmap64_set_range`
      * :c:func:`stroll_bmap64_set_all`

   * Clear bit(s):

      * :c:func:`stroll_bmap_clear`
      * :c:func:`stroll_bmap_clear_mask`
      * :c:func:`stroll_bmap_clear_range`
      * :c:func:`stroll_bmap_clear_all`
      * :c:func:`stroll_bmap32_clear`
      * :c:func:`stroll_bmap32_clear_mask`
      * :c:func:`stroll_bmap32_clear_range`
      * :c:func:`stroll_bmap32_clear_all`
      * :c:func:`stroll_bmap64_clear`
      * :c:func:`stroll_bmap64_clear_mask`
      * :c:func:`stroll_bmap64_clear_range`
      * :c:func:`stroll_bmap64_clear_all`

   * Toggle bit(s):

      * :c:func:`stroll_bmap_toggle`
      * :c:func:`stroll_bmap_toggle_mask`
      * :c:func:`stroll_bmap_toggle_range`
      * :c:func:`stroll_bmap_toggle_all`
      * :c:func:`stroll_bmap32_toggle`
      * :c:func:`stroll_bmap32_toggle_mask`
      * :c:func:`stroll_bmap32_toggle_range`
      * :c:func:`stroll_bmap32_toggle_all`
      * :c:func:`stroll_bmap64_toggle`
      * :c:func:`stroll_bmap64_toggle_mask`
      * :c:func:`stroll_bmap64_toggle_range`
      * :c:func:`stroll_bmap64_toggle_all`

.. index:: length-value string, lvstr

Length-Value Strings
====================

When compiled with the :c:macro:`CONFIG_STROLL_LVSTR` build configuration option
enabled, the Stroll library provides support for :c:struct:`stroll_lvstr`
length-value strings.

This framework ease the management of C strings life-cycle. In addition,
it caches the length of string registered into it to mitigate client code string
length computation overhead.

The following manipulations are available:

.. hlist::

   * Static initialization:

      * :c:macro:`STROLL_LVSTR_INIT`
      * :c:macro:`STROLL_LVSTR_INIT_LEND`
      * :c:macro:`STROLL_LVSTR_INIT_NLEND`
      * :c:macro:`STROLL_LVSTR_INIT_NCEDE`

   * Initialization:

      * :c:func:`stroll_lvstr_init`
      * :c:func:`stroll_lvstr_init_cede`
      * :c:func:`stroll_lvstr_init_dup`
      * :c:func:`stroll_lvstr_init_lend`
      * :c:func:`stroll_lvstr_init_ncede`
      * :c:func:`stroll_lvstr_init_ndup`
      * :c:func:`stroll_lvstr_init_nlend`

   * C string registration:

      * :c:func:`stroll_lvstr_cede`
      * :c:func:`stroll_lvstr_drop`
      * :c:func:`stroll_lvstr_dup`
      * :c:func:`stroll_lvstr_lend`
      * :c:func:`stroll_lvstr_ncede`
      * :c:func:`stroll_lvstr_ndup`
      * :c:func:`stroll_lvstr_nlend`

   * Accessors:

      * :c:macro:`STROLL_LVSTR_LEN_MAX`
      * :c:func:`stroll_lvstr_cstr`
      * :c:func:`stroll_lvstr_len`

   * Finalization:

      * :c:func:`stroll_lvstr_fini`

.. index:: API reference, reference
   
Reference
=========

Configuration macros
--------------------

CONFIG_DPACK_ASSERT_API
***********************

.. doxygendefine:: CONFIG_DPACK_ASSERT_API
   
CONFIG_DPACK_ASSERT_INTERN
**************************
   
.. doxygendefine:: CONFIG_DPACK_ASSERT_INTERN
   
CONFIG_DPACK_DEBUG
******************

.. doxygendefine:: CONFIG_DPACK_DEBUG
   
CONFIG_DPACK_SCALAR
*******************

.. doxygendefine:: CONFIG_DPACK_SCALAR
   
CONFIG_DPACK_FLOAT
******************

.. doxygendefine:: CONFIG_DPACK_FLOAT
   
CONFIG_DPACK_DOUBLE
*******************

.. doxygendefine:: CONFIG_DPACK_DOUBLE
   
CONFIG_DPACK_STRING
*******************

.. doxygendefine:: CONFIG_DPACK_STRING
   
CONFIG_DPACK_LVSTR
******************

.. doxygendefine:: CONFIG_DPACK_LVSTR
   
CONFIG_DPACK_BIN
****************

.. doxygendefine:: CONFIG_DPACK_BIN
   
CONFIG_DPACK_ARRAY
******************

.. doxygendefine:: CONFIG_DPACK_ARRAY
   
CONFIG_DPACK_MAP
****************

.. doxygendefine:: CONFIG_DPACK_MAP
   
CONFIG_DPACK_UTEST
******************

.. doxygendefine:: CONFIG_DPACK_UTEST
   
CONFIG_DPACK_VALGRIND
*********************

.. doxygendefine:: CONFIG_DPACK_VALGRIND
   
CONFIG_DPACK_SAMPLE
*******************

.. doxygendefine:: CONFIG_DPACK_SAMPLE
   
Macros
------

Structures
----------

dpack_decoder
*************

.. doxygenstruct:: dpack_decoder

dpack_encoder
*************

.. doxygenstruct:: dpack_encoder

Functions
---------

