from pydpack.util import *
from pydpack import types
from Cheetah.Template import Template

CHILDREN_STRUCTURE_MASK = ['container', 'list']

class Module(object):

    def __init__(self, ctx, node):
        self.ctx = ctx
        self.node = node
        self.node.dpack_module = self

        self.includes = set(['stdlib.h'])
        self.defines = []
        self.typedefs = []
        self.InlineFunctions = []
        self.functions = []
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
            "includes":        self.includes,
            "typedefs":        self.typedefs,
            "InlineFunctions": self.InlineFunctions,
            "functions":       self.functions,
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
            elif c.keyword in ['container', 'list']:
                type = types.BaseType(
                        f"struct {self.prefix}_{c.arg}",
                        None,
                        None,
                        None,
                        None,
                        None
                )
            else:
                raise BaseException(f"{c.pos} bad keyword {c.keyword}")
            struct.append((type, name))

        name_struct = f"{self.prefix}_{self.name}"
        name_min = f"{todef(self.prefix)}_{todef(self.name)}_PACKED_SIZE_MIN"
        name_max = f"{todef(self.prefix)}_{todef(self.name)}_PACKED_SIZE_MAX"
        value_min = []
        value_max = []
        value_struct = []
        value_pack = []
        value_unpack = []
        value_free = []

        value_free.append(f"{getAssertFunction(node)}(data);")
        newline(value_free)

        value_pack.append(f"{getAssertFunction(node)}(encoder);")
        value_pack.append(f"{getAssertFunction(node)}(data);")
        value_pack.append(f"{getAssertFunction(node)}(dpack_encoder_space_left(encoder) >=")
        value_pack.append(f"{' ' * (len(getAssertFunction(node)) + 1)}{name_min});")
        newline(value_pack)
        value_pack.append("int err;")
        newline(value_pack)

        value_unpack.append(f"{getAssertFunction(node)}(decoder)")
        value_unpack.append(f"{getAssertFunction(node)}(data)")
        value_unpack.append(f"{getAssertFunction(node)}(dpack_decoder_data_left(decoder) >=")
        value_unpack.append(f"{' ' * (len(getAssertFunction(node)) + 1)}{name_max});")
        newline(value_unpack)
        value_unpack.append("int err;")
        newline(value_unpack)

        for t, n in struct:
            value_struct.append((t.getType(), n))
            x = t.getMin()
            if len(x) == 1:
                value_min.append(f"{x[0]} +")
            else:
                x = addBracket(x)
                x[-1] += " +"
                value_min.extend(x)
            
            x = t.getMax()
            if len(x) == 1:
                value_max.append(f"{x[0]} +")
            else:
                x = addBracket(x)
                x[-1] += " +"
                value_max.extend(x)

            value_pack.append(f"err = {t.getEncode('encoder', f'data->{n}')};")
            value_pack.append("if (err)")
            value_pack.append("\treturn err;")
            newline(value_pack)

            value_unpack.append(f"err = {t.getDecode('decoder', f'data->{n}')};")
            value_unpack.append("if (err)")
            value_unpack.append("\treturn err;")
            newline(value_unpack)

            if t.mustFree():
                value_free.append(f"if (data->{n})")
                value_free.append(f"\t{t.getFree(f'data->{n}')};")

        value_pack.append("return 0;")
        value_unpack.append("return 0;")

        value_min[-1] = value_min[-1][:-2]
        value_max[-1] = value_max[-1][:-2]
        addDefine(node, name_min, addBracket(value_min))
        addDefine(node, name_max, addBracket(value_max))
        addStructure(node, name_struct, value_struct)
        addInlineFunction(node, 
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_alloc", []),
            [f"return malloc(sizeof(struct {self.prefix}_{self.name}));"])
        value_free.append("return free(data);")
        addInlineFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_free", [(f"struct {self.prefix}_{self.name} *", "data")]),
            value_free)
        
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_pack", [
                (f"struct dpack_encoder *", "encoder"),
                (f"const struct {self.prefix}_{self.name} *", "data")
            ]), value_pack)
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_unpack", [
                (f"struct dpack_decoder *", "decoder"),
                (f"const struct {self.prefix}_{self.name} *", "data")
            ]), value_unpack)
