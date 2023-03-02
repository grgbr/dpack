from pyang import plugin
from pydpack.statements import Module
import optparse



def pyang_plugin_init():
    plugin.register_plugin(DpackPlugin())

class DpackPlugin(plugin.PyangPlugin):
    def __init__(self):
        plugin.PyangPlugin.__init__(self, 'dpack')

    def add_output_format(self, fmts):
        self.multiple_modules = True
        fmts['dpack'] = self

    def add_opts(self, optparser):
        optlist = [
            optparse.make_option(
                "--dpack-output-dir",
                metavar="DIR",
                dest="dpack_output_dir",
                help="""Split the code output into
                    multiple directories""",
            )
        ]
        option_group = optparse.OptionGroup(optparser, "dpack output specific options")
        option_group.add_options(optlist)
        optparser.add_option_group(option_group)

    def emit(self, ctx, modules, fd):
        if not ctx.opts.dpack_output_dir:
            for module in modules:
                m = Module(ctx, module)
                fd.write(m.getH())
                fd.write(m.getC())
        else:
            for module in modules:
                m = Module(ctx, module)
                with(open(f"{ctx.opts.dpack_output_dir}/{m.name}.h", "w")) as f:
                    f.write(m.getH())
                with(open(f"{ctx.opts.dpack_output_dir}/{m.name}.c", "w")) as f:
                    f.write(m.getC())
