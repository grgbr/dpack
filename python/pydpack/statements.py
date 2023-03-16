from pydpack.util import *
from pydpack import types
from Cheetah.Template import Template

CHILDREN_STRUCTURE_MASK = ['container', 'list']

class Module(object):

    def __init__(self, ctx, node):
        self.ctx = ctx
        self.node = node
        self.node.dpack_module = self

        self.includes = set([
            'stdlib.h',
            'errno.h',
        ])
        self.defines = [
            ('__unused', ["__attribute__((unused))"]),
            ('__warn_result', ["__attribute__((warn_unused_result))"]),
            ('__nonull(_arg_index, ...)',
                ["__attribute__((nonnull(_arg_index, ## __VA_ARGS__)))"]
            ),
        ]
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
            'struct': struct,
            'must': parseMust(self.ctx, self.node),
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
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_alloc", [], set(["__warn_result"])),
            [f"return malloc(sizeof(struct {self.prefix}_{self.name}));"])
        addFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_free", [
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set()),
            ["free(data);"])

        init_data_attrs = set(["__unused"])
        for t, _ in struct:
            if t.getInit(''):
                init_data_attrs.remove("__unused")
                break
        init_template = '''\
#for $t, $n in $struct
#set $init = $t.getInit('data->' + $n)
#if $init
$init;
#end if
#end for
return 0;
'''
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_init", [
                (f"struct {self.prefix}_{self.name} *", "data", init_data_attrs)
            ], set(["__warn_result", "__nonull(1)"])),
            str(Template(init_template, searchList=[nameSpace])).splitlines())

        fini_template = '''\
#for $t, $n in $struct
#set $free = $t.getFree('data->' + $n)
#if $free
$free;
#end if
#end for
'''
        fini_data_attrs = set(["__unused"])
        for t, _ in struct:
            if t.getFree(''):
                fini_data_attrs.remove("__unused")
                break
        addFunction(node, 
            (f"void", f"{self.prefix}_{self.name}_fini", [
                (f"struct {self.prefix}_{self.name} *", "data", fini_data_attrs)
            ], set(["__nonull(1)"])),
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
            (f"struct {self.prefix}_{self.name} *", f"{self.prefix}_{self.name}_create", [], set(["__warn_result"])),
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
            (f"void", f"{self.prefix}_{self.name}_destroy", [
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__nonull(1)"])),
            [f"{self.prefix}_{self.name}_fini(data);",
             f"{self.prefix}_{self.name}_free(data);"])

        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_pack", [
                (f"struct dpack_encoder *", "encoder", set()),
                (f"const struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__warn_result", "__nonull(1,2)"])),
            str(Template(pack_template, searchList=[nameSpace])).splitlines())

        unpack_template = '''\
$(assert)(decoder);
$(assert)(dpack_decoder_data_left(decoder) >=
#echo ' ' * (len($assert) + 1) + $name_min + ');'

$(assert)(data);


int err;

#for $t, $n in $struct
#echo 'err = ' + $t.getDecode('decoder', '&data->' + $n) + ';'

if (err)
	return err;
#if $check
err = $(prefix)_$(name)_check_$(n)(data->$n);
if (err)
	return err;
#end if
    
#end for
#if $check and $must
#for $f in $must
err = $(f)(value);
if (err)
	return err;

#end for
#end if
return 0;
'''
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_unpack", [
                (f"struct dpack_decoder *", "decoder", set()),
                (f"struct {self.prefix}_{self.name} *", "data", set()),
            ], set(["__warn_result", "__nonull(1,2)"])),
            str(Template(unpack_template, searchList=[nameSpace, {'check':True}])).splitlines())
        
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_unpack_no_check", [
                (f"struct dpack_decoder *", "decoder", set()),
                (f"struct {self.prefix}_{self.name} *", "data", set())
            ], set(["__warn_result", "__nonull(1,2)"])),
            str(Template(unpack_template, searchList=[nameSpace, {'check':False}])).splitlines())

        check_template = '''\
$(assert)(data);
#for $t, $n in $struct
$(assert)(!$(prefix)_$(name)_check_$(n)(data->$n));
#end for

#if $must
int err;

#for $f in $must
err = $(f)(data);
if (err)
	return err;

#end for
#end if
return 0;
'''
        addFunction(node, 
            (f"int", f"{self.prefix}_{self.name}_check", [
                (f"struct {self.prefix}_{self.name} *", "data", set()),
            ], set(["__warn_result", "__nonull(1)"])),
            str(Template(check_template, searchList=[nameSpace])).splitlines())
