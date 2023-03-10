
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

def removeStart(data: str) -> tuple[str, int]:
    c = 0
    data = str.strip(data)
    while data[-1] == "*":
        c += 1
        data = str.strip(data[:-1])
    return data, c

def findAlign(data: list[str]) -> int:
    c = 0
    m = 0
    for x in data:
        d, t = removeStart(x)
        c = max([c, t])
        m = max([m, len(d)])
    return m + c + 1

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
        l = []
        for t, n in args:
            t, s = removeStart(t)
            l.append(t + " " + "*" * s + n)
        inline += ", ".join(l)
    else:
        inline += "void"
    inline += ")" + endline
    if len(inline) < 80:
        return [inline]
    if len(args) == 0:
         return [inline]
    ret = []
    l = f"{f}("
    a = findAlign([x[0] for x in args])
    args_cp = args[:]
    ft, fv = args_cp.pop(0)
    lt, lv = args_cp.pop()
    ft, s = removeStart(ft)
    ret = [l + ft + " " * (a - len(ft) - s) + "*" * s + fv + ","]
    for t, v in args_cp:
        t, s = removeStart(t)
        ret.append(" " * len(l) + t + " " * (a - len(t) - s) + "*" * s + v + ",")
    lt, s = removeStart(lt)
    ret.append(" " * len(l) + lt + " " * (a - len(lt) - s) + "*" * s + lv + ")" + endline)
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

def range2liststr(ranges: list, value: str) -> list[str]:
    t = []
    for mi, ma in ranges:
        ti = None
        ta = None
        if mi != 'min':
            ti = f"({value} >= {hex(mi)})"
        if ma != 'max':
            ta = f"({value} <= {hex(ma)})"
        if ti and ta:
            t.append(f"({ti} && {ta})")
        elif ti:
            t.append(ti)
        elif ta:
            t.append(ta)
    if len(t) == 1:
        return [t[0][1:-1]]
    for i in range(len(t) - 1):
        t[i] += " ||"
    return t