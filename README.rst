.. This file is part of rbh-find-iosea
   Copyright (C) 2022 Commissariat a l'energie atomique et aux energies
                      alternatives

   SPDX-License-Identifer: LGPL-3.0-or-later

##############
rbh-find-iosea
##############

rbh-find-iosea is an overload of `rbh-find(1)`__.

.. __: rbh-find_
.. _rbh-find: https://github.com/cea-hpc/rbh-find.git

Installation
============

Install the `RobinHood library`_ and the `rbh-find library`_ first, then
download the sources:

.. code:: bash

    git clone https://github.com/cea-hpc/rbh-find-iosea.git
    cd rbh-find-iosea

Build and install with meson_ and ninja_:

.. code:: bash

    meson builddir
    ninja -C builddir
    sudo ninja -C builddir install

.. _meson: https://mesonbuild.com
.. _ninja: https://ninja-build.org
.. _RobinHood library: https://github.com/cea-hpc/librobinhood
.. _rbh-find library: https://github.com/cea-hpc/rbh-find

A work in progress
==================

rbh-find-iosea is a close twin of `rbh-find(1)`__. It focuses more on the use
of the MOtr backend of the `RobinHood library`_. As such, it will implement
some MOtr specific features, but will still behave exactly like `rbh-find`.

.. __: _rbh-find

Usage
=====

`rbh-ifind` works so much like `rbh-find` that documenting how it works would
look a lot like `rbh-find's documentation`_. Instead, we will document the
differences of `rbh-ifind` compared to `rbh-find`.

.. _: https://github.com/cea-hpc/rbh-find/README.rst

Extra features
==============

-tier
----------

rbh-ifind defines a ``-tier`` predicate that filters the entries based on the
tier they are on. It should be given a numerical value corresponding to the
targeted tier index.

.. code:: bash

Examples
--------

WORK IN PROGRESS
