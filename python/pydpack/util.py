
def toid(name):
     return name.replace('-','_').replace('.','_')

def todef(name):
    return toid(name).upper()

def newline(l):
    if l[-1] != '':
        l.append("")
