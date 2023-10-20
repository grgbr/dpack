#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################


import sys
import os
import argparse
import itertools
from kconfiglib import Kconfig

conf_syms = frozenset({
    frozenset({
        frozenset({ 'CONFIG_DPACK_ASSERT_API=n',
                    'CONFIG_DPACK_ASSERT_INTERN=n',
                    'CONFIG_DPACK_DEBUG=n' }),
        frozenset({ 'CONFIG_DPACK_ASSERT_API=y' }),
        frozenset({ 'CONFIG_DPACK_ASSERT_INTERN=y' }),
        frozenset({ 'CONFIG_DPACK_DEBUG=y' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_SCALAR=n' }),
        frozenset({ 'CONFIG_DPACK_SCALAR=y' }),
        frozenset({ 'CONFIG_DPACK_SCALAR=y',
                    'CONFIG_DPACK_FLOAT=y',
                    'CONFIG_DPACK_DOUBLE=n' }),
        frozenset({ 'CONFIG_DPACK_SCALAR=y',
                    'CONFIG_DPACK_FLOAT=n',
                    'CONFIG_DPACK_DOUBLE=y' }),
        frozenset({ 'CONFIG_DPACK_SCALAR=y',
                    'CONFIG_DPACK_FLOAT=y',
                    'CONFIG_DPACK_DOUBLE=y' }),
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_STRING=y',
                    'CONFIG_DPACK_STRING_MAXLEN=31' }),
        frozenset({ 'CONFIG_DPACK_STRING=y',
                    'CONFIG_DPACK_STRING_MAXLEN=255' }),
        frozenset({ 'CONFIG_DPACK_STRING=y',
                    'CONFIG_DPACK_STRING_MAXLEN=65535' }),
        frozenset({ 'CONFIG_DPACK_STRING=y',
                    'CONFIG_DPACK_STRING_MAXLEN=4294967294' }),
        frozenset({ 'CONFIG_DPACK_STRING=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_LVSTR=y' }),
        frozenset({ 'CONFIG_DPACK_LVSTR=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_BIN=y' }),
        frozenset({ 'CONFIG_DPACK_BIN=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_ARRAY=y' }),
        frozenset({ 'CONFIG_DPACK_ARRAY=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_MAP=y' }),
        frozenset({ 'CONFIG_DPACK_MAP=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_UTEST=y', 'CONFIG_DPACK_VALGRIND=y' }),
        frozenset({ 'CONFIG_DPACK_UTEST=y', 'CONFIG_DPACK_VALGRIND=n' })
    }),
    frozenset({
        frozenset({ 'CONFIG_DPACK_SAMPLE=y' }),
        frozenset({ 'CONFIG_DPACK_SAMPLE=n' })
    }),
})


def build_conf(syms):
    def flatten(collection):
        for item in collection:
            if isinstance(item, (list, tuple, set, frozenset)):
                yield from flatten(item)
            else:
                yield str(item)

    return tuple(sorted(frozenset(flatten(syms))))


def gen_conf(path, syms):
    with open(path, 'wt') as f:
        print('\n'.join(build_conf(syms)), file=f)


def gen_all_confs(kconf, out_dpath):
    try:
        os.mkdir(out_dpath)
    except FileExistsError:
        pass
    for root, dirs, files in os.walk(out_dpath, topdown=False):
        for name in files:
            os.unlink(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))
    
    idx = 0
    for cfg in itertools.product(*conf_syms):
        fpath = os.path.join(out_dpath, 'conf{}.in'.format(idx))
        gen_conf(fpath, cfg)
        idx += 1


def main():
    global arg0

    arg0 = os.path.basename(sys.argv[0])

    parser = argparse.ArgumentParser(
        description='DPack test build configuration generator')
    parser.add_argument('kconf_fpath',
                        nargs = 1,
                        default = None,
                        metavar = 'KCONFIG_FILEPATH',
                        help = 'Pathname to KConfig file')
    parser.add_argument('out_dpath',
                        nargs = 1,
                        default = None,
                        metavar = 'OUTPUT_DIRPATH',
                        help = 'Pathname to configurations output directory')

    args = parser.parse_args()

    try:
        kconf = Kconfig(args.kconf_fpath[0])
    except Exception as e:
        print("{}: KConfig parsing failed: {}.".format(arg0, e),
              file=sys.stderr)
        sys.exit(1)

    if not len(kconf.unique_defined_syms):
        print("{}: Empty KConfig file.".format(arg0), file=sys.stderr)
        sys.exit(1)

    if not len(args.out_dpath[0]):
        print("{}: '{}': Invalid configurations "
              "output directory.".format(arg0, args.out_dpath[0]),
              file=sys.stderr)
        sys.exit(1)

    try:
        gen_all_confs(kconf, args.out_dpath[0])
    except Exception as e:
        print("{}: test build configurations "
              "generation failed: {}.".format(arg0, e), file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
