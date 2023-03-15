from pyang.error import err_add

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

def addInlineFunction(node, declaration: tuple[str, str, list[tuple[str, str, set[str]]], set[str]], content: list[str]):
    node.top.dpack_module.InlineFunctions.append((declaration, content))

def addFunction(node, declaration: tuple[str, str, list[tuple[str, str, set[str]]], set[str]], content: list[str]):
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

def functionAlign(function: tuple[str, str, list[tuple[str, str, set[str]]], set[str]], endline = "") -> list[str]:
    _ , f, args, _ = function
    inline  = f"{f}("
    if len(args) > 0:
        l = []
        for t, n, attrs in args:
            attr = " " + " ".join(attrs) if len(attrs) > 0 else ""
            t, s = removeStart(t)
            l.append(t + " " + "*" * s + n + attr)
        inline += ", ".join(l)
    else:
        inline += "void"
    inline += ")"
    if len(inline + endline) < 80:
        return [inline + endline]
    if len(args) == 0 or (len(args) == 1 and len(endline) == 0):
        return [inline + endline]
    if len(args) == 1:
        return [inline, "\t" + endline]
    ret = []
    l = f"{f}("
    a = findAlign([x[0] for x in args])
    args_cp = args[:]
    ft, fv, fattrs = args_cp.pop(0)
    fattr = " " + " ".join(fattrs) if len(fattrs) > 0 else ""
    lt, lv, lattrs = args_cp.pop()
    lattr = " " + " ".join(lattrs) if len(lattrs) > 0 else ""
    ft, s = removeStart(ft)
    ret = [l + ft + " " * (a - len(ft) - s) + "*" * s + fv + fattr + ","]
    for t, v, attrs in args_cp:
        attr = " " + " ".join(attrs) if len(attrs) > 0 else ""
        t, s = removeStart(t)
        ret.append(" " * len(l) + t + " " * (a - len(t) - s) + "*" * s + v + attr + ",")
    lt, s = removeStart(lt)
    if len(endline) > 1:
        ret.append(" " * len(l) + lt + " " * (a - len(lt) - s) + "*" * s + lv + lattr + ")")
        ret.append("\t" + endline)
    else:
        ret.append(" " * len(l) + lt + " " * (a - len(lt) - s) + "*" * s + lv + lattr + ")" + endline)
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

def range2liststr(ranges: list, type: str) -> list[str]:
    t = []
    c = []
    for mi, ma in ranges:
        if mi == 'min':
            mi = f"{type}_MIN"
            mih = mi
        elif mi == 'max':
            mi = f"{type}_MAX"
            mih = mi
        else:
            mih = hex(mi)
        if ma == 'max':
            ma = f"{type}_MAX"
            mah = ma
        elif ma:
            mah = hex(ma)
        if ma:
            t.append(f"case {mi} ... {ma}:")
            c.append((f"{mih}", f"{mah}"))
        else:
            t.append(f"case {mi}:")
            c.append((f"{mih}", ""))
    pad0 = max([len(x) for x in t]) + 1
    pad1 = max([len(x) for x, _ in c])
    pad2 = max([len(x) for _, x in c])
    for i in range(len(t)):
        if c[i][1] != "":
            comment = "/* " + " " * (pad1 - len(c[i][0])) + c[i][0] + " ... " + " " * (pad2 - len(c[i][1])) + c[i][1] + " */"
        elif pad2 == 0:
            comment = "/* " + " " * (pad1 - len(c[i][0])) + c[i][0] + " */"
        else:
            comment = "/* " + " " * (pad1 - len(c[i][0])) + c[i][0] + "     " + " " * pad2 + " */"
        t[i] += " " * (pad0 - len(t[i])) + comment
    return t

def parseMust(ctx, node):
    musts = node.search("must")
    if not musts:
        return None
    
    ret = []
    
    for must in musts:
        if (   must.i_xpath[0] != 'path_expr'
            or must.i_xpath[1][0] != 'function_call'
            or must.i_xpath[1][1] != 'checker'
            or len(must.i_xpath[1][2]) != 1
            or must.i_xpath[1][2][0][0] != 'path_expr'
            or must.i_xpath[1][2][0][1][0] != 'literal'):
            err_add(ctx.errors, node.pos,'BAD_VALUE', (must.arg, "checker('my_check')"))
            return None
        ret.append(must.i_xpath[1][2][0][1][1][1:-1])
    return ret