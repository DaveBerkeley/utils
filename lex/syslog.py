#
#   Syslog

import re
import datetime

r_prog = re.compile("(\w+)(?:\[(\d+)\])?")

def process(text):
    # syslog format
    d = { 'whole' : text, 'type' : 'syslog' }
    head, tail = text.split(": ", 1)
    parts = head.split()

    def progpid(word):
        match = r_prog.match(word)
        assert match
        return match.groups()

    if len(parts) == 5:
        # "Day dd hh:mm:ss host prog[pid]:"
        month, day = parts[0:2]
        fmt = "%Y %b %d"
        year = datetime.datetime.now().year
        dt = datetime.datetime.strptime(" ".join([str(year), month, day]), fmt)

        ymd = dt.strftime("%Y-%m-%d")
        hms = parts[2]

        d['dt'] = ymd + "T" + hms
        d['ymd'] = ymd
        d['hms'] = hms
        d['host'] = parts[3]
        prog, pid = progpid(parts[4])
        d['prog'] = prog
        d['pid'] = pid
        d['msg' ] = tail
        if pid:
            d['fmt'] = "%(dt)s %(host)s %(prog)s[%(pid)s]: %(msg)s"
        else:
            d['fmt'] = "%(dt)s %(host)s %(prog)s: %(msg)s"
    else:
        raise Exception(("Todo",text))

    return d

# FIN
