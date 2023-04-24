.. _breathe:              https://github.com/breathe-doc/breathe
.. _cmocka:               https://cmocka.org/
.. _ebuild:               https://github.com/grgbr/ebuild/
.. _stroll:               https://github.com/grgbr/stroll/
.. _gnu_make:             https://www.gnu.org/software/make/
.. |eBuild|               replace:: `eBuild <ebuild_>`_
.. |eBuild User Guide|    replace:: :external+ebuild:doc:`eBuild User Guide <user>`
.. |eBuild Prerequisites| replace:: :external+ebuild:ref:`eBuild Prerequisites <sect-user-prerequisites>`
.. |Stroll|               replace:: `Stroll <stroll_>`_
.. |Configure|            replace:: :external+ebuild:ref:`sect-user-configure`
.. |Build|                replace:: :external+ebuild:ref:`sect-user-build`
.. |Install|              replace:: :external+ebuild:ref:`sect-user-install`
.. |Staged install|       replace:: :external+ebuild:ref:`sect-user-staged-install`
.. |BUILDDIR|             replace:: :external+ebuild:ref:`var-builddir`
.. |PREFIX|               replace:: :external+ebuild:ref:`var-prefix`
.. |CROSS_COMPILE|        replace:: :external+ebuild:ref:`var-cross_compile`
.. |DESTDIR|              replace:: :external+ebuild:ref:`var-destdir`
.. |GNU Make|             replace:: `GNU Make <gnu_make_>`_

Overview
========

This guide mainly focuses upon the construction process required to install
DPack.

DPack's build logic is based upon |eBuild|. In addition to the build process
description explained below, you may refer to the |eBuild User Guide|
for further detailed informations.

Prerequisites
=============

In addition to the standard |eBuild Prerequisites|, DPack requires a working
|Stroll| install at build time and runtime.

Optionally, you will need cmocka_ at build time and at runtime when unit
testsuite is enabled (see :ref:`CONFIG_DPACK_UTEST`).

Optionally, you will need multiple packages installed to build the
documentation. In addition to packages listed into |eBuild Prerequisites|,
DPack's documentation generation process requires breathe_.

Getting help
============

From DPack source tree root, enter:

.. code-block:: console

   $ make help

Also note that a more detailed help message is available:

.. code-block:: console

   $ make help-full

Refer to :external+ebuild:ref:`eBuild help target <target-help>` and
:external+ebuild:ref:`eBuild help-full target <target-help-full>` for further
informations.

The :external+ebuild:ref:`eBuild Troubleshooting <sect-user-troubleshooting>`
section also contains valuable informations.

Build Workflow
==============

As mentioned earlier, DPack's build logic is based on |eBuild|, a |GNU make|
based build system. To build and install DPack, the typical workflow is:

#. Prepare and collect workflow requirements,
#. |Configure| the construction logic,
#. |Build| programs, libraries, documentation, etc.,
#. |Install| components, copying files previously built to
   system-wide directories

Alternatively, you may replace the last step mentioned above with a |Staged
Install|. You will find below a **quick starting guide** showing how to build
DPack.

Preparation phase
-----------------

The overall :external+ebuild:ref:`eBuild Workflow <sect-user-workflow>` is
customizable thanks to multiple :command:`make` variable settings. You should
adjust values according to your specific needs.

Most of the time, setting |BUILDDIR|, |PREFIX|, |CROSS_COMPILE| is enough.
You should also set the :envvar:`PATH` environment variable according to the
set of tools required by the build process.

Optionally, you may set ``EXTRA_CFLAGS`` and ``EXTRA_LDFLAGS`` variables to
give the compiler and linker additional flags respectively.

Refer to :external+ebuild:ref:`eBuild Tools <sect-user-tools>` and
:external+ebuild:ref:`eBuild Variables <sect-user-variables>` for further
informations.

.. _workflow-configure-phase:
   
Configure phase
---------------

To begin with, |Configure| the build process interactively by running the
:external+ebuild:ref:`eBuild menuconfig target <target-menuconfig>`:

.. code-block:: console

   $ make menuconfig BUILDDIR=$HOME/build/dpack

Build phase
-----------

Now, proceed to the |Build| phase and compile / link programs, libraries, etc.
by running the :external+ebuild:ref:`eBuild build target <target-build>`:

.. code-block:: console

   $ make build BUILDDIR=$HOME/build/dpack PREFIX=/usr
 
Install phase
-------------

Finally, |Install| programs, libraries, etc.: by running the
:external+ebuild:ref:`eBuild install target <target-install>`:

.. code-block:: console
   
   $ make install BUILDDIR=$HOME/build/dpack PREFIX=/usr
 
Alternative staged install phase
--------------------------------

Alternatively, perform a |Staged install| by specifying the |DESTDIR| variable
instead:
   
.. code-block:: console

   $ make install BUILDDIR=$HOME/build/dpack PREFIX=/usr DESTDIR=$HOME/staging

Documentation generation
------------------------

You may generate DPack documentation by running the `doc` target like so:

.. code-block:: console

   $ make doc BUILDDIR=$HOME/build/dpack PREFIX=/usr

You may further install generated documentation by running the `install-doc`
target:

.. code-block:: console

   $ make install-doc BUILDDIR=$HOME/build/dpack PREFIX=/usr DESTDIR=$HOME/staging

Finally, you may find lots of usefull informations into the
:external+ebuild:ref:`Reference <sect-user-reference>` section of the |eBuild
User Guide|.
