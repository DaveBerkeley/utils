#!/usr/bin/python

import re
import datetime

#
#

def validate_month(match=None, word=None):
    if word is None:
        if match is None:
            return None
        word = match.groups()[0]
    dt = datetime.datetime.strptime(word, "%b")
    return "%02d" % dt.month

re_hms = "(\d\d):(\d\d):(\d\d)"
re_m   = "^([A-Z][a-z][a-z])$"
re_d   = "^(\d+)$"
re_y   = "^(\d\d\d\d)$"
re_ymd = "(\d\d\d\d)[/-](\d\d)[/-](\d\d)"
re_z   = "([+-]\d\d:\d\d)"
re_us  = "\.(\d\d\d\d\d\d)"
re_ms  = "\.(\d\d\d)"

res = {
    "hms"   : [ re_hms, None, ],
    "YMD"   : [ re_ymd, None, ],
    "M"     : [ re_m, validate_month, ],
    "D"     : [ re_d, None, ],
    "Y"     : [ re_y, None, ],
    "zone"  : [ re_z, None, ],
    "us"    : [ re_us, None, ],
    "ms"    : [ re_ms, None, ],
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
    def matched(match):
        return match
    for i, part in enumerate(parts):
        seen = False
        for section, (regex, validate) in res.items():
            validate_fn = validate or matched
            # check for whole match
            r = re.compile("^" + regex + "$")
            match = r.match(part)
            if not validate_fn(match) is None:
                seen = True
                found[section] = i, section, match.span(), len(match.groups()), validate
                continue
            if regex[0] == "^":
                continue
            # check for part match
            r = re.compile(regex)
            match = r.search(part)
            if not validate_fn(match) is None:
                seen = True
                found[section] = i, section, match.span(), len(match.groups()), validate
        if not seen:
            # must find date/time values in first parts of the line
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
        self.validate = {}
        self.offset = 0
        self.match = None

    def set_regex(self, regex):
        self.regex = re.compile(regex)

    def add(self, key, validate):
        self.fns[key] = self.offset
        self.validate[key] = validate
        self.offset += 1

    def parse(self, text):
        #print "parse", text
        self._match = self.regex.search(text)

    def make_fn(self, key):
        idx = self.fns.get(key)
        if idx is None:
            return None
        valid = self.validate.get(key)
        def fn():
            v = self._match.groups()[idx]
            #print "call", key, v, idx, self._match.groups()
            if valid:
                v = valid(word=v)
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
            return "%04d" % now.year
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
            return fn_Y() + "/" + fn_M() + "/" + fn_D()
        fns['ymd'] = ymd

        fn_h = handlers['h']
        fn_m = handlers['m']
        fn_s = handlers['s']
        def hms():
            return fn_h() + ":" + fn_m() + ":" + fn_s()
        fns['hms'] = hms

        # add may haves
        may_haves = [ "zone", "ms", "us" ]
        for key in may_haves:
            fn = self.find_fn(key)
            if fn:
                fns[key] = fn

        # TODO : add "dt" parser
        # TODO : convert Jan month to 01
        # TODO : turn ms into us ?

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
    for start, end, section, num, validate in col:
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
                field.add(c, validate)
        else:
            assert num == 1
            field.add(section, validate)

    field.set_regex(regex)
    return field

#
#

def construct(found):
    # build a class that returns ymd, hms, dt, fmt and offset to remainder
    max_idx = -1
    cols = {}
    for idx, section, (start, end), num, validate in found.values():
        #print idx, section, start, end, num
        col = cols.get(idx, [])
        col.append((start, end, section, num, validate))
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
        "2018/11/10 00:08:34.123456 klatu postfix",
        "2018-10-18 07:32:47.266078 abcdef asdert:",
        "2018-05-23T10:08:09.202422+00:00 host prog:",
        "Nov 10 07:33:31 anything",
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
