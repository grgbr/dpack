from pydpack.util import *
from pydpack import types
from Cheetah.Template import Template

CHILDREN_STRUCTURE_MASK = ['container', 'list']

class Module(object):

    def __init__(self, ctx, node):
        self.ctx = ctx
        self.node = node
        self.node.dpack_module = self

        self.includes = set(['stdlib.h', 'errno.h'])
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

        nameSpace = {
            'prefix': self.prefix,
            'name': self.name,
            'assert': getAssertFunction(node),
            'name_min': name_min,
            'name_max': name_max,
            'struct': struct
        }

        for t, n in struct:
            t.addFunctions(n, name_struct)

            value_struct.append((t.getType(), n))

            value_min.append(f"{t.getMin()} +")
            value_max.append(f"{t.getMax()} +")

        value_min[-1] = value_min[-1][:-2]
        value_max[-1] = value_max[-1][:-2]
        addDefine(node, name_min, addBracket(value_min))
        addDefine(node, name_max, addBracket(value_max))
        addStructure(node, name_struct, value_struct)
        addFunction(node, 
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_alloc", []),
            [f"return malloc(sizeof(struct {self.prefix}_{self.name}));"])
        addFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_free", [(f"struct {self.prefix}_{self.name} *", "data")]),
            ["free(data);"])
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_init", [(f"struct {self.prefix}_{self.name} *", "data")]),
            ["UNUSED(data);", "", "return 0;"])

        fini_template = '''\
#set $unused = True
#for $t, $n in $struct
#if $t.mustFree()
#set $unused = False
#echo $t.getFree('data->' + $n) + ';'
#end if
#end for
#if $unused
UNUSED(data);
#end if
'''
        addFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_fini", [(f"struct {self.prefix}_{self.name} *", "data")]),
            str(Template(fini_template, searchList=[nameSpace])).splitlines())

        create_template = '''\
struct $(prefix)_$(name) *data;

data = $(prefix)_$(name)_alloc();
if (data == NULL)
	return NULL;

if ($(prefix)_$(name)_init(data) == 0)
	return data;

$(prefix)_$(name)_free(data);
return NULL;
'''
        addFunction(node, 
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_create", []),
            str(Template(create_template, searchList=[nameSpace])).splitlines())

        pack_template = '''\
$(assert)(encoder);
$(assert)(data);
$(assert)(dpack_encoder_space_left(encoder) >=
#echo ' ' * (len($assert) + 1) + $name_min + ');'


int err;

#for $t, $n in $struct
#echo 'err = ' + $t.getEncode('encoder', 'data->' + $n) + ';'

if (err)
	return err;

#end for
return 0;
'''

        addFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_destroy", [(f"struct {self.prefix}_{self.name} *", "data")]),
            [f"{self.prefix}_{self.name}_fini(data);",
             f"{self.prefix}_{self.name}_free(data);"])

        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_pack", [
                (f"struct dpack_encoder *", "encoder"),
                (f"const struct {self.prefix}_{self.name} *", "data")
            ]), str(Template(pack_template, searchList=[nameSpace])).splitlines())

        unpack_template = '''\
$(assert)(decoder);
$(assert)(data);
$(assert)(dpack_decoder_data_left(decoder) >=
#echo ' ' * (len($assert) + 1) + $name_max + ');'


int err;

#for $t, $n in $struct
#echo 'err = ' + $t.getDecode('decoder', '&data->' + $n) + ';'

if (err)
	return err;

err = $(prefix)_$(name)_check_$(n)(data->$n);
if (err)
	return err;

#end for
return 0;
'''
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_unpack", [
                (f"struct dpack_decoder *", "decoder"),
                (f"struct {self.prefix}_{self.name} *", "data")
            ]), str(Template(unpack_template, searchList=[nameSpace])).splitlines())
