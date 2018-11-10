#!/usr/bin/python

import re
import datetime

#    if len(parts) == 5:
#        # "Day dd hh:mm:ss host prog[pid]:"
#        month, day = parts[0:2]
#        fmt = "%Y %b %d"
#        year = datetime.datetime.now().year
#        dt = datetime.datetime.strptime(" ".join([str(year), month, day]), fmt)
#
#        ymd = dt.strftime("%Y-%m-%d")
#        hms = parts[2]
#
#        d['dt'] = ymd + "T" + hms
#        d['ymd'] = ymd
#        d['hms'] = hms

#this_year = datetime.datetime.now().year

re_hms = "(\d\d):(\d\d):(\d\d)"
re_m = "^([A-Z][a-z][a-z])$"
re_d = "^(\d+)$"
re_ymd = "(\d\d\d\d)[/-](\d\d)[/-](\d\d)"
re_z = "([+-]\d\d:\d\d)"
re_us = "\.(\d\d\d\d\d\d)"
re_ms = "\.(\d\d\d)"

res = [
    [   re_hms, "hms", ],
    [   re_ymd, "ymd", ],
    [   re_m, "Mon", ],
    [   re_d, "dd", ],
    [   re_z, "zone", ],
    [   re_us, "us", ],
    [   re_ms, "ms", ],
]

if __name__ == "__main__":

    tests = [
        "Nov 10 07:33:31 anything",
        "2018/11/10 00:08:34.123456 klatu postfix",
        "2018-10-18 07:32:47.266078 abcdef asdert:",
        "2018-05-23T10:08:09.202422+00:00 host prog:",
    ]

    for line in tests:
        parts = line.split()
        print parts
        for i, part in enumerate(parts):
            for regex, x in res:
                #print i, part, regex
                # check for whole match
                r = re.compile("^" + regex + "$")
                match = r.match(part)
                if match:
                    print "whole ", i, part, x
                    continue
                if regex[0] == "^":
                    continue
                r = re.compile(regex)
                match = r.search(part)
                if match:
                    print "offset", i, match.span(), part, x


# FIN
