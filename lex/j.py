#!/usr/bin/python

#
#   Decode json dict data

import json

def process(text):
    try:
        d = json.loads(text)
    except ValueError:
        return {}

    # need strings for all fields, as we are doing string processing
    for key in d.keys():
        v = d[key]
        if type(v) != type(""):
            d[key] = str(v)

    # get the format string
    fmt = []
    keys = d.keys()
    keys.sort()
    for key in keys:
        fmt.append('%%(%s)s' % key)
    d['fmt'] = ' '.join(fmt)

    d['whole'] = text

    return d

# FIN
