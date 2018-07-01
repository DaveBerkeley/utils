
import imp
import sys

# don't search in the current directory
f, path, desc = imp.find_module('csv', sys.path[1:])
# load the system file csv, not this local one
csv = imp.load_module('csv', f, path, desc)

#
#

class Line:

    def __init__(self):
        self.data = None
    def write(self, line):
        self.data = line
    def read(self):
        return self.data
    def __next__(self):
        d, self.data = self.data, None
        if d is None:
            raise StopIteration
        return d
    def __iter__(self):
        return self
    def next(self):
        return self.__next__()

#
#

class Reader:

    def __init__(self):
        self.io = Line()
        self.reader = None
        self.fields = None

    def next(self):
        for row in self.reader:
            return row

    def process(self, line, **kwargs):

        self.io.write(line + '\n')

        if self.reader is None:
            delimiter = kwargs.get('delimiter')
            self.reader = csv.reader(self.io, delimiter=delimiter)

            data = self.next()
            self.fields = data
            self.reader = csv.DictReader(self.io, fieldnames=self.fields, delimiter=',')
            return None

        d = self.next()
        d['whole'] = line
        return d

#
#

r = Reader()
process = r.process

# FIN
