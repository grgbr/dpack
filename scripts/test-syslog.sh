#!/bin/sh -e

topdir=$(realpath $(dirname $0)/..)
yang2dpack=${topdir}/scripts/yang2dpack.sh

exec ${yang2dpack} ${topdir}/sample/yang/kvs-syslog.yang \
        --dpack-output-dir "${topdir}/build" \
        --lint