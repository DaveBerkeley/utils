
def process(line, **kwaargs):

    parts = line.split()
    d = {}
    d['whole'] = line
    for i, part in enumerate(parts):
        d[str(i)] = part
    return d

# FIN
