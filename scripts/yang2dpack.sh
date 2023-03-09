#!/bin/sh -e

topdir=$(realpath $(dirname $0)/..)
builddir=${topdir}/build
venv=${builddir}/venv
yang2dpack=${venv}/bin/yang2dpack



if [ ! -d "${builddir}" ]; then
        mkdir -p ${builddir}
fi

if [ ! -f "${yang2dpack}" ]; then
        if [ -z "${PYTHON}" ]; then
                PYTHON=$(which python3)
        fi
        ${PYTHON} -m venv ${venv}
        ${venv}/bin/python -m pip install --upgrade pip
        ${venv}/bin/pip install git+https://github.com/mbj4668/pyang#egg=pyang
        ${venv}/bin/pip install -e ${topdir}/python
fi

PYTHONPYCACHEPREFIX=${builddir} exec ${yang2dpack} "$@"