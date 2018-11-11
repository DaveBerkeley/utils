
import socket
from dateutil import parser as date_parser

#
#

def dequote(text):
    quote = text[0]
    if quote in [ "'", '"' ]:
        if text[-1] == quote:
            return text[1:-1]
    return text

#
#

def is_ip(ip):
    try:
        socket.inet_pton(socket.AF_INET, ip)
        return True
    except socket.error:
        pass

    try:
        socket.inet_pton(socket.AF_INET6, ip)
        return True
    except socket.error:
        pass

    return False
    
#
#

def process(line):
    
    parts = line.split(' ')

    if is_ip(parts[0]):
        # first field is ip (not server)
        parts.insert(0, '') # dummy server

    #print parts
    server, ip, x, y, d1, d2 = parts[:6]
    #print parts

    dd = " ".join([d1, d2])
    dd = dd.replace(':', ' ',1)
    dd = dd[1:-1] # remove [ and ]

    try:
        dt = date_parser.parse(dd)
    except Exception as ex:
        log(str(ex), `line`)
        raise
    ymd = dt.strftime("%Y-%m-%d")
    hms = dt.strftime("%H:%M:%S")

    assert parts[6][0] == '"'
    cmd = parts[6][1:]
    if cmd in [ '-"', '\\n"' ]:
        # no GET command, fake one
        cmd = 'None'
        parts.insert(6, cmd)
        parts.insert(7, '?')
        parts.insert(8, 'HTTP/1.x')
    assert cmd in [ 'None', 'GET', 'HEAD', 'OPTIONS', 'POST', 'PROPFIND' ], (cmd, `line`)
    assert parts[8].startswith('HTTP/1.'), `line`

    d = {
        'whole' : line,
        'server' : server,
        'ip' : ip,
        'ymd' : ymd,
        'hms' : hms,
        'dt' : ymd + " " + hms,
        'cmd' : cmd,
        'path' : parts[7], # eg '/static/style.css'
        'err' : parts[9],
        'bytes' : parts[10],
        'referrer' : dequote(parts[11]),
        'agent' : dequote(" ".join(parts[12:])),
    }
    return d

# FIN
