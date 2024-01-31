.. SPDX-License-Identifier: GPL-3.0-only

   This file is part of DPack.
   Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

.. |MessagePack format|        replace:: :doc:`msgpack`
.. |MessagePack bool format|   replace:: :ref:`MessagePack boolean format <sect-msgpack-bool>`
.. |MessagePack int format|    replace:: :ref:`MessagePack integer format <sect-msgpack-int>`
.. |MessagePack float format|  replace:: :ref:`MessagePack float format <sect-msgpack-float>`
.. |MessagePack string format| replace:: :ref:`MessagePack string format <sect-msgpack-str>`
.. |MessagePack bin format|    replace:: :ref:`MessagePack bin format <sect-msgpack-bin>`
.. |MessagePack nil format|    replace:: :ref:`MessagePack nil format <sect-msgpack-nil>`
.. |MessagePack array format|  replace:: :ref:`MessagePack array format <sect-msgpack-array>`
.. |MessagePack map format|    replace:: :ref:`MessagePack map format <sect-msgpack-map>`

.. _mpack:                     https://github.com/ludocode/mpack
.. |MPack|                     replace:: `MPack <mpack_>`_

.. |Stroll|                    replace:: :external+stroll:doc:`Stroll <index>`
.. |Stroll's API guide|        replace:: :external+stroll:doc:`Stroll's API guide <api>`

.. |lvstr|                     replace:: :external+stroll:ref:`lvstr <sect-api-lvstr>`
.. |stroll_lvstr_fini|         replace:: :external+stroll:ref:`sect-api-stroll_lvstr_fini`
.. |stroll_lvstr_drop|         replace:: :external+stroll:ref:`sect-api-stroll_lvstr_drop`

.. _glibc:                     https://www.gnu.org/s/libc/
.. |GLibc|                     replace:: `GNU C library <glibc_>`_

.. _glibc_nan_infinity:        https://www.gnu.org/software/libc/manual/html_node/Infinity-and-NaN.html
.. |NaN|                       replace:: `NaN <glibc_nan_infinity_>`_
.. |Infinity|                  replace:: `Infinity <glibc_nan_infinity_>`_

Overview
========

What follows here provides a thorough description of how to use DPack_ library.

Basically, DPack_ is a C library that wraps calls to |MPack| to (de)serialize
objects according to |MessagePack format|. As stated onto the home page:

   It's like JSON but fast and small.

   |MessagePack| is an efficient binary serialization format. It lets you
   exchange data among multiple languages like JSON. But it's faster and
   smaller. Small integers are encoded into a single byte, and typical short
   strings require only one extra byte in addition to the strings themselves.

You may find a copy of the |MessagePack| format specification into section
:doc:`msgpack`.

The library is implemented to run on GNU Linux / |GLibc| platforms only
(although porting to alternate C library such as `musl libc
<https://www.musl-libc.org/>`_ should not be much of a hassle).

DPack_ sources are distributed under the :ref:`GNU Lesser General Public License
<lgpl>` whereas documentation manuals are distributed under the :ref:`GNU
General Public License <gpl>`.

DPack_ library API is organized around the following functional areas which
you can refer to for further details :

* Encoder_,
* Decoder_,
* Boolean_,
* Integer_,
* `Floating point number`_,
* String_,
* `Length-Value string`_,
* Bin_,
* Array_,
* Map_.

.. index:: build configuration, configuration macros

Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final DPack_ build. From client code, you may
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

.. index:: encode, serialize, pack

Encoder
=======

The DPack_ library serialization interface is provided through to the
:c:struct:`dpack_encoder` interface. The following operations are available:

* :c:func:`dpack_encoder_init_buffer`
* :c:func:`dpack_encoder_fini`
* :c:func:`dpack_encoder_space_used`
* :c:func:`dpack_encoder_space_left`

You *MUST* include :file:`dpack/codec.h` header to use this interface.

.. index:: decode, unserialize, unpack

Decoder
=======

The DPack_ library unserialization interface is provided through to the
:c:struct:`dpack_decoder` interface. The following operations are available:

* :c:func:`dpack_decoder_init_buffer`
* :c:func:`dpack_decoder_init_skip_buffer`
* :c:func:`dpack_decoder_fini`
* :c:func:`dpack_decoder_data_left`
* :c:func:`dpack_decoder_skip`

You *MUST* include :file:`dpack/codec.h` header to use this interface.

.. index:: boolean, bool

.. _bool:
.. _sect-api-bool:

Boolean
=======

When compiled with the :c:macro:`CONFIG_DPACK_SCALAR` build configuration
option enabled, the DPack_ library provides support for boolean
(de)serialization operations according to the |MessagePack bool format|.

Available operations are:

* :c:macro:`DPACK_BOOL_SIZE`
* :c:func:`dpack_encode_bool`
* :c:func:`dpack_decode_bool`

You *MUST* include :file:`dpack/scalar.h` header to use this interface.

.. index:: integers, signed, unsigned, 8-bits, 16-bits, 32-bits, 64-bits

.. _int:
.. _sect-api-int:

Integer
========

When compiled with the :c:macro:`CONFIG_DPACK_SCALAR` build configuration
option enabled, the DPack_ library provides support for integer
(de)serialization operations according to the |MessagePack int format|.

Available operations are:

.. hlist::

   * 8-bits unsigned integers:

      * :c:macro:`DPACK_UINT8_SIZE_MIN`
      * :c:macro:`DPACK_UINT8_SIZE_MAX`
      * :c:func:`dpack_encode_uint8`
      * :c:func:`dpack_decode_uint8`
      * :c:func:`dpack_decode_uint8_min`
      * :c:func:`dpack_decode_uint8_max`
      * :c:func:`dpack_decode_uint8_range`

   * 8-bits signed integers:

      * :c:macro:`DPACK_INT8_SIZE_MIN`
      * :c:macro:`DPACK_INT8_SIZE_MAX`
      * :c:func:`dpack_encode_int8`
      * :c:func:`dpack_decode_int8`
      * :c:func:`dpack_decode_int8_min`
      * :c:func:`dpack_decode_int8_max`
      * :c:func:`dpack_decode_int8_range`

   * 16-bits unsigned integers:

      * :c:macro:`DPACK_UINT16_SIZE_MIN`
      * :c:macro:`DPACK_UINT16_SIZE_MAX`
      * :c:func:`dpack_encode_uint16`
      * :c:func:`dpack_decode_uint16`
      * :c:func:`dpack_decode_uint16_min`
      * :c:func:`dpack_decode_uint16_max`
      * :c:func:`dpack_decode_uint16_range`

   * 16-bits signed integers:

      * :c:macro:`DPACK_INT16_SIZE_MIN`
      * :c:macro:`DPACK_INT16_SIZE_MAX`
      * :c:func:`dpack_encode_int16`
      * :c:func:`dpack_decode_int16`
      * :c:func:`dpack_decode_int16_min`
      * :c:func:`dpack_decode_int16_max`
      * :c:func:`dpack_decode_int16_range`

   * 32-bits unsigned integers:

      * :c:macro:`DPACK_UINT32_SIZE_MIN`
      * :c:macro:`DPACK_UINT32_SIZE_MAX`
      * :c:func:`dpack_encode_uint32`
      * :c:func:`dpack_decode_uint32`
      * :c:func:`dpack_decode_uint32_min`
      * :c:func:`dpack_decode_uint32_max`
      * :c:func:`dpack_decode_uint32_range`

   * 32-bits signed integers:

      * :c:macro:`DPACK_INT32_SIZE_MIN`
      * :c:macro:`DPACK_INT32_SIZE_MAX`
      * :c:func:`dpack_encode_int32`
      * :c:func:`dpack_decode_int32`
      * :c:func:`dpack_decode_int32_min`
      * :c:func:`dpack_decode_int32_max`
      * :c:func:`dpack_decode_int32_range`

   * 64-bits unsigned integers:

      * :c:macro:`DPACK_UINT64_SIZE_MIN`
      * :c:macro:`DPACK_UINT64_SIZE_MAX`
      * :c:func:`dpack_encode_uint64`
      * :c:func:`dpack_decode_uint64`
      * :c:func:`dpack_decode_uint64_min`
      * :c:func:`dpack_decode_uint64_max`
      * :c:func:`dpack_decode_uint64_range`

   * 64-bits signed integers:

      * :c:macro:`DPACK_INT64_SIZE_MIN`
      * :c:macro:`DPACK_INT64_SIZE_MAX`
      * :c:func:`dpack_encode_int64`
      * :c:func:`dpack_decode_int64`
      * :c:func:`dpack_decode_int64_min`
      * :c:func:`dpack_decode_int64_max`
      * :c:func:`dpack_decode_int64_range`

   * Unsigned integers:

      * :c:macro:`DPACK_UINT_SIZE_MIN`
      * :c:macro:`DPACK_UINT_SIZE_MAX`
      * :c:func:`dpack_encode_uint`
      * :c:func:`dpack_decode_uint`
      * :c:func:`dpack_decode_uint_min`
      * :c:func:`dpack_decode_uint_max`
      * :c:func:`dpack_decode_uint_range`

   * Signed integers:

      * :c:macro:`DPACK_INT_SIZE_MIN`
      * :c:macro:`DPACK_INT_SIZE_MAX`
      * :c:func:`dpack_encode_int`
      * :c:func:`dpack_decode_int`
      * :c:func:`dpack_decode_int_min`
      * :c:func:`dpack_decode_int_max`
      * :c:func:`dpack_decode_int_range`

   * :c:macro:`DPACK_STDINT_SIZE_MIN`
   * :c:macro:`DPACK_STDINT_SIZE_MAX`

You *MUST* include :file:`dpack/scalar.h` header to use this interface.

.. index:: float, double, floating point number

.. _float:
.. _sect-api-float:

Floating point number
=====================

When compiled with the :c:macro:`CONFIG_DPACK_FLOAT` build configuration
option enabled, the DPack_ library provides support for single precision
floating point numbers (de)serialization operations according to the
|MessagePack float format|.

Available operations are:

* :c:macro:`DPACK_FLOAT_SIZE`
* :c:func:`dpack_encode_float`
* :c:func:`dpack_decode_float`
* :c:func:`dpack_decode_float_min`
* :c:func:`dpack_decode_float_max`
* :c:func:`dpack_decode_float_range`

When compiled with the :c:macro:`CONFIG_DPACK_DOUBLE` build configuration
option enabled, the DPack_ library provides support for double precision
floating point numbers (de)serialization operations according to the
|MessagePack float format|.

Available operations are:

* :c:macro:`DPACK_DOUBLE_SIZE`
* :c:func:`dpack_encode_double`
* :c:func:`dpack_decode_double`
* :c:func:`dpack_decode_double_min`
* :c:func:`dpack_decode_double_max`
* :c:func:`dpack_decode_double_range`

You *MUST* include :file:`dpack/scalar.h` header to use these interfaces.

.. index:: nil, null

.. _nil:
.. _sect-api-nil:

Nil
===

When compiled with the :c:macro:`CONFIG_DPACK_SCALAR` build configuration
option enabled, the DPack_ library provides support for nil / null
(de)serialization operations according to the |MessagePack nil format|.

Available operations are:

* :c:macro:`DPACK_NIL_SIZE`
* :c:func:`dpack_encode_nil`
* :c:func:`dpack_decode_nil`

You *MUST* include :file:`dpack/scalar.h` header to use this interface.

.. index:: string

.. _string:
.. _sect-api-string:

String
======

When compiled with the :c:macro:`CONFIG_DPACK_STRING` build configuration option
enabled, the DPack_ library provides support for string (de)serialization
according to the |MessagePack string format|.

Available operations are:

.. hlist::

   * string serialization utilities:

      * :c:macro:`DPACK_STRLEN_MAX`
      * :c:macro:`DPACK_STR_SIZE()`
      * :c:func:`dpack_str_size`

   * string encoding:

      * :c:func:`dpack_encode_str`
      * :c:func:`dpack_encode_str_fix`

   * string decoding with allocation:

      * :c:func:`dpack_decode_strdup`
      * :c:func:`dpack_decode_strdup_equ`
      * :c:func:`dpack_decode_strdup_max`
      * :c:func:`dpack_decode_strdup_range`

   * string decoding with copy:

      * :c:func:`dpack_decode_strcpy`
      * :c:func:`dpack_decode_strcpy_equ`
      * :c:func:`dpack_decode_strcpy_range`

You *MUST* include :file:`dpack/string.h` header to use these interfaces.

.. index:: Length-Value string, lvstr

.. _lvstr:
.. _sect-api-lvstr:

Length-Value string
===================

When compiled with the :c:macro:`CONFIG_DPACK_LVSTR` build configuration option
enabled, the DPack_ library provides support for
:external+stroll:ref:`Length-Value string <sect-api-lvstr>`
(de)serialization according to the |MessagePack string format|.

Available operations are:

.. hlist::

   * lvstr serialization utilities:

      * :c:macro:`DPACK_LVSTRLEN_MAX`
      * :c:macro:`DPACK_LVSTR_SIZE()`
      * :c:func:`dpack_lvstr_size`

   * lvstsr encoding:

      * :c:func:`dpack_encode_lvstr`

   * string decoding:

      * :c:func:`dpack_decode_lvstr`
      * :c:func:`dpack_decode_lvstr_equ`
      * :c:func:`dpack_decode_lvstr_max`
      * :c:func:`dpack_decode_lvstr_range`

You *MUST* include :file:`dpack/lvstr.h` header to use these interfaces.

.. index:: bin, blob, byte array

.. _bin:
.. _sect-api-bin:

Bin
===

When compiled with the :c:macro:`CONFIG_DPACK_BIN` build configuration option
enabled, the DPack_ library provides support for bin (de)serialization
operations.

*Bins* are binary objects / bytes array that |MessagePack| can serialize
according to the |MessagePack bin format|.

Available operations are:

.. hlist::

   * bin serialization utilities:

      * :c:macro:`DPACK_BINSZ_MAX`
      * :c:macro:`DPACK_BIN_SIZE()`
      * :c:func:`dpack_bin_size`

   * bin encoding:

      * :c:func:`dpack_encode_bin`

   * bin decoding with allocation:

      * :c:func:`dpack_decode_bindup`
      * :c:func:`dpack_decode_bindup_equ`
      * :c:func:`dpack_decode_bindup_max`
      * :c:func:`dpack_decode_bindup_range`

   * bin decoding with copy:

      * :c:func:`dpack_decode_bincpy`
      * :c:func:`dpack_decode_bincpy_equ`
      * :c:func:`dpack_decode_bincpy_range`

You *MUST* include :file:`dpack/bin.h` header to use these interfaces.

.. index:: list, array, collection

.. _array:
.. _sect-api-array:

Array
=====

When compiled with the :c:macro:`CONFIG_DPACK_ARRAY` build configuration option
enabled, the DPack_ library provides support for array (de)serialization
operations.

*Arrays* are collections of |MessagePack| objects serialized according to the
|MessagePack array format|.

Available operations are:

.. hlist::

   * array utilities:

      * :c:macro:`DPACK_ARRAY_DATA_SIZE_MAX`
      * :c:macro:`DPACK_ARRAY_ELMNR_MAX`
      * :c:macro:`DPACK_ARRAY_ELMSZ_MAX`
      * :c:macro:`DPACK_ARRAY_FIXED_SIZE()`
      * :c:macro:`DPACK_ARRAY_HEAD_SIZE()`
      * :c:macro:`DPACK_ARRAY_MIXED_SIZE()`
      * :c:macro:`DPACK_ARRAY_SIZE_MAX`
      * :c:func:`dpack_array_fixed_size()`
      * :c:func:`dpack_array_mixed_size()`

   * array encoding:

      * :c:func:`dpack_array_begin_encode`
      * :c:func:`dpack_array_end_encode`

   * array decoding:

      * :c:type:`dpack_decode_item_fn`
      * :c:func:`dpack_array_decode`
      * :c:func:`dpack_array_decode_equ`
      * :c:func:`dpack_array_decode_min`
      * :c:func:`dpack_array_decode_max`
      * :c:func:`dpack_array_decode_range`

   * boolean array:

      * :c:macro:`DPACK_ARRAY_BOOL_SIZE()`

   * signed integer array:

      * :c:macro:`DPACK_ARRAY_INT8_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_INT8_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_INT16_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_INT16_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_INT32_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_INT32_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_INT64_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_INT64_SIZE_MIN()`

   * unsigned integer array:

      * :c:macro:`DPACK_ARRAY_UINT8_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_UINT8_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_UINT16_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_UINT16_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_UINT32_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_UINT32_SIZE_MIN()`
      * :c:macro:`DPACK_ARRAY_UINT64_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_UINT64_SIZE_MIN()`

   * floating point number array:

      * :c:macro:`DPACK_ARRAY_DOUBLE_SIZE()`
      * :c:macro:`DPACK_ARRAY_FLOAT_SIZE()`

   * string array:

      * :c:macro:`DPACK_ARRAY_STR_SIZE()`
      * :c:macro:`DPACK_ARRAY_STR_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_STR_SIZE_MIN()`

   * bin array:

      * :c:macro:`DPACK_ARRAY_BIN_SIZE()`
      * :c:macro:`DPACK_ARRAY_BIN_SIZE_MAX()`
      * :c:macro:`DPACK_ARRAY_BIN_SIZE_MIN()`

.. index:: map, structured aggregate, structured collection

.. _map:
.. _sect-api-map:

Map
===

When compiled with the :c:macro:`CONFIG_DPACK_MAP` build configuration option
enabled, the DPack_ library provides support for map (de)serialization
operations.

DPack_ *maps* are collections of |MessagePack| objects indexed by numerical
field identifiers and serialized according to the |MessagePack map format|.

Available operations are:

.. hlist::

   * map utilities:

      * :c:macro:`DPACK_MAP_DATA_SIZE_MAX`
      * :c:macro:`DPACK_MAP_HEAD_SIZE()`
      * :c:macro:`DPACK_MAP_FLDID_SIZE_MIN`
      * :c:macro:`DPACK_MAP_FLDID_SIZE_MAX`
      * :c:macro:`DPACK_MAP_FLDNR_MAX`
      * :c:macro:`DPACK_MAP_FLDSZ_MAX`
      * :c:macro:`DPACK_MAP_SIZE()`
      * :c:macro:`DPACK_MAP_SIZE_MAX`
      * :c:func:`dpack_map_size()`

   * map encoding:

      * :c:func:`dpack_map_begin_encode`
      * :c:func:`dpack_map_end_encode`

   * map field identifiers:

      * :c:func:`dpack_map_encode_fldid`
      * :c:func:`dpack_map_decode_fldid`

   * boolean map fields:

      * :c:macro:`DPACK_MAP_BOOL_SIZE_MAX`
      * :c:macro:`DPACK_MAP_BOOL_SIZE_MIN`
      * :c:func:`dpack_map_encode_bool`

   * signed integer map fields:

      * :c:macro:`DPACK_MAP_INT8_SIZE_MAX`
      * :c:macro:`DPACK_MAP_INT8_SIZE_MIN`
      * :c:macro:`DPACK_MAP_INT16_SIZE_MAX`
      * :c:macro:`DPACK_MAP_INT16_SIZE_MIN`
      * :c:macro:`DPACK_MAP_INT32_SIZE_MAX`
      * :c:macro:`DPACK_MAP_INT32_SIZE_MIN`
      * :c:macro:`DPACK_MAP_INT64_SIZE_MAX`
      * :c:macro:`DPACK_MAP_INT64_SIZE_MIN`
      * :c:func:`dpack_map_encode_int8`
      * :c:func:`dpack_map_encode_int16`
      * :c:func:`dpack_map_encode_int32`
      * :c:func:`dpack_map_encode_int64`

   * unsigned integer map fields:

      * :c:macro:`DPACK_MAP_UINT8_SIZE_MAX`
      * :c:macro:`DPACK_MAP_UINT8_SIZE_MIN`
      * :c:macro:`DPACK_MAP_UINT16_SIZE_MAX`
      * :c:macro:`DPACK_MAP_UINT16_SIZE_MIN`
      * :c:macro:`DPACK_MAP_UINT32_SIZE_MAX`
      * :c:macro:`DPACK_MAP_UINT32_SIZE_MIN`
      * :c:macro:`DPACK_MAP_UINT64_SIZE_MAX`
      * :c:macro:`DPACK_MAP_UINT64_SIZE_MIN`
      * :c:func:`dpack_map_encode_uint8`
      * :c:func:`dpack_map_encode_uint16`
      * :c:func:`dpack_map_encode_uint32`
      * :c:func:`dpack_map_encode_uint64`

   * floating point number map fields:

      * :c:macro:`DPACK_MAP_DOUBLE_SIZE_MAX`
      * :c:macro:`DPACK_MAP_DOUBLE_SIZE_MIN`
      * :c:macro:`DPACK_MAP_FLOAT_SIZE_MAX`
      * :c:macro:`DPACK_MAP_FLOAT_SIZE_MIN`
      * :c:func:`dpack_map_encode_double`
      * :c:func:`dpack_map_encode_float`

   * string and lvstr map fields:

     * :c:macro:`DPACK_MAP_LVSTR_SIZE()`
     * :c:macro:`DPACK_MAP_LVSTR_SIZE_MAX`
     * :c:macro:`DPACK_MAP_LVSTR_SIZE_MIN`
     * :c:macro:`DPACK_MAP_STR_SIZE()`
     * :c:macro:`DPACK_MAP_STR_SIZE_MAX`
     * :c:macro:`DPACK_MAP_STR_SIZE_MIN`
     * :c:func:`dpack_map_encode_lvstr`
     * :c:func:`dpack_map_encode_str`
     * :c:func:`dpack_map_encode_str_fix`

   * bin map fields:

     * :c:macro:`DPACK_MAP_BIN_SIZE()`
     * :c:macro:`DPACK_MAP_BIN_SIZE_MAX`
     * :c:macro:`DPACK_MAP_BIN_SIZE_MIN`
     * :c:func:`dpack_map_encode_bin`

   * nil map fields:

     * :c:macro:`DPACK_MAP_NIL_SIZE_MAX`
     * :c:macro:`DPACK_MAP_NIL_SIZE_MIN`
     * :c:func:`dpack_map_encode_nil`

   * nested collection map fields:

     * :c:macro:`DPACK_MAP_NEST_SIZE_MAX()`
     * :c:macro:`DPACK_MAP_NEST_SIZE_MIN()`
     * :c:func:`dpack_map_begin_encode_nest_array`
     * :c:func:`dpack_map_begin_encode_nest_map`

.. index:: API reference, reference

Reference
=========

Configuration macros
--------------------

CONFIG_DPACK_ARRAY
******************

.. doxygendefine:: CONFIG_DPACK_ARRAY

CONFIG_DPACK_ASSERT_API
***********************

.. doxygendefine:: CONFIG_DPACK_ASSERT_API

CONFIG_DPACK_ASSERT_INTERN
**************************

.. doxygendefine:: CONFIG_DPACK_ASSERT_INTERN

CONFIG_DPACK_BIN
****************

.. doxygendefine:: CONFIG_DPACK_BIN

CONFIG_DPACK_DEBUG
******************

.. doxygendefine:: CONFIG_DPACK_DEBUG

.. _CONFIG_DPACK_DOUBLE:

CONFIG_DPACK_DOUBLE
*******************

.. doxygendefine:: CONFIG_DPACK_DOUBLE

.. _CONFIG_DPACK_FLOAT:

CONFIG_DPACK_FLOAT
******************

.. doxygendefine:: CONFIG_DPACK_FLOAT

CONFIG_DPACK_LVSTR
******************

.. doxygendefine:: CONFIG_DPACK_LVSTR

CONFIG_DPACK_MAP
****************

.. doxygendefine:: CONFIG_DPACK_MAP

CONFIG_DPACK_SAMPLE
*******************

.. doxygendefine:: CONFIG_DPACK_SAMPLE

CONFIG_DPACK_SCALAR
*******************

.. doxygendefine:: CONFIG_DPACK_SCALAR

CONFIG_DPACK_STRING
*******************

.. doxygendefine:: CONFIG_DPACK_STRING

.. _CONFIG_DPACK_UTEST:

CONFIG_DPACK_UTEST
******************

.. doxygendefine:: CONFIG_DPACK_UTEST

CONFIG_DPACK_VALGRIND
*********************

.. doxygendefine:: CONFIG_DPACK_VALGRIND

Macros
------

DPACK_ABORT
***********

.. doxygendefine:: DPACK_ABORT

DPACK_ARRAY_BIN_SIZE
********************

.. doxygendefine:: DPACK_ARRAY_BIN_SIZE

DPACK_ARRAY_BIN_SIZE_MAX
************************

.. doxygendefine:: DPACK_ARRAY_BIN_SIZE_MAX

DPACK_ARRAY_BIN_SIZE_MIN
************************

.. doxygendefine:: DPACK_ARRAY_BIN_SIZE_MIN

DPACK_ARRAY_BOOL_SIZE
*********************

.. doxygendefine:: DPACK_ARRAY_BOOL_SIZE

DPACK_ARRAY_DATA_SIZE_MAX
*************************

.. doxygendefine:: DPACK_ARRAY_DATA_SIZE_MAX

DPACK_ARRAY_DOUBLE_SIZE
***********************

.. doxygendefine:: DPACK_ARRAY_DOUBLE_SIZE

DPACK_ARRAY_ELMNR_MAX
*********************

.. doxygendefine:: DPACK_ARRAY_ELMNR_MAX

DPACK_ARRAY_ELMSZ_MAX
*********************

.. doxygendefine:: DPACK_ARRAY_ELMSZ_MAX

DPACK_ARRAY_FIXED_SIZE
**********************

.. doxygendefine:: DPACK_ARRAY_FIXED_SIZE

DPACK_ARRAY_FLOAT_SIZE
**********************

.. doxygendefine:: DPACK_ARRAY_FLOAT_SIZE

DPACK_ARRAY_HEAD_SIZE
*********************

.. doxygendefine:: DPACK_ARRAY_HEAD_SIZE

DPACK_ARRAY_INT8_SIZE_MAX
*************************

.. doxygendefine:: DPACK_ARRAY_INT8_SIZE_MAX

DPACK_ARRAY_INT8_SIZE_MIN
*************************

.. doxygendefine:: DPACK_ARRAY_INT8_SIZE_MIN

DPACK_ARRAY_INT16_SIZE_MAX
**************************

.. doxygendefine:: DPACK_ARRAY_INT16_SIZE_MAX

DPACK_ARRAY_INT16_SIZE_MIN
**************************

.. doxygendefine:: DPACK_ARRAY_INT16_SIZE_MIN

DPACK_ARRAY_INT32_SIZE_MAX
**************************

.. doxygendefine:: DPACK_ARRAY_INT32_SIZE_MAX

DPACK_ARRAY_INT32_SIZE_MIN
**************************

.. doxygendefine:: DPACK_ARRAY_INT32_SIZE_MIN

DPACK_ARRAY_INT64_SIZE_MAX
**************************

.. doxygendefine:: DPACK_ARRAY_INT64_SIZE_MAX

DPACK_ARRAY_INT64_SIZE_MIN
**************************

.. doxygendefine:: DPACK_ARRAY_INT64_SIZE_MIN

DPACK_ARRAY_MIXED_SIZE
**********************

.. doxygendefine:: DPACK_ARRAY_MIXED_SIZE

DPACK_ARRAY_SIZE_MAX
********************

.. doxygendefine:: DPACK_ARRAY_SIZE_MAX

DPACK_ARRAY_STR_SIZE
********************

.. doxygendefine:: DPACK_ARRAY_STR_SIZE

DPACK_ARRAY_STR_SIZE_MAX
************************

.. doxygendefine:: DPACK_ARRAY_STR_SIZE_MAX

DPACK_ARRAY_STR_SIZE_MIN
************************

.. doxygendefine:: DPACK_ARRAY_STR_SIZE_MIN

DPACK_ARRAY_UINT8_SIZE_MAX
**************************

.. doxygendefine:: DPACK_ARRAY_UINT8_SIZE_MAX

DPACK_ARRAY_UINT8_SIZE_MIN
**************************

.. doxygendefine:: DPACK_ARRAY_UINT8_SIZE_MIN

DPACK_ARRAY_UINT16_SIZE_MAX
***************************

.. doxygendefine:: DPACK_ARRAY_UINT16_SIZE_MAX

DPACK_ARRAY_UINT16_SIZE_MIN
***************************

.. doxygendefine:: DPACK_ARRAY_UINT16_SIZE_MIN

DPACK_ARRAY_UINT32_SIZE_MAX
***************************

.. doxygendefine:: DPACK_ARRAY_UINT32_SIZE_MAX

DPACK_ARRAY_UINT32_SIZE_MIN
***************************

.. doxygendefine:: DPACK_ARRAY_UINT32_SIZE_MIN

DPACK_ARRAY_UINT64_SIZE_MAX
***************************

.. doxygendefine:: DPACK_ARRAY_UINT64_SIZE_MAX

DPACK_ARRAY_UINT64_SIZE_MIN
***************************

.. doxygendefine:: DPACK_ARRAY_UINT64_SIZE_MIN

DPACK_BIN_SIZE
**************

.. doxygendefine:: DPACK_BIN_SIZE

DPACK_BINSZ_MAX
***************

.. doxygendefine:: DPACK_BINSZ_MAX

DPACK_BOOL_SIZE
***************

.. doxygendefine:: DPACK_BOOL_SIZE

DPACK_DONE
**********

.. doxygendefine:: DPACK_DONE

DPACK_DOUBLE_SIZE
*****************

.. doxygendefine:: DPACK_DOUBLE_SIZE

DPACK_FLOAT_SIZE
****************

.. doxygendefine:: DPACK_FLOAT_SIZE

DPACK_INT16_SIZE_MAX
********************

.. doxygendefine:: DPACK_INT16_SIZE_MAX

DPACK_INT16_SIZE_MIN
********************

.. doxygendefine:: DPACK_INT16_SIZE_MIN

DPACK_INT32_SIZE_MAX
********************

.. doxygendefine:: DPACK_INT32_SIZE_MAX

DPACK_INT32_SIZE_MIN
********************

.. doxygendefine:: DPACK_INT32_SIZE_MIN

DPACK_INT64_SIZE_MAX
********************

.. doxygendefine:: DPACK_INT64_SIZE_MAX

DPACK_INT64_SIZE_MIN
********************

.. doxygendefine:: DPACK_INT64_SIZE_MIN

DPACK_INT8_SIZE_MAX
*******************

.. doxygendefine:: DPACK_INT8_SIZE_MAX

DPACK_INT8_SIZE_MIN
*******************

.. doxygendefine:: DPACK_INT8_SIZE_MIN

DPACK_INT_SIZE_MAX
******************

.. doxygendefine:: DPACK_INT_SIZE_MAX

DPACK_INT_SIZE_MIN
******************

.. doxygendefine:: DPACK_INT_SIZE_MIN

DPACK_LVSTR_SIZE
****************

.. doxygendefine:: DPACK_LVSTR_SIZE

DPACK_LVSTRLEN_MAX
******************

.. doxygendefine:: DPACK_LVSTRLEN_MAX

DPACK_MAP_BIN_SIZE
******************

.. doxygendefine:: DPACK_MAP_BIN_SIZE

DPACK_MAP_BIN_SIZE_MAX
**********************

.. doxygendefine:: DPACK_MAP_BIN_SIZE_MAX

DPACK_MAP_BIN_SIZE_MIN
**********************

.. doxygendefine:: DPACK_MAP_BIN_SIZE_MIN

DPACK_MAP_BOOL_SIZE_MAX
***********************

.. doxygendefine:: DPACK_MAP_BOOL_SIZE_MAX

DPACK_MAP_BOOL_SIZE_MIN
***********************

.. doxygendefine:: DPACK_MAP_BOOL_SIZE_MIN

DPACK_MAP_DATA_SIZE_MAX
***********************

.. doxygendefine:: DPACK_MAP_DATA_SIZE_MAX

DPACK_MAP_DOUBLE_SIZE_MAX
*************************

.. doxygendefine:: DPACK_MAP_DOUBLE_SIZE_MAX

DPACK_MAP_DOUBLE_SIZE_MIN
*************************

.. doxygendefine:: DPACK_MAP_DOUBLE_SIZE_MIN

DPACK_MAP_FLDID_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_FLDID_SIZE_MIN

DPACK_MAP_FLDID_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_FLDID_SIZE_MAX

DPACK_MAP_FLDNR_MAX
*******************

.. doxygendefine:: DPACK_MAP_FLDNR_MAX

DPACK_MAP_FLDSZ_MAX
*******************

.. doxygendefine:: DPACK_MAP_FLDSZ_MAX

DPACK_MAP_FLOAT_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_FLOAT_SIZE_MAX

DPACK_MAP_FLOAT_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_FLOAT_SIZE_MIN

DPACK_MAP_HEAD_SIZE
*******************

.. doxygendefine:: DPACK_MAP_HEAD_SIZE

DPACK_MAP_INT8_SIZE_MAX
***********************

.. doxygendefine:: DPACK_MAP_INT8_SIZE_MAX

DPACK_MAP_INT16_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_INT16_SIZE_MAX

DPACK_MAP_INT8_SIZE_MIN
***********************

.. doxygendefine:: DPACK_MAP_INT8_SIZE_MIN

DPACK_MAP_INT16_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_INT16_SIZE_MIN

DPACK_MAP_INT32_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_INT32_SIZE_MAX

DPACK_MAP_INT32_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_INT32_SIZE_MIN

DPACK_MAP_INT64_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_INT64_SIZE_MAX

DPACK_MAP_INT64_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_INT64_SIZE_MIN

DPACK_MAP_LVSTR_SIZE()
**********************

.. doxygendefine:: DPACK_MAP_LVSTR_SIZE

DPACK_MAP_LVSTR_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_LVSTR_SIZE_MAX

DPACK_MAP_LVSTR_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_LVSTR_SIZE_MIN

DPACK_MAP_NEST_SIZE_MAX
***********************

.. doxygendefine:: DPACK_MAP_NEST_SIZE_MAX

DPACK_MAP_NEST_SIZE_MIN
***********************

.. doxygendefine:: DPACK_MAP_NEST_SIZE_MIN

DPACK_MAP_NIL_SIZE_MAX
**********************

.. doxygendefine:: DPACK_MAP_NIL_SIZE_MAX

DPACK_MAP_NIL_SIZE_MIN
**********************

.. doxygendefine:: DPACK_MAP_NIL_SIZE_MIN

DPACK_MAP_SIZE
**************

.. doxygendefine:: DPACK_MAP_SIZE

DPACK_MAP_SIZE_MAX
******************

.. doxygendefine:: DPACK_MAP_SIZE_MAX

DPACK_MAP_STR_SIZE
******************

.. doxygendefine:: DPACK_MAP_STR_SIZE

DPACK_MAP_STR_SIZE_MIN
**********************

.. doxygendefine:: DPACK_MAP_STR_SIZE_MIN

DPACK_MAP_STR_SIZE_MAX
**********************

.. doxygendefine:: DPACK_MAP_STR_SIZE_MAX

DPACK_MAP_UINT8_SIZE_MAX
************************

.. doxygendefine:: DPACK_MAP_UINT8_SIZE_MAX

DPACK_MAP_UINT16_SIZE_MAX
*************************

.. doxygendefine:: DPACK_MAP_UINT16_SIZE_MAX

DPACK_MAP_UINT8_SIZE_MIN
************************

.. doxygendefine:: DPACK_MAP_UINT8_SIZE_MIN

DPACK_MAP_UINT16_SIZE_MIN
*************************

.. doxygendefine:: DPACK_MAP_UINT16_SIZE_MIN

DPACK_MAP_UINT32_SIZE_MAX
*************************

.. doxygendefine:: DPACK_MAP_UINT32_SIZE_MAX

DPACK_MAP_UINT32_SIZE_MIN
*************************

.. doxygendefine:: DPACK_MAP_UINT32_SIZE_MIN

DPACK_MAP_UINT64_SIZE_MAX
*************************

.. doxygendefine:: DPACK_MAP_UINT64_SIZE_MAX

DPACK_MAP_UINT64_SIZE_MIN
*************************

.. doxygendefine:: DPACK_MAP_UINT64_SIZE_MIN

DPACK_NIL_SIZE
**************

.. doxygendefine:: DPACK_NIL_SIZE

DPACK_STDINT_SIZE_MAX
*********************

.. doxygendefine:: DPACK_STDINT_SIZE_MAX

DPACK_STDINT_SIZE_MIN
*********************

.. doxygendefine:: DPACK_STDINT_SIZE_MIN

DPACK_STR_SIZE
**************

.. doxygendefine:: DPACK_STR_SIZE

DPACK_STRLEN_MAX
****************

.. doxygendefine:: DPACK_STRLEN_MAX

DPACK_UINT16_SIZE_MAX
*********************

.. doxygendefine:: DPACK_UINT16_SIZE_MAX

DPACK_UINT16_SIZE_MIN
*********************

.. doxygendefine:: DPACK_UINT16_SIZE_MIN

DPACK_UINT32_SIZE_MAX
*********************

.. doxygendefine:: DPACK_UINT32_SIZE_MAX

DPACK_UINT32_SIZE_MIN
*********************

.. doxygendefine:: DPACK_UINT32_SIZE_MIN

DPACK_UINT64_SIZE_MAX
*********************

.. doxygendefine:: DPACK_UINT64_SIZE_MAX

DPACK_UINT64_SIZE_MIN
*********************

.. doxygendefine:: DPACK_UINT64_SIZE_MIN

DPACK_UINT8_SIZE_MAX
********************

.. doxygendefine:: DPACK_UINT8_SIZE_MAX

DPACK_UINT8_SIZE_MIN
********************

.. doxygendefine:: DPACK_UINT8_SIZE_MIN

DPACK_UINT_SIZE_MAX
*******************

.. doxygendefine:: DPACK_UINT_SIZE_MAX

DPACK_UINT_SIZE_MIN
*******************

.. doxygendefine:: DPACK_UINT_SIZE_MIN

Structures
----------

dpack_decoder
*************

.. doxygenstruct:: dpack_decoder

dpack_encoder
*************

.. doxygenstruct:: dpack_encoder

Typedefs
--------

dpack_decode_item_fn
********************

.. doxygentypedef:: dpack_decode_item_fn

Functions
---------

dpack_array_fixed_size
**********************

.. doxygenfunction:: dpack_array_fixed_size
   
dpack_array_mixed_size
**********************

.. doxygenfunction:: dpack_array_mixed_size

dpack_array_begin_encode
************************

.. doxygenfunction:: dpack_array_begin_encode

dpack_array_decode
******************

.. doxygenfunction:: dpack_array_decode

dpack_array_decode_equ
**********************

.. doxygenfunction:: dpack_array_decode_equ

dpack_array_decode_min
**********************

.. doxygenfunction:: dpack_array_decode_min

dpack_array_decode_max
**********************

.. doxygenfunction:: dpack_array_decode_max

dpack_array_decode_range
************************

.. doxygenfunction:: dpack_array_decode_range

dpack_array_end_encode
**********************

.. doxygenfunction:: dpack_array_end_encode

dpack_bin_size
**************

.. doxygenfunction:: dpack_bin_size

dpack_decode_bincpy
*******************

.. doxygenfunction:: dpack_decode_bincpy

dpack_decode_bincpy_equ
***********************

.. doxygenfunction:: dpack_decode_bincpy_equ

dpack_decode_bincpy_range
*************************

.. doxygenfunction:: dpack_decode_bincpy_range

dpack_decode_bindup
*******************

.. doxygenfunction:: dpack_decode_bindup

dpack_decode_bindup_equ
***********************

.. doxygenfunction:: dpack_decode_bindup_equ

dpack_decode_bindup_max
***********************

.. doxygenfunction:: dpack_decode_bindup_max

dpack_decode_bindup_range
*************************

.. doxygenfunction:: dpack_decode_bindup_range

dpack_decode_bool
*****************

.. doxygenfunction:: dpack_decode_bool

dpack_decode_double
*******************

.. doxygenfunction:: dpack_decode_double

dpack_decode_double_min
***********************

.. doxygenfunction:: dpack_decode_double_min

dpack_decode_double_max
***********************

.. doxygenfunction:: dpack_decode_double_max

dpack_decode_double_range
*************************

.. doxygenfunction:: dpack_decode_double_range

dpack_decode_float
******************

.. doxygenfunction:: dpack_decode_float

dpack_decode_float_min
**********************

.. doxygenfunction:: dpack_decode_float_min

dpack_decode_float_max
**********************

.. doxygenfunction:: dpack_decode_float_max

dpack_decode_float_range
************************

.. doxygenfunction:: dpack_decode_float_range

dpack_decode_int
****************

.. doxygenfunction:: dpack_decode_int

dpack_decode_int16
******************

.. doxygenfunction:: dpack_decode_int16

dpack_decode_int16_max
**********************

.. doxygenfunction:: dpack_decode_int16_max

dpack_decode_int16_min
**********************

.. doxygenfunction:: dpack_decode_int16_min

dpack_decode_int16_range
************************

.. doxygenfunction:: dpack_decode_int16_range

dpack_decode_int32
******************

.. doxygenfunction:: dpack_decode_int32

dpack_decode_int32_max
**********************

.. doxygenfunction:: dpack_decode_int32_max

dpack_decode_int32_min
**********************

.. doxygenfunction:: dpack_decode_int32_min

dpack_decode_int32_range
************************

.. doxygenfunction:: dpack_decode_int32_range

dpack_decode_int64
******************

.. doxygenfunction:: dpack_decode_int64

dpack_decode_int64_max
**********************

.. doxygenfunction:: dpack_decode_int64_max

dpack_decode_int64_min
**********************

.. doxygenfunction:: dpack_decode_int64_min

dpack_decode_int64_range
************************

.. doxygenfunction:: dpack_decode_int64_range

dpack_decode_int8
*****************

.. doxygenfunction:: dpack_decode_int8

dpack_decode_int8_max
*********************

.. doxygenfunction:: dpack_decode_int8_max

dpack_decode_int8_min
*********************

.. doxygenfunction:: dpack_decode_int8_min

dpack_decode_int8_range
***********************

.. doxygenfunction:: dpack_decode_int8_range

dpack_decode_int_max
********************

.. doxygenfunction:: dpack_decode_int_max

dpack_decode_int_min
********************

.. doxygenfunction:: dpack_decode_int_min

dpack_decode_int_range
**********************

.. doxygenfunction:: dpack_decode_int_range

dpack_decode_lvstr
******************

.. doxygenfunction:: dpack_decode_lvstr

dpack_decode_lvstr_equ
**********************

.. doxygenfunction:: dpack_decode_lvstr_equ

dpack_decode_lvstr_max
**********************

.. doxygenfunction:: dpack_decode_lvstr_max

dpack_decode_lvstr_range
************************

.. doxygenfunction:: dpack_decode_lvstr_range

dpack_decode_nil
****************

.. doxygenfunction:: dpack_decode_nil

dpack_decode_strdup
*******************

.. doxygenfunction:: dpack_decode_strdup

dpack_decode_strdup_equ
***********************

.. doxygenfunction:: dpack_decode_strdup_equ

dpack_decode_strdup_max
***********************

.. doxygenfunction:: dpack_decode_strdup_max

dpack_decode_strdup_range
*************************

.. doxygenfunction:: dpack_decode_strdup_range

dpack_decode_strcpy
*******************

.. doxygenfunction:: dpack_decode_strcpy

dpack_decode_strcpy_equ
***********************

.. doxygenfunction:: dpack_decode_strcpy_equ

dpack_decode_strcpy_range
*************************

.. doxygenfunction:: dpack_decode_strcpy_range

dpack_decode_uint
*****************

.. doxygenfunction:: dpack_decode_uint

dpack_decode_uint16
*******************

.. doxygenfunction:: dpack_decode_uint16

dpack_decode_uint16_max
***********************

.. doxygenfunction:: dpack_decode_uint16_max

dpack_decode_uint16_min
***********************

.. doxygenfunction:: dpack_decode_uint16_min

dpack_decode_uint16_range
*************************

.. doxygenfunction:: dpack_decode_uint16_range

dpack_decode_uint32
*******************

.. doxygenfunction:: dpack_decode_uint32

dpack_decode_uint32_max
***********************

.. doxygenfunction:: dpack_decode_uint32_max

dpack_decode_uint32_min
***********************

.. doxygenfunction:: dpack_decode_uint32_min

dpack_decode_uint32_range
*************************

.. doxygenfunction:: dpack_decode_uint32_range

dpack_decode_uint64
*******************

.. doxygenfunction:: dpack_decode_uint64

dpack_decode_uint64_max
***********************

.. doxygenfunction:: dpack_decode_uint64_max

dpack_decode_uint64_min
***********************

.. doxygenfunction:: dpack_decode_uint64_min

dpack_decode_uint64_range
*************************

.. doxygenfunction:: dpack_decode_uint64_range

dpack_decode_uint8
******************

.. doxygenfunction:: dpack_decode_uint8

dpack_decode_uint8_max
**********************

.. doxygenfunction:: dpack_decode_uint8_max

dpack_decode_uint8_min
**********************

.. doxygenfunction:: dpack_decode_uint8_min

dpack_decode_uint8_range
************************

.. doxygenfunction:: dpack_decode_uint8_range

dpack_decode_uint_max
*********************

.. doxygenfunction:: dpack_decode_uint_max

dpack_decode_uint_min
*********************

.. doxygenfunction:: dpack_decode_uint_min

dpack_decode_uint_range
***********************

.. doxygenfunction:: dpack_decode_uint_range

dpack_decoder_data_left
***********************

.. doxygenfunction:: dpack_decoder_data_left

dpack_decoder_fini
******************

.. doxygenfunction:: dpack_decoder_fini

dpack_decoder_init_buffer
*************************

.. doxygenfunction:: dpack_decoder_init_buffer

dpack_decoder_init_skip_buffer
******************************

.. doxygenfunction:: dpack_decoder_init_skip_buffer

dpack_decoder_skip
******************

.. doxygenfunction:: dpack_decoder_skip

dpack_encode_bin
****************

.. doxygenfunction:: dpack_encode_bin

dpack_encode_bool
*****************

.. doxygenfunction:: dpack_encode_bool

dpack_encode_double
*******************

.. doxygenfunction:: dpack_encode_double

dpack_encode_float
******************

.. doxygenfunction:: dpack_encode_float

dpack_encode_int
****************

.. doxygenfunction:: dpack_encode_int

dpack_encode_int16
******************

.. doxygenfunction:: dpack_encode_int16

dpack_encode_int32
******************

.. doxygenfunction:: dpack_encode_int32

dpack_encode_int64
******************

.. doxygenfunction:: dpack_encode_int64

dpack_encode_int8
*****************

.. doxygenfunction:: dpack_encode_int8

dpack_encode_lvstr
******************

.. doxygenfunction:: dpack_encode_lvstr

dpack_encode_nil
****************

.. doxygenfunction:: dpack_encode_nil

dpack_encode_str
****************

.. doxygenfunction:: dpack_encode_str

dpack_encode_str_fix
********************

.. doxygenfunction:: dpack_encode_str_fix

dpack_encode_uint
*****************

.. doxygenfunction:: dpack_encode_uint

dpack_encode_uint16
*******************

.. doxygenfunction:: dpack_encode_uint16

dpack_encode_uint32
*******************

.. doxygenfunction:: dpack_encode_uint32

dpack_encode_uint64
*******************

.. doxygenfunction:: dpack_encode_uint64

dpack_encode_uint8
******************

.. doxygenfunction:: dpack_encode_uint8

dpack_encoder_fini
******************

.. doxygenfunction:: dpack_encoder_fini

dpack_encoder_init_buffer
*************************

.. doxygenfunction:: dpack_encoder_init_buffer

dpack_encoder_space_left
************************

.. doxygenfunction:: dpack_encoder_space_left

dpack_encoder_space_used
************************

.. doxygenfunction:: dpack_encoder_space_used

dpack_lvstr_size
****************

.. doxygenfunction:: dpack_lvstr_size

dpack_map_begin_encode
**********************

.. doxygenfunction:: dpack_map_begin_encode

dpack_map_begin_encode_nest_array
*********************************

.. doxygenfunction:: dpack_map_begin_encode_nest_array

dpack_map_begin_encode_nest_map
*******************************

.. doxygenfunction:: dpack_map_begin_encode_nest_map

dpack_map_decode_fldid
**********************

.. doxygenfunction:: dpack_map_decode_fldid

dpack_map_encode_bin
********************

.. doxygenfunction:: dpack_map_encode_bin

dpack_map_encode_bool
*********************

.. doxygenfunction:: dpack_map_encode_bool

dpack_map_encode_double
***********************

.. doxygenfunction:: dpack_map_encode_double

dpack_map_encode_fldid
**********************

.. doxygenfunction:: dpack_map_encode_fldid

dpack_map_encode_float
**********************

.. doxygenfunction:: dpack_map_encode_float

dpack_map_encode_int8
*********************

.. doxygenfunction:: dpack_map_encode_int8

dpack_map_encode_int16
**********************

.. doxygenfunction:: dpack_map_encode_int16

dpack_map_encode_int32
**********************

.. doxygenfunction:: dpack_map_encode_int32

dpack_map_encode_int64
**********************

.. doxygenfunction:: dpack_map_encode_int64

dpack_map_encode_lvstr
**********************

.. doxygenfunction:: dpack_map_encode_lvstr

dpack_map_encode_nil
********************

.. doxygenfunction:: dpack_map_encode_nil

dpack_map_encode_str
********************

.. doxygenfunction:: dpack_map_encode_str

dpack_map_encode_str_fix
************************

.. doxygenfunction:: dpack_map_encode_str_fix

dpack_map_encode_uint8
**********************

.. doxygenfunction:: dpack_map_encode_uint8

dpack_map_encode_uint16
***********************

.. doxygenfunction:: dpack_map_encode_uint16

dpack_map_encode_uint32
***********************

.. doxygenfunction:: dpack_map_encode_uint32

dpack_map_encode_uint64
***********************

.. doxygenfunction:: dpack_map_encode_uint64

dpack_map_end_encode
********************

.. doxygenfunction:: dpack_map_end_encode

dpack_map_size
**************

.. doxygenfunction:: dpack_map_size

dpack_str_size
**************

.. doxygenfunction:: dpack_str_size
