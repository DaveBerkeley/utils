#
#   Syslog

import re
import datetime

import dt

r_prog = re.compile("([\w/]+)(?:\[(\d+)\])?")

parser = dt.Parser()

def process(text):
    # syslog format

    d, i = parser.parse(text)

    d['type'] = 'syslog'

    head, tail = text.split(": ", 1)
    parts = head.split()
    parts = parts[i:]

    d['whole'] = d['dt'] + " " + " ".join(parts) + ": " + tail

    def progpid(word):
        match = r_prog.match(word)
        assert match
        return match.groups()

    d['host'] = parts[0]

    prog, pid = progpid(parts[1])
    d['prog'] = prog
    d['pid'] = pid

    d['msg' ] = tail

    if pid:
        d['fmt'] += " %(host)s %(prog)s[%(pid)s]: %(msg)s"
    else:
        d['fmt'] += " %(host)s %(prog)s: %(msg)s"

    return d

# FIN
