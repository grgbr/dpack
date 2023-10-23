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


def sort_uniq(collection):
    #return sorted(list(dict.fromkeys(collection)))
    return sorted(tuple(frozenset(collection)))


def make_conf(syms):
    def flatten(collection):
        for item in collection:
            if isinstance(item, (list, tuple, set, frozenset)):
                yield from flatten(item)
            else:
                yield str(item)

    return tuple(sorted(frozenset(flatten(syms))))


def conf_db(kconf):
    lst = []
    for cfg in itertools.product(*conf_syms):
        lst.append(make_conf(cfg))
    return sort_uniq(lst)


def conf_str(db, index):
    return '\n'.join(db[index])


def gen_conf(db, index, path):
    with open(path, 'wt') as f:
        print(conf_str(db, index), file=f)


def count_conf(kconf):
    return len(conf_db(kconf))


def show_one_conf(kconf, index):
    db = conf_db(kconf)
    if index < 0 or index >= len(db):
        raise Exception("cannot show configuration '{}': "
                        "invalid specified index".format(index))
    print(conf_str(db, index))


def gen_one_conf(kconf, index, path):
    db = conf_db(kconf)
    if index < 0 or index >= len(db):
        raise Exception("cannot generate configuration '{}': "
                        "invalid specified index".format(index))
    gen_conf(conf_db(kconf), index, path)


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
    db = conf_db(kconf)
    for idx in range(len(db)):
        fpath = os.path.join(out_dpath, 'conf{}.in'.format(idx))
        gen_conf(db, idx, fpath)

def init_kconf(path):
    try:
        kconf = Kconfig(path)
    except Exception as e:
        raise Exception("cannot parse KConfig file '{}': {}".format(path, e))
    if not len(kconf.unique_defined_syms):
        raise Exception("empty KConfig file '{}'".format(path))
    return kconf


def main():
    global arg0

    arg0 = os.path.basename(sys.argv[0])

    parser = argparse.ArgumentParser(description = 'DPack testing build '
                                                   'configuration generator')
    parser.add_argument('kconf_fpath',
                        nargs = 1,
                        default = None,
                        metavar = 'KCONF_FILEPATH',
                        help = 'Pathname to KConfig file')
    subparser = parser.add_subparsers(dest = 'cmd')
    subparser.add_parser('count', help = 'Display count of configurations')
    show_parser = subparser.add_parser('show',
                                       help = 'Show content of a configuration')
    show_parser.add_argument('index',
                             nargs = 1,
                             type = int,
                             default = None,
                             metavar = 'CONF_INDEX',
                             help = 'Configuration index')
    genone_parser = subparser.add_parser('genone',
                                         help = 'Generate one configuration')
    genone_parser.add_argument('index',
                               nargs = 1,
                               type = int,
                               default = None,
                               metavar = 'CONF_INDEX',
                               help = 'Configuration index')
    genone_parser.add_argument('out_fpath',
                               nargs = 1,
                               type = str,
                               default = None,
                               metavar = 'CONF_FILEPATH',
                               help = 'Pathname to output configuration file')
    genall_parser = subparser.add_parser('genall',
                                         help = 'Generate all configurations')
    genall_parser.add_argument('out_dpath',
                               nargs = 1,
                               type = str,
                               default = None,
                               metavar = 'CONF_DIRPATH',
                               help = 'Pathname to configurations '
                                      'output directory')

    args = parser.parse_args()

    try:
        kconf = init_kconf(args.kconf_fpath[0])
        cmd = args.cmd
        if cmd == 'count':
            print(count_conf(kconf))
        elif cmd == 'show':
            show_one_conf(kconf, args.index[0])
        elif cmd == 'genone':
            gen_one_conf(kconf, args.index[0], args.out_fpath[0])
        else:
            gen_all_confs(kconf, args.out_dpath[0])
    except Exception as e:
        print("{}: {}.".format(arg0, e), file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
