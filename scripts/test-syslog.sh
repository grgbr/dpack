#!/bin/sh -e

topdir=$(realpath $(dirname $0)/..)
yang2dpack=${topdir}/scripts/yang2dpack.sh

exec ${yang2dpack} ${topdir}/test/yang/modules/ietf-syslog.yang \
        --path="${topdir}/test/yang/modules" \
        --deviation-module="${topdir}/test/yang/dpack-syslog-deviation.yang" \
        --exclude-features="ietf-syslog:signed-messages,select-match,select-adv-compare" \
        --lint