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
import msgpack
from Cheetah.Template import Template

def make_hexstr(octets: bytes):
    return ''.join('\\x{:02x}'.format(o) for o in octets)


def pack_array(packer, array):
    return make_hexstr(packer.pack(array))


def pack_tuple(packer, uplet):
    return ''.join(make_hexstr(packer.pack(t)) for t in uplet)


def pack_dict(packer, dic):
    return make_hexstr(packer.pack(dic))


def pack_scalar(packer, scalar):
    return make_hexstr(packer.pack(scalar))


def pack(packer, data):
    if isinstance(data, list):
        return pack_array(packer, data)
    elif isinstance(data, tuple):
        return pack_tuple(packer, data)
    elif isinstance(data, dict):
        return pack_dict(packer, data)
    elif isinstance(data, int) or \
         isinstance(data, float) or \
         isinstance(data, bool) or \
         isinstance(data, str):
        return pack_scalar(packer, data)
    else:
        raise TypeError("{}: Unsupported type.".format(type(data)))


def main():
    global arg0

    arg0 = os.path.basename(sys.argv[0])

    parser = argparse.ArgumentParser(description='DPack unit test writer helper'
                                                 'tool')
    parser.add_argument('expr',
                        nargs=1,
                        default=None,
                        metavar='EXPRESSION',
                        help='Python expression')
    parser.add_argument('-s',
                        '--single-float',
                        action='store_true',
                        dest='float',
                        help='Use single precision floating point numbers')

    args = parser.parse_args()

    try:
        packer = msgpack.Packer(autoreset=True,
                                use_bin_type=True,
                                use_single_float=args.float,
                                strict_types=True,
                                datetime=False)
    except Exception as e:
        print("{}: msgpack packer instantiation failed: {}.".format(arg0, e),
              file=sys.stderr)
        sys.exit(1)

    try:
        data = eval(args.expr[0])
    except Exception as e:
        print("{}: invalid python expression: {}.".format(arg0, e),
              file=sys.stderr)
        sys.exit(1)

    try:
        print(pack(packer, data))
    except Exception as e:
        print("{}: msgpack packing failed: {}.".format(arg0, e),
              file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
