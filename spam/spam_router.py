#!/usr/bin/python

import sys

from HTMLParser import HTMLParser

import requests

timeout = 1

class Parser(HTMLParser):

    def __init__(self):
        self.title = None
        self.read = None
        HTMLParser.__init__(self)

    def handle_starttag(self, tag, attrs):
        if tag == 'title':
            self.read = []

    def handle_endtag(self, tag):
        if tag == 'title':
            self.title = ' '.join(self.read)
            self.read = None

    def handle_data(self, data):
        if not self.read is None:
            self.read.append(data)


parser = Parser()

def test(ip):
    try:
        r = requests.get('http://' + ip, auth=('admin', 'password'), timeout=timeout)
        parser.feed(r.text)
        print `parser.title`,
        return 'WNR2000v' in r.text
    except requests.exceptions.ConnectTimeout:
        return None
    except requests.exceptions.ConnectionError:
        return None
    except requests.exceptions.ReadTimeout:
        return None
    except requests.exceptions.InvalidURL:
        return None

#
#

def read_list():
    count = 0
    matches = 0

    for line in open('ips.txt'):
        ip = line.strip().split(' ')
        if not len(ip[0]):
            continue
        ip = ip[0]
        print ip,
        okay = test(ip)
        print okay

        count += 1
        if okay:
            matches += 1

    print matches, count

#
#

if len(sys.argv) > 1:
    for ip in sys.argv[1:]:
        print ip,
        okay = test(ip)
        print okay
        
else:
    read_list()


# FIN
