#!/usr/bin/python -u

import datetime
import argparse
import sys
import os
import re
import imp
import warnings

#
#

def log(*args):
    for arg in args:
        print >> sys.stderr, arg,
    print >> sys.stderr

#
#

class Collection:

    def __init__(self, *args):
        self.filters = list(args)

    def add(self, filt):
        self.filters.append(filt)

    def __repr__(self):
        text = ", ".join(map(str, self.filters))
        return "%s(%s)" % (self.__class__.__name__, text)

class And(Collection):
    def match(self, line):
        # must match all
        for filt in self.filters:
            if not filt.match(line):
                return False
        return True

class Or(Collection):
    def match(self, line):
        # must match any one
        for filt in self.filters:
            if filt.match(line):
                return True
        return False

class Base:
    def __init__(self, **kwargs):
        self.kwargs = kwargs

    def __repr__(self):
        text = []
        for key, value in self.kwargs.items():
            text.append("%s='%s'" % (key, value))
        return "%s(%s)" % (self.__class__.__name__, ",".join(text))

class Match(Base):

    def match(self, d):
        for key in self.kwargs:
            m = d.get(key)
            if (not m is None) and (m == self.kwargs[key]):
                return True
        return False

class Has(Base):

    def match(self, d):
        for key in self.kwargs:
            m = d.get(key)
            if (not m is None) and (self.kwargs[key] in m):
                return True
        return False

class Not:
    def __init__(self, filt):
        self.filt = filt
    def match(self, line):
        return not self.filt.match(line)
    def __repr__(self):
        return "Not(%s)" % self.filt

class _True:
    def match(self, d):
        return True
    def __repr__(self):
        return "True"

class Var(Base):

    def __init__(self, **kwargs):
        Base.__init__(self, **kwargs)
        regex = kwargs['regex']
        self.re = re.compile(regex)

    def match(self, d):
        name = self.kwargs['name']
        text = d.get(self.kwargs['field'])
        if text is None:
            return False

        match = self.re.search(text)
        if match:
            value = match.groups()
            d[name] = value[0]
            return True

        return False

#
#

class Action:

    def __init__(self, filt, fn):
        self.filt = filt
        self.fn = fn

    def match(self, d):
        if not self.filt.match(d):
            return False
        return self.fn()

#
#

try:
    import rply
except ImportError:
    log("Requires python-rply")
    log("See: http://rply.readthedocs.io")
    raise

lg = rply.LexerGenerator()

lg.add('AND', r'and')
lg.add('OR', r'or')
lg.add('TRUE', r'true')
lg.add('NOT', r'not')
lg.add('EQUALS', r'=')
lg.add('HAS', r'~=')
lg.add('ASSIGN', r':=')
lg.add('COMMA', r',')
lg.add('L', r'\(')
lg.add('R', r'\)')
lg.add('VAR', r'\w+')
lg.add('STR', r'\'[^\']+\'')

lg.ignore('\s+')

tokens = [ 'AND', 'OR', 'TRUE', 'NOT', 'EQUALS', 'HAS', 'ASSIGN', 'COMMA', 'VAR', 'STR', 'L', 'R', ] 
pg = rply.ParserGenerator(tokens)

@pg.production('expression : expression AND expression')
def expression_and(p):
    return And(p[0], p[2])

@pg.production('expression : expression OR expression')
def expression_or(p):
    return Or(p[0], p[2])

@pg.production('expression : NOT expression')
def expression_not(p):
    return Not(p[1])

@pg.production('expression : TRUE')
def expression_true(p):
    return _True()

@pg.production('expression : L expression R')
def expression_bracket(p):
    return p[1]

def xstrip(p):
    s = p.getstr()
    assert s[0] == "'"
    assert s[-1] == "'"
    return s[1:-1]

@pg.production('expression : VAR EQUALS STR')
def expression_match(p):
    d = {}
    d[p[0].getstr()] = xstrip(p[2])
    return Match(**d)

@pg.production('expression : VAR HAS STR')
def expression_has(p):
    d = {}
    d[p[0].getstr()] = xstrip(p[2])
    return Has(**d)

@pg.production('expression : VAR ASSIGN STR COMMA STR')
def expression_has(p):
    var = p[0].getstr()
    field = xstrip(p[2])
    regex = xstrip(p[4])
    return Var(name=var,field=field,regex=regex)

@pg.production('expression : VAR ASSIGN STR')
def expression_has(p):
    var = p[0].getstr()
    regex = xstrip(p[2])
    return Var(name=var,field='msg',regex=regex)

with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    lexer = lg.build()
    parser = pg.build()

def make_filter(text):
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        return parser.parse(lexer.lex(text))

#
#

def get_import(path):
    log("Importing module", path)
    dirname, fname = os.path.split(path)
    if (not dirname) and not os.path.exists(path):
        # need to find out where lex.py is
        lex_path = os.path.realpath(__file__)
        dirname, _ = os.path.split(lex_path)
        path = os.path.join(dirname, fname)
    name = "mod_%s" % fname.replace('.', '_')
    try:
        module = imp.load_source(name, path)
    except IOError:
        log("Error importing '%s'" % path)
        raise
    try:
        return fname, module.process
    except AttributeError:
        log("External Modules require a functions name 'process'")
        raise

#
#

def main():
    p = argparse.ArgumentParser(description='log file filtering')
    p.add_argument('-f', "--fmt", dest='fmt', help="output format", default="%(whole)s")
    p.add_argument('-m', "--match", dest='match', help="match", action="append", default=[])
    p.add_argument('-c', "--config", dest='config', help="config", action="append", default=[])
    p.add_argument('-M', "--module", dest='module', help="module", action="append", default=[])
    p.add_argument('-S', "--show", dest='show', help="show logic", action="store_true")
    p.add_argument('-F', "--show-fields", dest='show_fields', help="show fields", action="store_true")
    p.add_argument('-X', "--exit", dest='exit', help="exit", action="store_true")
    p.add_argument('-E', "--error", dest='error', help="error", action="store_true")
    p.add_argument('-o', "--or", dest='logic_or', help="OR terms together, not AND", action="store_true")

    args = p.parse_args()

    filt = _True()

    if args.match or args.config:
        if args.logic_or:
            filt = Or()
        else:
            filt = And()

    for path in args.config:
        match = file(path).read()
        filt.add(make_filter(match))

    if args.match:
        for match in args.match:
            filt.add(make_filter(match))

    if args.show:
        print filt
        sys.exit(0)

    fmt = args.fmt

    handlers = []

    # default to using syslog
    if (not args.module) and not handlers:
        args.module.append("syslog.py")

    module_args = {}

    for i, module in enumerate(args.module):
        parts = module.split(':')
        path = parts[0]
        d = {}
        for part in parts[1:]:
            n, v = part.split('=')
            d[n] = v
        name, fn = get_import(path)
        module_args[name] = d
        handlers.insert(0, (name, fn))

    if args.error:
        def catchall(line):
            raise Exception(line)    
        handlers.append(('catchall.py',catchall))

    # process the input file line by line
    for line in sys.stdin:
        line = line.strip()
        for name, handler in handlers:
            m_args = module_args.get(name)
            d = handler(line, **m_args)
            if d:
                break;
        if not d:
            continue

        if args.show_fields:
            print d.keys()
            return

        if filt.match(d):
            try:
                print fmt % d
            except KeyError:
                print `d`
                raise
            if args.exit:
                return

#
#

if __name__ == "__main__":
    main()

# FIN
