
def toid(name):
     return name.replace('-','_').replace('.','_')

def todef(name):
    return toid(name).upper()

def newline(l):
    if l[-1] != '':
        l.append("")

def addIncludes(node, file: str):
    if not file:
        return
    node.top.dpack_module.includes.add(file)

def addDefine(node, name: str, data: list[str]):
    node.top.dpack_module.defines.append((name, data))

def addTypeDef(node, type: str, name: str):
    node.top.dpack_module.typedefs.append((type, name))

def addInlineFunction(node, declaration: tuple[str, str, list[tuple[str, str]]], content: list[str]):
    node.top.dpack_module.InlineFunctions.append((declaration, content))

def addFunction(node, declaration: tuple[str, str, list[tuple[str, str]]], content: list[str]):
    node.top.dpack_module.functions.append((declaration, content))

def addStructure(node, name: str, content: list[tuple[str, str]]):
    node.top.dpack_module.structures.append((name, content))

def getAssertFunction(node):
    return node.top.dpack_module.assertFunction

def defAlign(align: int, name: str, data: list[str]) -> list[str]:
    a = ""
    if len(data) == 0:
        return [f"#define {name}"]
    inline  = f"#define {name}"
    if data[0][0] == "(":
        align -= 1
        a = " "
    if len(inline) > align:
        raise Exception(f"bad align value must be > {len(inline)}")
    inline += " " * (align - len(inline))
    inline += " ".join(data)
    if len(inline) < 80:
        return [inline]
    ret = [f"#define {name} \\"]
    if len(data) == 1:
        ret.append(f"\t{data[0]}")
    else:
        data_cp = data[:]
        f = data_cp.pop(0)
        l = data_cp.pop()
        ret.append(f"\t{f} \\")
        for i in data_cp:
            ret.append(f"\t{a}{i} \\")
        ret.append(f"\t{a}{l}")
    return ret

def functionAlign(function: tuple[str, str, list[tuple[str, str]]], endline = "") -> list[str]:
    _ , f, args = function
    inline  = f"{f}("
    if len(args) > 0:
        inline += ", ".join([t + " " + n if t[-1] != "*" else t + n for t, n in args])
    else:
        inline += "void"
    inline += ")" + endline
    if len(inline) < 80:
        return [inline]
    if len(args) == 0:
         return [inline]
    ret = []
    l = f"{f}("
    a = max([len(x[0]) for x in args]) + 2
    args_cp = args[:]
    ft, fv = args_cp.pop(0)
    lt, lv = args_cp.pop()
    if ft[-1] == "*":
        ft, fv = ft[:-1], "*" + fv
    ret = [l + ft + " " * (a - len(ft)) + fv + ","]
    for t, v in args_cp:
        if t[-1] == "*":
            t, v = t[:-1], "*" + v
        ret.append(" " * len(l) + t + " " * (a - len(t)) + v + ",")
    if lt[-1] == "*":
        lt, lv = lt[:-1], "*" + lv
    ret.append(" " * len(l) + lt + " " * (a - len(lt)) + lv + ")" + endline)
    return ret

def addBracket(l: list[str]):
    ret = []
    if len(l) == 0:
        return []
    elif len(l) == 1:
        return [f"({l[0]})"]
    else:
        ret.append(f"({l.pop(0)}")
        ret.extend(l)
        ret[-1] += ")"
        return ret
