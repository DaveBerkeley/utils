
from lex import log

def process(text):
    # 'ssh klatu tail -f /tmp/cloud_graphs.log'

    if text.startswith("Error "):
        return None
    if text.startswith("connection failed"):
        return None

    dt, tt, line = text.split(' ', 2)

    if line.startswith("No fields for "):
        return None
    if line.startswith('Drop'):
        code, line = line.split(' ', 1)
    elif line.startswith('TX'):
        code, line = line.split(' ', 1)
    elif line.startswith('No change'):
        size = len('No change')
        code, line = line[:size], line[size:]
    elif " None " in line:
        return None
    else:
        log("Malformed line?", `line`)
        return None

    dev, line = line.split(' ', 1)

    d = {
        'type' : 'cloudgraph',
        'date' : dt,
        'time' : tt,
        'code' : code,
        'dev'  : dev,
        'msg'  : line,
        'whole' : text,
    }

    return d

# FIN
