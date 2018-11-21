#!/usr/bin/python

import re
import datetime

def log(*args):
    for arg in args:
        print arg,
    print

#
#

def validate_month(match=None, word=None):
    if word is None:
        if match is None:
            return None
        word = match.groups()[0]
    try:
        dt = datetime.datetime.strptime(word, "%b")
        return "%02d" % dt.month
    except ValueError:
        return None

#
#

re_hms = "(\d\d):(\d\d):(\d\d)"
re_m   = "^([A-Z][a-z][a-z])$"
re_d   = "^(\d+)$"
re_y   = "^(\d\d\d\d)$"
re_ymd = "(\d\d\d\d)[/-](\d\d)[/-](\d\d)"
re_z   = "([+-]\d\d:\d\d)"
re_us  = "(\.\d\d\d\d\d\d)"
re_ms  = "(\.\d\d\d)"

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

#
#

prefer = [ 
    [ "YMD", "M", ],
    [ "YMD", "D", ],
    [ "hms", ],
    [ "us", "ms", ],
    [ "zone", ],
]

#
#

def analyze(parts):
    found = {}
    def matched(match):
        return match
    for i, part in enumerate(parts):
        seen = False
        for section, (regex, validate) in res.items():
            # always use the first match
            if section in found:
                continue
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
    for priorities in prefer:
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
        #log("parse", text)
        self._match = self.regex.search(text)

    def make_fn(self, key, column):
        idx = self.fns.get(key)
        if idx is None:
            return None
        #log("make_fn", key, idx, column)
        valid = self.validate.get(key)
        def fn():
            v = self._match.groups()[idx]
            #print "call", key, v, idx, self._match.groups()
            if valid:
                v = valid(word=v)
            return v
        fn.column = column
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
            fn = field.make_fn(key, idx)
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
        fraction = None
        may_haves = [ "zone", "ms", "us" ]
        for key in may_haves:
            fn = self.find_fn(key)
            if not fn:
                continue
            # fraction _must_ be in the same column as 's'
            if fn.column != fn_s.column:
                continue
            fns[key] = fn
            if key in [ "ms", "us" ]:
                fraction = fn

        # standard "dt" generation
        def _dt():
            text = "%s %s" % (ymd(), hms())
            if fraction:
                text += fraction()
            return text
        fns['dt'] = _dt

        self.handlers = fns
        #log("handlers", fns)

    def parse(self, parts):
        d = {}
        for i, part in enumerate(parts):
            field = self.fields.get(i)
            if field is None:
                break
            field.parse(part)

        for key, fn in self.handlers.items():
            d[key] = fn()

        # this is all the format we can generate at this stage
        d['fmt'] = "%(dt)s"

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
        if size > start:
            continue # shouldn't get (or process) overlapping matches
        next_regex = res[section][0]
        # force "^" match if start of field
        if (start == 0) and (next_regex[0] != "^"):
            regex += "^"
        # pad with "." if there is a hole in the match
        if size < start:
            regex += "." * (start - size)
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

class Parser:

    def __init__(self):
        self.decode = None

    def parse(self, line):
        parts = line.split()

        if not self.decode:        
            found = analyze(parts)
            self.decode = construct(found)
            self.decode.resolve()

        d, i = self.decode.parse(parts)
        return d, i

#
#

if __name__ == "__main__":

    import sys

    if len(sys.argv) == 1:
        parser = Parser()
        for line in sys.stdin:
            line = line.strip()
            d, i = parser.parse(line)
            rest = line.split()[i:]
            print d['fmt'] % d, " ".join(rest)
        sys.exit(0)

    must_have = [ "ymd", "hms", "dt", "fmt" ]
    tests = [
        [ "2018/11/10 00:08:34.123456 klatu postfix", [ "us" ], ],
        [ "2018-10-18 07:32:47.266078 abcdef asdert:", [ "us" ], ],
        [ "2018-10-18 07:32:47.266 abcdef asdert:", [ "ms" ], ],
        [ "2018-10-18 07:32:47 abcdef asdert:", [], ],
        [ "2018-05-23T10:08:09.202422+00:00 host prog:", [ "us" ], ],
        [ "Nov 10 07:33:31 anything", [ ], ],
        [ "2018/08/12 06:33:04.224 37.49.224.116", [ "ms" ], ],
    ]

    for line, fields in tests:
        print "*" * 20
        print repr(line)

        parser = Parser()
        d, i = parser.parse(line)
        print d, i

        for key in must_have + fields:
            assert d.get(key), key

# FIN
