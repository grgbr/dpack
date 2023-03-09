#!/bin/sh -e

topdir=$(realpath $(dirname $0)/..)
yang2dpack=${topdir}/scripts/yang2dpack.sh

exec ${yang2dpack} ${topdir}/test/yang/kvs-syslog.yang \
        --path="${topdir}/test/yang/modules" \
        --dpack-output-dir "${topdir}/build"
        --lint