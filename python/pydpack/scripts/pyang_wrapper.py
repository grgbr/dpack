import sys
import os
from pydpack import modules
from pydpack.plugin import dpack
from pyang import plugin
from pyang.scripts import pyang_tool

def init_maker(init):

    def new_init(plugindirs=None):
        init(plugindirs)
        plugins_blacklist = []
        for p in plugin.plugins:
            if p.add_output_format.__module__ != 'pyang.plugin':
                plugins_blacklist.append(p)
        for p in plugins_blacklist:
            plugin.plugins.remove(p)
        dpack.pyang_plugin_init()

    return new_init

def run():
    m = sys.modules['pyang.plugin']
    m.init = init_maker(m.init)
    sys.modules['pyang.plugin'] = m
    fix_path = os.path.dirname(modules.__file__)
    for i, v in enumerate(sys.argv):
        if v == "-p" or v.startswith("--path"):
            if v.startswith("--path="):
                sys.argv[i] += f':{fix_path}'
            else:
                sys.argv[i + 1] += f':{fix_path}'
            fix_path = None
    if fix_path:
        sys.argv.append(
            f'--path={fix_path}'
        )
    sys.argv.extend([
            "-f", "dpack"
        ])
    sys.exit(pyang_tool.run())
