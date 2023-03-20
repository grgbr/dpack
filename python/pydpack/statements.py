from pydpack.util import *
from pydpack import types
from Cheetah.Template import Template
from pydpack import templates
try:
    import importlib.resources as pkg_resources
except ImportError:
    # Try backported to PY<37 `importlib_resources`.
    import importlib_resources as pkg_resources

CHILDREN_STRUCTURE_MASK = ['container', 'list']

class Module(object):

    def __init__(self, ctx, node):
        self.ctx = ctx
        self.node = node
        self.node.dpack_module = self

        self.includes = set([
            'stdlib.h',
            'errno.h',
            'dpack/map.h'
        ])
        self.defines = [
            ('__warn_result', ["__attribute__((warn_unused_result))"]),
            ('__nonull(_arg_index, ...)',
                ["__attribute__((nonnull(_arg_index, ## __VA_ARGS__)))"]
            ),
        ]
        self.typedefs = []
        self.enums = []
        self.InlineFunctions = []
        self.ExternFunctions = []
        self.StaticFunctions = []
        self.structures = []
        
        self.name = self.node.arg
        self.define = f"_{todef(self.name)}_H"
        self.prefix = self.node.i_prefix
        if not self.prefix:
            self.prefix = self.node.arg

        self.assertFunction = f"{todef(self.prefix)}_ASSERT"

        addDefine(node.i_children[0], 
                  self.assertFunction + "(condition)", 
                  ["dpack_assert(condition)"])

        self.nameSpace = {
            "name":            self.name,
            "define":          self.define,
            "defines":         self.defines,
            "enums":           self.enums,
            "includes":        self.includes,
            "typedefs":        self.typedefs,
            "InlineFunctions": self.InlineFunctions,
            "StaticFunctions": self.StaticFunctions,
            "ExternFunctions": self.ExternFunctions,
            "structures":      self.structures,
        }
        chs = [ch for ch in self.node.i_children
            if ch.keyword in CHILDREN_STRUCTURE_MASK]
        for c in chs:
            self.__parseStructure(c)

    def __parseStructure(self, node):
        if hasattr(node, 'i_children'):
            chs = [ch for ch in node.i_children 
                  if ch.keyword in CHILDREN_STRUCTURE_MASK]
            for c in chs:
                self.__parseStructure(c)
        Struct(self.prefix, self.ctx, node)

    def applyTemplate(self, template):
        return str(Template(template, searchList=[self.nameSpace]))

class Struct(object):

    def __init__(self, prefix, ctx, node):
        self.ctx = ctx
        self.node = node
        self.prefix = prefix
        self.name = toid(self.node.arg)
        
        struct = []
        for c in self.node.i_children:
            name = toid(c.arg)
            if c.keyword in ['leaf', 'leaf-list']:
                type = types.getType(ctx, c)
            # elif c.keyword in ['container', 'list']:
            #     type = types.BaseType(
            #             f"struct {self.prefix}_{c.arg}",
            #             None,
            #             None,
            #             None,
            #             None,
            #             None
            #     )
            else:
                err_add(ctx.errors, c.pos,'BAD_TARGET_NODE', (self.prefix, c.arg, c.keyword))
                return

            if not type:
                return

            struct.append((type, name))

        name_struct = f"{self.prefix}_{self.name}"
        name_min = f"{todef(self.prefix)}_{todef(self.name)}_PACKED_SIZE_MIN"
        name_max = f"{todef(self.prefix)}_{todef(self.name)}_PACKED_SIZE_MAX"
        name_validFld = f"{todef(self.prefix)}_{todef(self.name)}_VALID_FLD_MSK"
        name_mandFld = f"{todef(self.prefix)}_{todef(self.name)}_MAND_FLD_MSK"
        name_mandNR = f"{todef(self.prefix)}_{todef(self.name)}_MAND_FLD_NR"
        fld_nr = todef(f"{name_struct}_FLD_NR")
        value_min = [f"DPACK_MAP_HEAD_SIZE({name_mandNR}) + "]
        value_max = [f"DPACK_MAP_HEAD_SIZE({fld_nr}) + "]
        value_struct = []

        nameSpace = {
            'struct_name': name_struct,
            'prefix': self.prefix,
            'name': self.name,
            'assert': getAssertFunction(node),
            'name_min': name_min,
            'name_max': name_max,
            'valid_fld': name_validFld,
            'mand_fld': name_mandFld,
            'mand_nr': name_mandNR,
            'fld_nr': fld_nr,
            'struct': struct,
            'must': parseMust(self.ctx, self.node),
        }

        mandFld = []
        enum_field = []
        for i, x in enumerate(struct):
            t, n = x
            t.addFunctions(n, name_struct)
            fld = todef(f"{name_struct}_{n}_FLD")
            enum_field.append((fld, i))
            value_struct.append((t.getType(), n))
            value_max.append(f"{t.getMax()} +")
            if t.isMandatory():
                value_min.append(f"{t.getMin()} +")
                mandFld.append(f"(1U << {fld}) |")
        
        enum_field.append((fld_nr, None))

        addDefine(node, name_mandNR, addBracket([f"{len(mandFld)}U"]))
        addDefine(node, name_validFld, addBracket([f"(1U << {fld_nr}) - 1"]))
        if mandFld:
            mandFld[-1] = mandFld[-1][:-2]
            if len(mandFld) == 1:
                mandFld[0] = mandFld[0][1:-1]
        else:
            mandFld = [0]
        addDefine(node, name_mandFld, addBracket(mandFld))

        addEnum(node, f"{name_struct}_field", enum_field)
        if value_min:
            value_min[-1] = value_min[-1][:-2]
        else:
            value_min = [0]
        if value_max:
            value_max[-1] = value_max[-1][:-2]
        else:
            value_max = [0]
        addDefine(node, name_min, addBracket(value_min))
        addDefine(node, name_max, addBracket(value_max))
        addStructure(node, name_struct, value_struct)
        addExternFunction(node, 
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_alloc", [], set(["__warn_result"])),
            [f"return malloc(sizeof(struct {self.prefix}_{self.name}));"])
        addExternFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_free", [
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set()),
            ["free(data);"])

        nameSpace['hasInit'] = False
        for t, _ in struct:
            if t.hasInit():
                nameSpace['hasInit'] = True
                break
        init_template = pkg_resources.read_text(templates, 'struct_init.c.tmpl')
        addExternFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_init", [
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__warn_result", "__nonull(1)"])),
            str(Template(init_template, searchList=[nameSpace])).splitlines())

        fini_template =pkg_resources.read_text(templates, 'struct_fini.c.tmpl')
        fini_data_attrs = set(["__unused"])
        for t, _ in struct:
            if t.hasFini():
                fini_data_attrs.remove("__unused")
                break
        addExternFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_fini", [
                (f"struct {self.prefix}_{self.name} *", "data", fini_data_attrs)
            ], set(["__nonull(1)"])),
            str(Template(fini_template, searchList=[nameSpace])).splitlines())

        create_template = pkg_resources.read_text(templates, 'struct_create.c.tmpl')
        addExternFunction(node, 
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_create", [], set(["__warn_result"])),
            str(Template(create_template, searchList=[nameSpace])).splitlines())

        addExternFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_destroy", [
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__nonull(1)"])),
            [f"{self.prefix}_{self.name}_fini(data);",
             f"{self.prefix}_{self.name}_free(data);"])

        pack_template = pkg_resources.read_text(templates, 'struct_pack.c.tmpl')
        addExternFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_pack", [
                (f"struct dpack_encoder *", "encoder", set()),
                (f"const struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__warn_result", "__nonull(1,2)"])),
            str(Template(pack_template, searchList=[nameSpace])).splitlines())

        unpack_template = pkg_resources.read_text(templates, 'struct_unpack.c.tmpl')
        addExternFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_unpack", [
                (f"struct dpack_decoder *", "decoder", set()),
                (f"struct {self.prefix}_{self.name} *", "data", set()),
            ], set(["__warn_result", "__nonull(1,2)"])),
            str(Template(unpack_template, searchList=[nameSpace, {'check':ctx.opts.dpack_no_check}])).splitlines())
        
        dispatch_template = pkg_resources.read_text(templates, 'struct_dispatch.c.tmpl')
        addStaticFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_dispatch_field", [
                (f"struct dpack_decoder *", "decoder", set()),
                (f"unsigned int", "fid", set()),
                (f"void *", "data", set()),
            ], set(["__warn_result", "__nonull(1,3)"])),
            str(Template(dispatch_template, searchList=[nameSpace])).splitlines())

        check_template = pkg_resources.read_text(templates, 'struct_check.c.tmpl')
        addExternFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_check", [
                (f"const struct {self.prefix}_{self.name} *", "data", set()),
            ], set(["__warn_result", "__nonull(1)"])),
            str(Template(check_template, searchList=[nameSpace])).splitlines())
