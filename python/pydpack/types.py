from pyang.error import err_add
from pyang import util
from pyang.statements import search_typedef
from pydpack.util import *

class BaseType(object):
    def __init__(self, _type, _include, _encode, _decode, _min, _max) -> None:
        self.type = _type
        self.include = _include
        self.encode = _encode
        self.decode = _decode
        self.min = _min
        self.max = _max

    def updateName(self, name):
        return name

    def getInclude(self):
        return self.include

    def getType(self):
        return self.type

    def getEncode(self, ctx_name: str, data_name: str) -> str:
        return f"{self.encode}({ctx_name}, {data_name})"

    def getDecode(self, ctx_name: str, data_name: str) -> str:
        return f"{self.decode}({ctx_name}, {data_name})"

    def getMin(self):
        return [self.min]

    def getMax(self):
        return [self.max]
    
    def mustFree(self):
        return False
    
    def getFree(self, data_name: str) -> str:
        return ""

class stdintType(BaseType):
    def __init__(self, _type, _encode, _decode, _min, _max) -> None:
        super().__init__(_type, 'dpack/stdint.h', _encode, _decode, _min, _max)

class u8Type(stdintType):
    def __init__(self) -> None:
        super().__init__('uint8_t',
                         'dpack_encode_u8',
                         'dpack_decode_u8',
                         'DPACK_U8_SIZE_MIN',
                         'DPACK_U8_SIZE_MAX')
class u16Type(stdintType):
    def __init__(self) -> None:
        super().__init__('uint16_t',
                         'dpack_encode_u16',
                         'dpack_decode_u16',
                         'DPACK_U16_SIZE_MIN',
                         'DPACK_U16_SIZE_MAX')
class u32Type(stdintType):
    def __init__(self) -> None:
        super().__init__('uint32_t',
                         'dpack_encode_u32',
                         'dpack_decode_u32',
                         'DPACK_U32_SIZE_MIN',
                         'DPACK_U32_SIZE_MAX')
class u64Type(stdintType):
    def __init__(self) -> None:
        super().__init__('uint64_t',
                         'dpack_encode_u64',
                         'dpack_decode_u64',
                         'DPACK_U64_SIZE_MIN',
                         'DPACK_U64_SIZE_MAX')

class stringType(BaseType):
    def __init__(self) -> None:
        super().__init__('char *',
                         'dpack/string.h',
                         'dpack_encode_str',
                         'dpack_decode_strdup',
                         'DPACK_STRLEN_MAX',
                         'DPACK_STRLEN_MAX')
        
    def mustFree(self):
        return True
    
    def getFree(self, data_name: str) -> str:
        return f"free({data_name})"

class ExternTypedef(BaseType):
    def __init__(self, ctx, node, name) -> None:
        self.ctx = ctx
        self.node = node
        _type =    toid(name)
        _include = f"{node.top.arg}.h"
        _encode =  toid(f"{node.top.arg}_encode_{name}")
        _decode =  toid(f"{node.top.arg}_decode_{name}")
        _min =     todef(f"{node.top.arg}_{name}_SIZE_MIN")
        _max =     todef(f"{node.top.arg}_{name}_SIZE_MAX")

        c_include = node.search_one(('pydpack-extension', 'c-include'))
        c_type    = node.search_one(('pydpack-extension', 'c-type'))
        c_pack    = node.search_one(('pydpack-extension', 'c-pack'))
        c_unpack  = node.search_one(('pydpack-extension', 'c-unpack'))
        c_min     = node.search_one(('pydpack-extension', 'c-min'))
        c_max     = node.search_one(('pydpack-extension', 'c-max'))

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

        super().__init__(_type, _include, _encode, _decode, _min, _max)

class Typedef(BaseType):
    def __init__(self, ctx, node, name) -> None:
        self.ctx = ctx
        self.node = node
        self.prefix = node.top.arg
        self.name = toid(self.node.arg)
        _type =   toid(f"{node.top.arg}_{name}")
        _encode = toid(f"{node.top.arg}_encode_{name}")
        _decode = toid(f"{node.top.arg}_decode_{name}")
        _min =    todef(f"{node.top.arg}_{name}_SIZE_MIN")
        _max =    todef(f"{node.top.arg}_{name}_SIZE_MAX")
        super().__init__(_type, None, _encode, _decode, _min, _max)
        dpType = getType(ctx, node)
        addTypeDef(node, dpType.getType(), _type)
        addDefine(node, _min, dpType.getMin())
        addDefine(node, _max, dpType.getMax())

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
        value_unpack.append(f"{' ' * (len(getAssertFunction(node)) + 1)}{_max});")
        newline(value_unpack)
        value_unpack.append(f"return {dpType.getDecode('decoder', 'value')};")

        addFunction(node, ("int", _encode, [
            ("struct dpack_encoder *", "encoder"),
            (_type, "value"),
        ]), value_pack)
        addFunction(node, ("int", _decode, [
            ("struct dpack_decoder *", "decoder"),
            (_type + " *", "value"),
        ]), value_unpack)

TYPE_MAP = {
    'uint8':  u8Type(),
    'uint16': u16Type(),
    'uint32': u32Type(),
    'uint64': u64Type(),
    'bits':   u64Type(),
    'string': stringType(),
}

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
            TYPE_MAP[map_name] = ExternTypedef(ctx, t, name)    
        elif t.search_one(('pydpack-extension', 'py-module')):
            modname = t.search_one(('pydpack-extension', 'py-module')).arg
            pluginmod = __import__(modname)
            try:
                 TYPE_MAP[map_name] = pluginmod.dpack_BaseType_maker(ctx, t, name)
            except AttributeError as s:
                err_add(ctx.errors, stmt.pos,'MODULE_NOT_FOUND', modname)
                return
        else:
            c_include = t.search_one(('pydpack-extension', 'c-include'))
            c_type    = t.search_one(('pydpack-extension', 'c-type'))
            c_pack    = t.search_one(('pydpack-extension', 'c-pack'))
            c_unpack  = t.search_one(('pydpack-extension', 'c-unpack'))
            c_min     = t.search_one(('pydpack-extension', 'c-min'))
            c_max     = t.search_one(('pydpack-extension', 'c-max'))
            if c_include and c_type and c_pack and c_unpack and c_min and c_max:
                TYPE_MAP[map_name] = BaseType(c_type.arg,
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
                TYPE_MAP[map_name] = Typedef(ctx, t, name)
    addIncludes(stmt, TYPE_MAP[map_name].getInclude())
    return TYPE_MAP[map_name]

