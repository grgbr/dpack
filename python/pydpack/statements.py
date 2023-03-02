from pydpack.util import *

DPACK_INCLUDE_ARRAY  = 'dpack/array.h'
DPACK_INCLUDE_STDINT = 'dpack/stdint.h'
DPACK_INCLUDE_STRING = 'dpack/string.h'

TYPE_MAP = {
    'uint8':  ('uint8_t',  DPACK_INCLUDE_STDINT),
    'uint16': ('uint16_t', DPACK_INCLUDE_STDINT),
    'uint32': ('uint32_t', DPACK_INCLUDE_STDINT),
    'string': ('char *',   DPACK_INCLUDE_STRING),
}

TYPE_SIZE = {
    'uint8_t':  ('DPACK_U8_SIZE_MIN', 'DPACK_U8_SIZE_MAX'),
    'uint16_t': ('DPACK_U16_SIZE_MIN','DPACK_U16_SIZE_MAX'),
    'uint32_t': ('DPACK_U32_SIZE_MIN','DPACK_U32_SIZE_MAX'),
}

ARRAY_SIZE = {
    'uint8_t':  ('DPACK_ARRAY_U8_SIZE_MIN', 'DPACK_ARRAY_U8_SIZE_MAX'),
    'uint16_t': ('DPACK_ARRAY_U16_SIZE_MIN','DPACK_ARRAY_U16_SIZE_MAX'),
    'uint32_t': ('DPACK_ARRAY_U32_SIZE_MIN','DPACK_ARRAY_U32_SIZE_MAX'),
}

TYPE_FUNCTION = {
    'uint8_t':  ('dpack_encode_u8', 'dpack_decode_u8'),
    'uint16_t': ('dpack_encode_u16','dpack_decode_u16'),
    'uint32_t': ('dpack_encode_u32','dpack_decode_u32'),
}

ARRAY_FUNCTION = {
    'uint8_t':  ('dpack_array_encode_u8', 'dpack_array_decode_u8'),
    'uint16_t': ('dpack_array_encode_u16','dpack_array_decode_u16'),
    'uint32_t': ('dpack_array_encode_u32','dpack_array_decode_u32'),
}

CHILDREN_STRUCTURE_MASK = ['container', 'list']

class Module():

    def __init__(self, ctx, node):
        self.ctx = ctx
        self.node = node
        self.name = self.node.arg
        self.define = f"_{todef(self.name)}_H"
        self.prefix = self.node.i_prefix
        if not self.prefix:
            self.prefix = self.node.arg
        self.structure = []
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
        self.structure.append(Struct(self.prefix, self.ctx, node))

    def getIncludes(self):
        ret = set()
        for s in self.structure:
            ret.update(s.includes)
        return sorted(list(ret))

    def getH(self):
        l = []
        l.append(f"#ifndef {self.define}")
        l.append(f"#define {self.define}")
        newline(l)
        includes = self.getIncludes()
        if len(includes) > 0:
            for i in includes:
                l.append(f"#include <{i}>")
        newline(l)
        for s in self.structure:
            l.extend(s.getDefine())
        newline(l)
        for s in self.structure:
            l.extend(s.getStruct())
        newline(l)
        for s in self.structure:
            l.extend(s.getStructSize())
        newline(l)
        for s in self.structure:
            l.extend(s.getFunctionDeclarations())
        newline(l)
        l.append(f"#endif /* {self.define} */")
        return "\n".join(l) + '\n'
    
    def getC(self):
        na = f"{todef(self.prefix)}_ASSERT"
        fa = [ f"#define {na}(condition) \\",
                  "\tdpack_assert(condition)"]
        l = []
        l.append(f'#include "{self.name}.h"')
        l.append(f'#include <dpack/codec.h>')
        newline(l)
        l.extend(fa)
        newline(l)
        for s in self.structure:
            l.extend(s.getFunctions(na))
            newline(l)
        return "\n".join(l) + '\n'

class Struct(object):

    def __init__(self, prefix, ctx, node):
        self.ctx = ctx
        self.node = node
        self.prefix = prefix
        self.name = toid(self.node.arg)
        self.includes = set(['stdlib.h'])
        self.struct = []
        self.define = []
        for c in self.node.i_children:
            name = c.arg
            if c.keyword in ['leaf', 'leaf-list']:
                type, include = TYPE_MAP[c.search_one('type').arg]
                self.includes.add(include)
            elif c.keyword in ['container', 'list']:
                type = f"struct {self.prefix}_{c.arg}"
            else:
                raise BaseException(f"{c.pos} bad keyword {c.keyword}")
            max_value_name = None
            if c.keyword in ['leaf-list', 'list'] :
                max_value = c.search_one('max-elements')
                self.includes.add(DPACK_INCLUDE_ARRAY)
                if not max_value:
                    raise BaseException(f"{c.pos} missing 'max-elements' in {c.arg}")
                max_value = max_value.arg
                SP = todef(self.prefix)
                SN = todef(self.name)
                N  = todef(name)
                max_value_name = f"{SP}_{SN}_{N}_NR"
                self.define.append((max_value_name, max_value))

            self.struct.append((type, name, max_value_name))



    def getStruct(self):
        lines = []
        lines.append(f"struct {self.prefix}_{self.name} {{")
        type_len = max([len(t) for t, _, _ in self.struct]) + 1
        for type, name, max_value in self.struct:
            l  =  "\t"
            if len(type) < type_len:
                pad = type_len - len(type)
                if type[-1] == '*':
                    type = type[:-1] + " "  * pad + "*"
                else:
                    type += " " * pad

            l += f"{type}{name}"
            if max_value:
                l += f"[{max_value}]"
            l +=  ";"
            lines.append(l)
        lines.append("};")
        return lines
    
    def getDefine(self):
        lines = []
        for max_value_name, max_value in self.define:
            lines.append(f"#define {max_value_name} ({max_value}U)")
        return lines
    
    def getStructSize(self):
        size_min = []
        size_max = []
        SP = todef(self.prefix)
        SN = todef(self.name)
        size_min.append(f"#define {SP}_{SN}_PACKED_SIZE_MIN \\")
        size_max.append(f"#define {SP}_{SN}_PACKED_SIZE_MAX \\")
        p = '('
        for type, _, array in self.struct:
            if array:
                mi, ma = ARRAY_SIZE.get(type, ("???","???"))
                mi = f"{mi}({array})"
                ma = f"{ma}({array})"
            else:
                mi, ma = TYPE_SIZE.get(type, ("???","???"))
            size_min.append(f"\t{p}{mi} + \\")
            size_max.append(f"\t{p}{ma} + \\")
            p = ' '

        size_min[-1] = size_min[-1][:-4] + ")"
        size_max[-1] = size_max[-1][:-4] + ")"
        return size_min + [" "] + size_max
    
    def getFunctionDeclarations(self):
        lines = []
        lines.append("extern int")
        f = f"{self.prefix}_{self.name}_pack("
        c = "struct dpack_encoder"
        d = f"const struct {self.prefix}_{self.name}"
        p = max(len(c), len(d)) + 1
        c += ' ' * (p - len(c)) + '*encoder,'
        d += ' ' * (p - len(d)) + '*data);'
        lines.append(f"{f}{c}")
        lines.append(f"{' ' * len(f)}{d}")
        lines.append("")
        lines.append("extern int")
        f = f"{self.prefix}_{self.name}_unpack("
        c = "struct dpack_decoder"
        d = f"struct {self.prefix}_{self.name}"
        p = max(len(c), len(d)) + 1
        c += ' ' * (p - len(c)) + '*decoder,'
        d += ' ' * (p - len(d)) + '*data);'
        lines.append(f"{f}{c}")
        lines.append(f"{' ' * len(f)}{d}")
        lines.append("")
        lines.append(f"static inline struct {self.prefix}_{self.name} *")
        lines.append(f"{self.prefix}_{self.name}_alloc(void)")
        lines.append("{")
        lines.append(f"\treturn malloc(sizeof(struct {self.prefix}_{self.name}));")
        lines.append("}")
        lines.append("")
        lines.append(f"static inline void")
        lines.append(f"{self.prefix}_{self.name}_free(struct {self.prefix}_{self.name} *data)")
        lines.append("{")
        lines.append(f"\treturn free(data);")
        lines.append("}")
        lines.append("")
        return lines

    def getFunctions(self, na="assert"):
        pa = []
        pu = []

        SP = todef(self.prefix)
        SN = todef(self.name)

        pa.append('int')
        f = f"{self.prefix}_{self.name}_pack("
        c = "struct dpack_encoder"
        d = f"const struct {self.prefix}_{self.name}"
        p = max(len(c), len(d)) + 1
        c += ' ' * (p - len(c)) + '*encoder,'
        d += ' ' * (p - len(d)) + '*data)'
        pa.append(f"{f}{c}")
        pa.append(f"{' ' * len(f)}{d}")

        pu.append('int')
        f = f"{self.prefix}_{self.name}_unpack("
        c = "struct dpack_decoder"
        d = f"struct {self.prefix}_{self.name}"
        p = max(len(c), len(d)) + 1
        c += ' ' * (p - len(c)) + '*decoder,'
        d += ' ' * (p - len(d)) + '*data)'
        pu.append(f"{f}{c}")
        pu.append(f"{' ' * len(f)}{d}")

        for p, v in [(pa, 'encoder'), (pu, 'decoder')]:
            p.append("{")
            p.append(f"\t{na}({v});")
            p.append(f"\t{na}(data);")
    
        pa.append(f"\t{na}(dpack_encoder_space_left(encoder) >=")
        pa.append(f"\t{' ' * (len(na) + 1)}{SP}_{SN}_PACKED_SIZE_MIN);")
        pu.append(f"\t{na}(dpack_decoder_data_left(decoder) >=")
        pu.append(f"\t{' ' * (len(na) + 1)}{SP}_{SN}_PACKED_SIZE_MIN);")

        for p in [pa, pu]:
            p.append("")
            p.append("\tint err;")
            p.append("")

        for type, name, array in self.struct:
            if array:
                fe, fd = ARRAY_FUNCTION.get(type, ("???","???"))
                pa.append(f"\terr = {fe}(encoder,")
                pa.append(f"\t{' ' * (len(fe) + 7)}data->{name},")
                pa.append(f"\t{' ' * (len(fe) + 7)}dpack_array_nr(data->{name}));")
                pu.append(f"\terr = {fd}(decoder,")
                pu.append(f"\t{' ' * (len(fd) + 7)}data->{name},")
                pu.append(f"\t{' ' * (len(fd) + 7)}dpack_array_nr(data->{name}));")
            else:
                fe, fd = TYPE_FUNCTION.get(type, ("???","???"))
                pa.append(f"\terr = {fe}(encoder, data->{name});")
                pu.append(f"\terr = {fd}(decoder, &data->{name});")
            for p in [pa, pu]:
                p.append("\tif (err)")
                p.append("\t\treturn err;")
                p.append("")

        
        for p in [pa, pu]:
            p.append("\treturn 0;")
            p.append("}")

        return pa + [""] + pu
