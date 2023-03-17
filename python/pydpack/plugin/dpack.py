from pyang import plugin, error, statements
from pydpack.statements import Module
import optparse
from pydpack import templates
try:
    import importlib.resources as pkg_resources
except ImportError:
    # Try backported to PY<37 `importlib_resources`.
    import importlib_resources as pkg_resources


def pyang_plugin_init():
    plugin.register_plugin(DpackPlugin())

class DpackPlugin(plugin.PyangPlugin):
    def __init__(self):
        plugin.PyangPlugin.__init__(self, 'dpack')

    def add_output_format(self, fmts):
        self.multiple_modules = True
        fmts['dpack'] = self



    def setup_fmt(self, ctx):
        ctx.implicit_errors = True
        ctx.strict = False
        statements.add_xpath_function('checker', ['string'], 'boolean')

    def add_opts(self, optparser):
        optlist = [
            optparse.make_option(
                "--dpack-output-dir",
                metavar="DIR",
                dest="dpack_output_dir",
                help="""Split the code output into
                    multiple directories""",
            ),
            optparse.make_option(
                "--dpack-no-check",
                dest="dpack_no_check",
                action="store_false",
                default=True,
                help="""don't call checkers function in unpack process""",
            )
        ]
        option_group = optparse.OptionGroup(optparser, "dpack output specific options")
        option_group.add_options(optlist)
        optparser.add_option_group(option_group)

    def post_validate_ctx(self, ctx, modules):
        ctx.dpackModules = []
        for module in modules:
                ctx.dpackModules.append(Module(ctx, module))

    def emit(self, ctx, _, fd):
        for _, etag, _ in ctx.errors:
            if error.is_error(error.err_level(etag)):
                raise error.EmitError("dpack plugin needs a valid module")
        ct = pkg_resources.read_text(templates, 'module.c.tmpl')
        ht = pkg_resources.read_text(templates, 'module.h.tmpl')
        if not ctx.opts.dpack_output_dir:
            for m in ctx.dpackModules:
                fd.write(m.applyTemplate(ht))
                fd.write(m.applyTemplate(ct))
        else:
            for m in ctx.dpackModules:
                with(open(f"{ctx.opts.dpack_output_dir}/{m.name}.h", "w")) as f:
                    f.write(m.applyTemplate(ht))
                with(open(f"{ctx.opts.dpack_output_dir}/{m.name}.c", "w")) as f:
                    f.write(m.applyTemplate(ct))
