#!/usr/bin/python

import re
import datetime

#
#

re_hms = "(\d\d):(\d\d):(\d\d)"
re_m = "^([A-Z][a-z][a-z])$"
re_d = "^(\d+)$"
re_y = "^(\d\d\d\d)$"
re_ymd = "(\d\d\d\d)[/-](\d\d)[/-](\d\d)"
re_z = "([+-]\d\d:\d\d)"
re_us = "\.(\d\d\d\d\d\d)"
re_ms = "\.(\d\d\d)"

res = {
    "hms"   : [ re_hms, ],
    "YMD"   : [ re_ymd, ],
    "M" : [ re_m, ],
    "D"   : [ re_d, ],
    "Y"  : [ re_y, ],
    "zone"  : [ re_z, ],
    "us"    : [ re_us, ],
    "ms"    : [ re_ms, ],
}

priority = [ 
    [ "YMD", "M", ],
    [ "YMD", "D", ],
    [ "hms", ],
    [ "us", "ms", ],
    [ "zone", ],
]

def analyze(parts):
    found = {}
    for i, part in enumerate(parts):
        seen = False
        for section, (regex, ) in res.items():
            # check for whole match
            r = re.compile("^" + regex + "$")
            match = r.match(part)
            if match:
                seen = True
                found[section] = i, section, match.span(), len(match.groups())
                continue
            if regex[0] == "^":
                continue
            # check for part match
            r = re.compile(regex)
            match = r.search(part)
            if match:
                seen = True
                found[section] = i, section, match.span(), len(match.groups())
        if not seen:
            # must find date/time value in first parts of the line
            break

    # remove duplicated (worse) matches
    best = {}
    for priorities in priority:
        for p in priorities:
            if p in found:
                best[p] = found[p]
                break
    return best

#
#

class Field:

    def __init__(self):
        self.fns = {}
        self.offset = 0
        self.match = None

    def set_regex(self, regex):
        self.regex = re.compile(regex)

    def add(self, key):
        self.fns[key] = self.offset
        self.offset += 1

    def parse(self, text):
        #print "parse", text
        self._match = self.regex.search(text)

    def make_fn(self, key):
        idx = self.fns.get(key)
        if idx is None:
            return None
        def fn():
            v = self._match.groups()[idx]
            #print "call", key, v, idx, self._match.groups()
            return v
        return fn

#
#

class Decode:

    def __init__(self):
        self.fields = {}

    def add(self, idx, field):
        self.fields[idx] = field

    def find_fn(self, key):
        for idx, field in self.fields.items():
            fn = field.make_fn(key)
            if fn:
                return fn
        return None

    def resolve(self):
        handlers = {}

        # add backstop handler for 'Y'
        def Y():
            # assume this year, in absense of any other info
            now = datetime.datetime.now()
            return now.year
        handlers['Y'] = Y

        must_have = "YMDhms"
        for key in must_have:
            fn = self.find_fn(key)
            if fn:
                handlers[key] = fn
        # assert must haves
        for key in must_have:
            assert handlers.get(key), key

        fns = {}

        # build composite ymd/hms handlers
        fn_Y = handlers['Y']
        fn_M = handlers['M']
        fn_D = handlers['D']
        def ymd():
            return fn_Y(), fn_M(), fn_D()
        fns['ymd'] = ymd

        fn_h = handlers['h']
        fn_m = handlers['m']
        fn_s = handlers['s']
        def hms():
            return fn_h(), fn_m(), fn_s()
        fns['hms'] = hms

        # add may haves
        may_haves = [ "zone", "ms", "us" ]
        for key in may_haves:
            fn = self.find_fn(key)
            if fn:
                fns[key] = fn

        self.handlers = fns

    def parse(self, parts):
        d = {}
        for i, part in enumerate(parts):
            field = self.fields.get(i)
            if field is None:
                break
            field.parse(part)

        for key, fn in self.handlers.items():
            print key, fn()
        return d, i

#
#

def make_regex(col):
    #print col
    field = Field()
    regex = ""
    size = 0
    for start, end, section, num in col:
        #print size, start, end, section, num
        next_regex = res[section][0]
        # force "^" match if start of field
        if (start == 0) and (next_regex[0] != "^"):
            regex += "^"
        # pad with "." if there is a hole in the match
        if size < start:
            regex += "." * (start - size)
        if size > start:
            raise Exception("xx")
        # concat the regex
        regex += next_regex
        size = end

        # set the handler functions for each field
        if section in [ "YMD", "hms" ]:
            assert num == 3
            for c in section:
                field.add(c)
        else:
            assert num == 1
            field.add(section)

    field.set_regex(regex)
    return field

#
#

def construct(found):
    # build a class that returns ymd, hms, dt, fmt and offset to remainder
    max_idx = -1
    cols = {}
    for idx, section, (start, end), num in found.values():
        #print idx, section, start, end, num
        col = cols.get(idx, [])
        col.append((start, end, section, num))
        col.sort()
        cols[idx] = col
        if idx > max_idx:
            max_idx = idx

    decode = Decode()
    for idx, col in cols.items():
        field = make_regex(col)
        decode.add(idx, field)

    return decode

#
#

if __name__ == "__main__":

    tests = [
        "Nov 10 07:33:31 anything",
        "2018/11/10 00:08:34.123456 klatu postfix",
        "2018-10-18 07:32:47.266078 abcdef asdert:",
        "2018-05-23T10:08:09.202422+00:00 host prog:",
    ]

    for line in tests:
        parts = line.split()
        print "*" * 20
        print parts
        found = analyze(parts)
        decode = construct(found)
        decode.resolve()

        decode.parse(parts)

# FIN
