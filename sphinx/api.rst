.. include:: _cdefs.rst

.. |MessagePack format|       replace:: :doc:`msgpack`
.. |MessagePack bool format|  replace:: :ref:`MessagePack boolean format <sect-msgpack-bool>`
.. |MessagePack int format|   replace:: :ref:`MessagePack integer format <sect-msgpack-int>`
.. |MessagePack float format| replace:: :ref:`MessagePack float format <sect-msgpack-float>`

.. _mpack:                    https://github.com/ludocode/mpack
.. |MPack|                    replace:: `MPack <mpack_>`_

.. |Stroll|                   replace:: :external+stroll:doc:`Stroll <index>`

.. |lvstr|                    replace:: :external+stroll:ref:`lvstr <sect-api-lvstr>`

.. _glibc:                    https://www.gnu.org/s/libc/
.. |GLibc|                    replace:: `GNU C library <glibc_>`_

Overview
========

What follows here provides a thorough description of how to use DPack library.

Basically, DPack library is a C library that wraps calls to |MPack| to
(de)serialize objects according to |MessagePack format|. As stated onto the home
page:

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

DPack library API is organized around the following functional areas which
you can refer to for further details :

* Encoder_,
* Decoder_,
* Boolean_,
* Integer_,
* `Floating point number`_
* String_,
* `Length-Value string`_,
* List_,
* Map_.

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
  
.. index:: encode, serialize, pack

Encoder
=======

The DPack library serialization interface is provided through to the
:c:struct:`dpack_encoder` interface. The following operations are available:

* :c:func:`dpack_encoder_init_buffer`
* :c:func:`dpack_encoder_fini`
* :c:func:`dpack_encoder_space_used`
* :c:func:`dpack_encoder_space_left`
  
You *MUST* include :file:`dpack/codec.h` header to use this interface.

.. index:: decode, unserialize, unpack
   
Decoder
=======

The DPack library unserialization interface is provided through to the
:c:struct:`dpack_decoder` interface. The following operations are available:

* :c:func:`dpack_decoder_init_buffer`
* :c:func:`dpack_decoder_init_skip_buffer`
* :c:func:`dpack_decoder_fini`
* :c:func:`dpack_decoder_data_left`
* :c:func:`dpack_decoder_skip`

You *MUST* include :file:`dpack/codec.h` header to use this interface.

.. index:: boolean, bool

Boolean
=======

When compiled with the :c:macro:`CONFIG_DPACK_SCALAR` build configuration
option enabled, the DPack library provides support for boolean (de)serialization
operations. These are:

* :c:macro:`DPACK_BOOL_SIZE`
* :c:func:`dpack_encode_bool`
* :c:func:`dpack_decode_bool`

You *MUST* include :file:`dpack/scalar.h` header to use this interface.

.. index:: integers, signed, unsigned, 8-bits, 16-bits, 32-bits, 64-bits

Integer
========

When compiled with the :c:macro:`CONFIG_DPACK_SCALAR` build configuration
option enabled, the DPack library provides support for integer (de)serialization
operations. These are:

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

You *MUST* include :file:`dpack/scalar.h` header to use this interface.

.. index:: float, double, floating point number
   
Floating point number
=====================

When compiled with the :c:macro:`CONFIG_DPACK_FLOAT` build configuration
option enabled, the DPack library provides support for single precision floating
point numbers (de)serialization operations. These are:

* :c:macro:`DPACK_FLOAT_SIZE`
* :c:func:`dpack_encode_float`
* :c:func:`dpack_decode_float`
* :c:func:`dpack_decode_float_min`
* :c:func:`dpack_decode_float_max`
* :c:func:`dpack_decode_float_range`

When compiled with the :c:macro:`CONFIG_DPACK_DOUBLE` build configuration
option enabled, the DPack library provides support for double precision floating
point numbers (de)serialization operations. These are:

* :c:macro:`DPACK_DOUBLE_SIZE`
* :c:func:`dpack_encode_double`
* :c:func:`dpack_decode_double`
* :c:func:`dpack_decode_double_min`
* :c:func:`dpack_decode_double_max`
* :c:func:`dpack_decode_double_range`

You *MUST* include :file:`dpack/scalar.h` header to use these interfaces.

.. index:: string
   
String
======
   
.. todo::

   Document strings

.. index:: Length-Value string, lvstr

Length-Value string
===================

.. todo::

   Document lvstr

.. index:: list, collection, array
   
.. _sect-api-list:
   
List
====
   
.. todo::

   Document lists

.. index:: map, structured aggregate, structured collection
   
.. _sect-api-map:

Map
===

.. todo::

   Document maps

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
  
DPACK_BOOL_SIZE
***************

.. doxygendefine:: DPACK_BOOL_SIZE

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

DPACK_STDINT_SIZE_MIN
*********************

.. doxygendefine:: DPACK_STDINT_SIZE_MIN

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

Functions
---------

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
