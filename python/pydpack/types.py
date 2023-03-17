from pyang.error import err_add
from pyang import util
from pyang.statements import search_typedef
from pydpack.util import *
from Cheetah.Template import Template

class BaseType(object):
    def __init__(self, ctx, node, _type, _include, _encode, _decode, _min, _max) -> None:
        self.node = node
        self.type = _type
        self.include = _include
        self.encode = _encode
        self.decode = _decode
        self.min = _min
        self.max = _max
        addIncludes(node, self.include)
        self.nameSpace = {
            'encode': self.encode,
            'decode': self.decode,
            'check':ctx.opts.dpack_no_check,
        }

        self.getter_attrs = {
            "function": set(["__warn_result", "__nonull(1,2)"])
        }
        self.getter_template = '''\
$(assert)(obj);
$(assert)(result);

if (!$(struct_type)_has_$(name)(obj))
	return -EPERM;

*result = obj->$name;
return 0;
'''
        self.setter_attrs = {
            "function": set(["__nonull(1)"])
        }
        self.setter_template = '''\
$(assert)(obj);
$(assert)(!$(struct_type)_check_$(name)(value));

obj->$name = value;
obj->filled |= (1U << $field);
'''
        self.nameSpace['must'] = parseMust(ctx, self.node)
        if self.nameSpace['must']:
            self.check_attrs = {
                "function": set(["__warn_result"]),
                "value": set(),
            }
        else:
            self.check_attrs = {
                "function": set(["__warn_result"]),
                "value": set(["__unused"]),
            }
        self.check_template = '''\
#if $must
int err;

#for $f in $must
err = $(f)(value, ctx);
if (err)
	return err;

#end for
#end if
return 0;
'''
        self.pack_attrs = {
            "function": set(["__warn_result"]),
        }
        self.pack_template = '''\
$(assert)(encoder);
$(assert)(data);

const struct $struct_type *obj = (struct $(struct_type) *)data;
int err;

err = $(encode)(encoder, obj->$name);
if (err)
	return err;

return 0;
'''

        self.unpack_attrs = {
            "function": set(["__warn_result"]),
        }
        self.unpack_template = '''\
$(assert)(decoder);
$(assert)(data);

int err;
struct $struct_type *obj = (struct $(struct_type) *)data;

err = $(decode)(decoder, &obj->$name);
if (err)
	return err;
#if $check

err = $(struct_type)_check_$(name)(obj->$name);
if (err)
	return err;
#end if

return 0;
'''

    def updateName(self, name: str) -> str:
        return name

    def getInclude(self) -> str:
        return self.include

    def getType(self) -> str:
        return self.type

    def getMin(self) -> str:
        return self.min

    def getMax(self) -> str:
        return self.max
    
    def hasFini(self) -> bool:
        return False
    
    def hasInit(self) -> bool:
        return False
    
    def addFunctions(self, name: str, struct_type: str) -> None:
        self.nameSpace["assert"]      = getAssertFunction(self.node)
        self.nameSpace["name"]        = name
        self.nameSpace["struct_type"] = struct_type
        self.nameSpace["field"]       = todef(f"{struct_type}_{name}_FLD")

        addExternFunction(self.node, ("int", f"{struct_type}_get_{name}", [
            (f"const struct {struct_type} *", "obj", self.getter_attrs.get("obj", set())),
            (f"{self.type} *", "result", self.getter_attrs.get("result", set())),
        ], self.getter_attrs.get("function", set())),
        str(Template(self.getter_template, searchList=[self.nameSpace])).splitlines())

        addExternFunction(self.node, ("void", f"{struct_type}_set_{name}", [
            (f"struct {struct_type} *", "obj", self.setter_attrs.get("obj", set())),
            (self.type, "value", self.setter_attrs.get("value", set())),
        ], self.setter_attrs.get("function", set())),
        str(Template(self.setter_template, searchList=[self.nameSpace])).splitlines())

        addExternFunction(self.node, ("int", f"{struct_type}_check_{name}", [
            ("const " + self.type, "value", self.check_attrs.get("value", set())),
        ], self.check_attrs.get("function", set())),
        str(Template(self.check_template, searchList=[self.nameSpace])).splitlines())

        addStaticFunction(self.node, ("int", f"{struct_type}_pack_{name}", [
            ('struct dpack_encoder *', "encoder", self.pack_attrs.get("encoder", set())),
            ('const void *', "data", self.pack_attrs.get("data", set())),
        ], self.pack_attrs.get("function", set())),
        str(Template(self.pack_template, searchList=[self.nameSpace])).splitlines())

        addStaticFunction(self.node, ("int", f"{struct_type}_unpack_{name}", [
            ('struct dpack_decoder *', "decoder", self.unpack_attrs.get("decoder", set())),
            ('void *', "data", self.unpack_attrs.get("data", set())),
        ], self.unpack_attrs.get("function", set())),
        str(Template(self.unpack_template, searchList=[self.nameSpace])).splitlines())

        addExternFunction(self.node,
            ('bool', f"{struct_type}_has_{name}", [
                (f"const struct {struct_type} *", "data", set()),
            ], set(["__warn_result"])),
            [f"return data->filled & (1U << {self.nameSpace['field']});"])

class scalarType(BaseType):
    unit2c = {
        'uint8':  'UCHAR',
        'uint16': 'USHRT',
        'uint32': 'UINT',
        'uint64': 'ULLONG',
        'int8':   'CHAR',
        'int16':  'SHRT',
        'int32':  'INT',
        'int64':  'LLONG',
    }
    def __init__(self, unit, ctx, node) -> None:
        _type   = f"{unit}_t"
        _encode = f"dpack_encode_{unit}"
        _decode = f"dpack_decode_{unit}"
        _min    = f"DPACK_{todef(unit)}_SIZE_MIN"
        _max    = f"DPACK_{todef(unit)}_SIZE_MAX"
        super().__init__(ctx, node, _type, 'dpack/scalar.h', _encode, _decode, _min, _max)

        self.nameSpace["ranges"] = \
            getattr(self.node.search_one("type").i_type_spec, 'ranges', [])
        if len(self.nameSpace["ranges"]) > 0:
            self.nameSpace["c_type"] = scalarType.unit2c[unit]
            if "__unused" in self.check_attrs["value"]:
                self.check_attrs["value"].remove("__unused")
            self.check_template = '''\
#from pydpack.util import range2liststr
#if $must
int err;

#end if
#set $t = "\\n".join(range2liststr($ranges, $c_type))
switch (value) {
default:
	return -ERANGE;
$t
	break;
}

#if $must
#for $f in $must
err = $(f)(value, ctx);
if (err)
	return err;

#end for
#end if
return 0;
'''

class bitmapType(BaseType):
    def __init__(self, ctx, node) -> None:
        super().__init__(ctx,
                         node,
                        'uint64_t',
                        'dpack/scalar.h',
                        'dpack_encode_uint64',
                        'dpack_decode_uint64',
                        'DPACK_UINT64_SIZE_MIN',
                        'DPACK_UINT64_SIZE_MAX')

    def getDecode(self, ctx_name: str, data_name: str) -> str:
        t = self.node.search_one("type").i_type_spec
        v_max = 0
        for _, p in t.bits:
            v_max |= 1 << p
        return f"{self.decode}_max({ctx_name}, {hex(v_max)}, {data_name})"

class boolType(BaseType):
    def __init__(self, ctx, node) -> None:
        super().__init__(ctx,
                         node,
                        'bool',
                        'dpack/scalar.h',
                        'dpack_encode_bool',
                        'dpack_decode_bool',
                        'DPACK_BOOL_SIZE',
                        'DPACK_BOOL_SIZE')
class stringType(BaseType):
    def __init__(self, ctx, node) -> None:
        super().__init__(ctx,
                         node,
                        'char *',
                        'dpack/string.h',
                        'dpack_encode_str',
                        'dpack_decode_strdup',
                        '(1)',
                        '(DPACK_STRLEN_MAX + 5)')

    def hasFini(self):
        return True
    
    def addFunctions(self, name, struct_type):
        super().addFunctions(name, struct_type)
        addStaticFunction(self.node, ("void", f"{struct_type}_fini_{name}", [
            (self.type, "value", set()),
        ], set()),
        str(Template('''\
free(value);
''', searchList=[self.nameSpace])).splitlines())

class ExternTypedef(BaseType):
    def __init__(self, ctx, node, name, parent_node) -> None:
        self.ctx = ctx
        self.node = node
        _type =     toid(name)
        _include =  f"{node.top.arg}.h"
        _encode =   toid(f"{node.top.arg}_encode_{name}")
        _decode =   toid(f"{node.top.arg}_decode_{name}")
        _min =      todef(f"{node.top.arg}_{name}_SIZE_MIN")
        _max =      todef(f"{node.top.arg}_{name}_SIZE_MAX")
        self.copy = toid(f"{node.top.arg}_copy_{name}")

        c_include = node.search_one(('pydpack-extension', 'c-include'))
        c_type    = node.search_one(('pydpack-extension', 'c-type'))
        c_pack    = node.search_one(('pydpack-extension', 'c-pack'))
        c_unpack  = node.search_one(('pydpack-extension', 'c-unpack'))
        c_min     = node.search_one(('pydpack-extension', 'c-min'))
        c_max     = node.search_one(('pydpack-extension', 'c-max'))
        c_copy    = node.search_one(('pydpack-extension', 'c-copy'))

        if c_include:
            _include = c_include.arg
        
        if c_type:
            _type = c_type.arg
        
        if c_pack:
            _encode = c_pack.arg

        if c_unpack:
            _decode = c_unpack.arg

        if c_min:
            _min = c_min.arg

        if c_max:
            _max = c_max.arg

        if c_copy:
            self.copy = c_copy.arg

        super().__init__(ctx, parent_node, _type, _include, _encode, _decode, _min, _max)

        self.nameSpace["copy_callback"] = self.copy
        self.getter_template = '''\
$(assert)(obj);
$(assert)(result);

return $(copy_callback)(result, obj->$name);
'''
        self.setter_template = '''\
$(assert)(obj);

return $(copy_callback)(&obj->$name, result);
'''

def dpack_Typedef_maker(ctx, node, name) -> None:
    _type =   toid(f"{node.top.arg}_{name}")
    _encode = toid(f"{node.top.arg}_encode_{name}")
    _decode = toid(f"{node.top.arg}_decode_{name}")
    _min =    todef(f"{node.top.arg}_{name}_SIZE_MIN")
    _max =    todef(f"{node.top.arg}_{name}_SIZE_MAX")
    dpType = getType(ctx, node)
    addTypeDef(node, dpType.getType(), _type)
    addDefine(node, _min, [dpType.getMin()])
    addDefine(node, _max, [dpType.getMax()])

    value_pack = []
    value_pack.append(f"{getAssertFunction(node)}(encoder);")
    value_pack.append(f"{getAssertFunction(node)}(dpack_encoder_space_left(encoder) >=")
    value_pack.append(f"{' ' * (len(getAssertFunction(node)) + 1)}{_min});")
    newline(value_pack)
    value_pack.append(f"return {dpType.getEncode('encoder', 'value')};")

    value_unpack = []
    value_unpack.append(f"{getAssertFunction(node)}(decoder)")
    value_unpack.append(f"{getAssertFunction(node)}(value)")
    value_unpack.append(f"{getAssertFunction(node)}(dpack_decoder_data_left(decoder) >=")
    value_unpack.append(f"{' ' * (len(getAssertFunction(node)) + 1)}{_min});")
    newline(value_unpack)
    value_unpack.append(f"return {dpType.getDecode('decoder', 'value')};")

    addExternFunction(node, ("int", _encode, [
        ("struct dpack_encoder *", "encoder", set()),
        (_type, "value", set()),
    ], set()), value_pack)
    addExternFunction(node, ("int", _decode, [
        ("struct dpack_decoder *", "decoder", set()),
        (_type + " *", "value", set()),
    ], set()), value_unpack)

    return lambda ctx, node: BaseType(ctx, node, _type, None, _encode, _decode, _min, _max)

TYPE_MAP = {
    'boolean': boolType,
    'int8':    lambda ctx, node: scalarType('int8', ctx, node),
    'int16':   lambda ctx, node: scalarType('int16', ctx, node),
    'int32':   lambda ctx, node: scalarType('int32', ctx, node),
    'int64':   lambda ctx, node: scalarType('int64', ctx, node),
    'uint8':   lambda ctx, node: scalarType('uint8', ctx, node),
    'uint16':  lambda ctx, node: scalarType('uint16', ctx, node),
    'uint32':  lambda ctx, node: scalarType('uint32', ctx, node),
    'uint64':  lambda ctx, node: scalarType('uint64', ctx, node),
    'bits':    bitmapType,
    'string':  stringType,
}

def dpack_BaseType_maker(c_type, c_include, c_pack, c_unpack, c_min, c_max):
    return lambda ctx, node: BaseType(ctx, node, c_type, c_include, c_pack, c_unpack, c_min, c_max)

def dpack_ExternTypedef_maker(ctx, t, name):
    return lambda _, node: ExternTypedef(ctx, t, name, node)

def getType(ctx, stmt):
    yangType = stmt.search_one('type').arg
    prefix, name = util.split_identifier(yangType)
    if prefix:
        modulename, revision = util.prefix_to_modulename_and_revision(stmt.top, prefix, stmt.pos, ctx.errors)
        map_name = f"{modulename}_{revision}_{name}"
    else:
        map_name = yangType

    if map_name not in TYPE_MAP:
        if prefix:
                module = ctx.get_module(modulename, revision)
        else:
                module = stmt
        t = search_typedef(module, name)
        if not t:
                err_add(ctx.errors, stmt.pos,'TYPE_NOT_FOUND', (yangType, "dpack types"))
                return

        if prefix:
            TYPE_MAP[map_name] = dpack_ExternTypedef_maker(ctx, t, name)    
        elif t.search_one(('pydpack-extension', 'py-module')):
            modname = t.search_one(('pydpack-extension', 'py-module')).arg
            pluginmod = __import__(modname)
            try:
                 TYPE_MAP[map_name] = pluginmod.dpack_BaseType_maker(ctx, t, name)
            except AttributeError as s:
                err_add(ctx.errors, stmt.pos,'MODULE_NOT_FOUND', modname)
                return
        else:
            c_type    = t.search_one(('pydpack-extension', 'c-type'))
            c_include = t.search_one(('pydpack-extension', 'c-include'))
            c_pack    = t.search_one(('pydpack-extension', 'c-pack'))
            c_unpack  = t.search_one(('pydpack-extension', 'c-unpack'))
            c_min     = t.search_one(('pydpack-extension', 'c-min'))
            c_max     = t.search_one(('pydpack-extension', 'c-max'))
            if c_include and c_type and c_pack and c_unpack and c_min and c_max:
                TYPE_MAP[map_name] = dpack_BaseType_maker(c_type.arg,
                                                          c_include.arg,
                                                          c_pack.arg,
                                                          c_unpack.arg,
                                                          c_min.arg,
                                                          c_max.arg)
            elif c_include or c_type or c_pack or c_unpack or c_min or c_max:
                if not c_include:
                    attr = 'c-include'
                elif not c_type:
                    attr = 'c-type'
                elif not c_pack:
                    attr = 'c-pack'
                elif not c_unpack:
                    attr = 'c-unpack'
                elif not c_min:
                    attr = 'c-min'
                elif not c_max:
                    attr = 'c-max'
                err_add(ctx.errors, t.pos,'MISSING_ARGUMENT_ATTRIBUTE', (attr, t))
                return
            else:
                TYPE_MAP[map_name] = dpack_Typedef_maker(ctx, t, name)
    return TYPE_MAP[map_name](ctx, stmt)

