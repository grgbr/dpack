#!/usr/bin/python3
# -*- coding: utf-8 -*-

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
         isinstance(data, bool):
        return pack_scalar(packer, data)
    else:
        raise TypeError("{}: Unsupported type.".format(type(data)))


def gen_scalar(packer, type_name, literal_suffix, tests, outfile):
    conf = {
        'cmd':            arg0 +
                          ''.join(' {}'.format(arg) for arg in sys.argv[1:]),
        'packer':         packer,
        'pack':           pack,
        'type_name':      type_name,
        'literal_suffix': literal_suffix,
        'tests':          tests
    }
    TMPL = """
#compiler-settings
directiveStartToken = @
#end compiler-settings
/*
 * Automatically generated thanks to the following command:
 *     $cmd
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_${type_name}(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	${type_name}_t val;

	assert_int_equal(dpack_decode_${type_name}(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->${type_name});
}

const struct dpack_scalar_utest dpack_scalar_utest_${type_name} = {
	.unpack = dpack_scalar_utest_unpack_${type_name},
	.data   = {
@for $t in $tests
    @set $v = $t.value
    @set $e = $t.error
    @set $b = $pack($packer, $v)
    @set $l = $int($len($b) / 4)
		/* $t.value */
    @if $e == '0':
		{ .packed = "$b", .size = $l, .error = 0, .${type_name} = ${v}${literal_suffix} },
    @else
		{ .packed = "$b", .size = $l, .error = $e },
    @end if
@end for
		/* End of operations marker */
		{ NULL, }
	}
};
"""

    if outfile:
        outfile = open(outfile, "w")
    else:
        outfile = sys.stdout
    print(Template(TMPL, searchList = [ conf ]), file=outfile)


def main():
    global arg0

    arg0 = os.path.basename(sys.argv[0])

    parser = argparse.ArgumentParser(description='DPack unit test writer helper'
                                                 'tool')
    parser.add_argument('--outfile',
                        nargs=1,
                        default=None,
                        metavar='OUTFILE',
                        help='Output file')
    subparsers = parser.add_subparsers(dest='cmd',
                                       required=True,
                                       metavar='COMMAND')
    pack_parser = subparsers.add_parser('pack',
                                        help='Parse a python expression and '
                                             'serialize as a stringified '
                                             'msgpack byte stream.')
    pack_parser.add_argument('expr',
                             nargs=1,
                             default=None,
                             metavar='EXPRESSION',
                             help='Python expression')
    gen_parser = subparsers.add_parser('gen', help='Generate C')
    gen_parser.add_argument('kind',
                            choices = [ 'uint8', 'int8',
                                        'uint16', 'int16',
                                        'uint32', 'int32',
                                        'uint64', 'int64' ],
                            nargs=1,
                            default=None,
                            help='Kind of generator')

    args = parser.parse_args()

    try:
        packer = msgpack.Packer(autoreset=True,
                                use_bin_type=True,
                                strict_types=True,
                                datetime=False)
    except Exception as e:
        print("{}: msgpack packer instantiation failed: {}.".format(arg0, e),
              file=sys.stderr)
        sys.exit(1)

    if args.cmd == 'pack':
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

    elif args.cmd == 'gen':
        kind = args.kind[0]
        literal_suffix = ''
        tests = []
        try:
            if kind == 'uint8':
                literal_suffix = 'U'
                tests = [
                    { 'value': -1,           'error': '-ENOMSG' },
                    { 'value': 0,            'error': '0' },
                    { 'value': 1,            'error': '0' },
                    { 'value': (1 << 8) - 1, 'error': '0' },
                    { 'value': 1 << 8,       'error': '-ERANGE' }
                ]
            elif kind == 'int8':
                literal_suffix = ''
                tests = [
                    { 'value': -(1 << (8 - 1)) - 1, 'error': '-ERANGE' },
                    { 'value': -(1 << (8 - 1)),     'error': '0' },
                    { 'value': -(1 << (8 - 1)) + 1, 'error': '0' },
                    { 'value': -1,                  'error': '0' },
                    { 'value': 0,                   'error': '0' },
                    { 'value': 1,                   'error': '0' },
                    { 'value': (1 << (8 - 1)) - 1,  'error': '0' },
                    { 'value': 1 << (8 - 1),        'error': '-ERANGE' }
                ]
            elif kind == 'uint16':
                literal_suffix = 'U'
                tests = [
                    { 'value': -1,            'error': '-ENOMSG' },
                    { 'value': 0,             'error': '0' },
                    { 'value': 1,             'error': '0' },
                    { 'value': (1 << 16) - 1, 'error': '0' },
                    { 'value': 1 << 16,       'error': '-ERANGE' }
                ]
            elif kind == 'int16':
                literal_suffix = ''
                tests = [
                    { 'value': -(1 << (16 - 1)) - 1, 'error': '-ERANGE' },
                    { 'value': -(1 << (16 - 1)),     'error': '0' },
                    { 'value': -(1 << (16 - 1)) + 1, 'error': '0' },
                    { 'value': -1,                   'error': '0' },
                    { 'value': 0,                    'error': '0' },
                    { 'value': 1,                    'error': '0' },
                    { 'value': (1 << (16 - 1)) - 1,  'error': '0' },
                    { 'value': 1 << (16 - 1),        'error': '-ERANGE' }
                ]
            elif kind == 'uint32':
                literal_suffix = 'U'
                tests = [
                    { 'value': -1,            'error': '-ENOMSG' },
                    { 'value': 0,             'error': '0' },
                    { 'value': 1,             'error': '0' },
                    { 'value': (1 << 32) - 1, 'error': '0' },
                    { 'value': 1 << 32,       'error': '-ERANGE' }
                ]
            elif kind == 'int32':
                literal_suffix = ''
                tests = [
                    { 'value': -(1 << (32 - 1)) - 1, 'error': '-ERANGE' },
                    { 'value': -(1 << (32 - 1)),     'error': '0' },
                    { 'value': -(1 << (32 - 1)) + 1, 'error': '0' },
                    { 'value': -1,                   'error': '0' },
                    { 'value': 0,                    'error': '0' },
                    { 'value': 1,                    'error': '0' },
                    { 'value': (1 << (32 - 1)) - 1,  'error': '0' },
                    { 'value': 1 << (32 - 1),        'error': '-ERANGE' }
                ]
            elif kind == 'uint64':
                literal_suffix = 'U'
                tests = [
                    { 'value': -1,            'error': '-ENOMSG' },
                    { 'value': 0,             'error': '0' },
                    { 'value': 1,             'error': '0' },
                    { 'value': (1 << 64) - 1, 'error': '0' }
                ]
            elif kind == 'int64':
                literal_suffix = ''
                tests = [
                    { 'value': 1 << (64 - 1),        'error': '-ERANGE' },
                    { 'value': -(1 << (64 - 1)),     'error': '0' },
                    { 'value': -(1 << (64 - 1)) + 1, 'error': '0' },
                    { 'value': -1,                   'error': '0' },
                    { 'value': 0,                    'error': '0' },
                    { 'value': 1,                    'error': '0' },
                    { 'value': (1 << (64 - 1)) - 1,  'error': '0' },
                ]
        except Exception as e:
            print("{}: generation failed: {}.".format(arg0, e), file=sys.stderr)
            sys.exit(1)

        try:
            gen_scalar(packer,
                       type_name = kind,
                       literal_suffix = literal_suffix,
                       tests = tests,
                       outfile = args.outfile[0])
        except Exception as e:
            print("{}: source generation failed: {}.".format(arg0, e),
                  file=sys.stderr)
            sys.exit(1)


if __name__ == "__main__":
    main()
