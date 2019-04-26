#!/usr/bin/python

import sys
import os
import SimpleHTTPServer
import SocketServer
import urlparse
import optparse
import time
from threading import Thread

files = {}

# Unique server name
server_name = "GVIM-" + str(os.getpid())

#
#

def visit(files, dirname, names):
    for name in names:
        path = os.path.join(dirname, name)
        if os.path.isdir(path):
            continue
        if files.get(name):
            files[name].append(path)
        else:
            files[name] = [ path ]

#
#

def find_file(args):
    info = urlparse.parse_qs(args)
    filename = info["M"][0]
    lineno = info["L"][0]
    paths = files.get(filename)
    return lineno, paths

#
#

def run(cmd):
    print cmd
    os.system(cmd)

#
#

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    
    def do_GET(self):
        path = self.path

        # strip "/?" .... "/"
        if path.startswith("/"):
            path = path[1:]
        if path.startswith("?"):
            path = path[1:]
        if path.endswith("/"):
            path = path[:-1]

        try:
            lineno, paths = find_file(path)
            # TODO : handle multiple path matches
            # TODO : make gvim usage conditional
            edit_file(paths[0], lineno)
        except Exception, ex:
            print str(ex)

        try:
            self.send_response(200)

            self.send_header('Content-type','text-html')
            self.end_headers()

            #send content to client
            self.wfile.write("hello world")
            return
            
        except IOError:
            self.send_error(404, 'file not found')
        except Exception, ex:
            print str(ex)
            raise

#
#

def edit_file(path, lineno):
    def fn():
        cmd = 'gvim --servername %s --remote-silent %s &' % (server_name, path)
        run(cmd)

        time.sleep(1)

        cmd = 'gvim --servername %s --remote-send +"%sGzz" &' % (server_name, lineno)
        run(cmd)
        print "done"

    thread = Thread(target=fn)
    thread.start()

#
#

if __name__ == "__main__":
    path = sys.argv[1]
    path = os.path.abspath(path)

    print "reading files on", path
    os.path.walk(path, visit, files)

    if len(sys.argv) > 2:
        args = sys.argv[2] # temporary

        lineno, paths = find_file(args)

        cmd = "gvim --remote %s +%s" % (paths[0], lineno)
        run(cmd)

        raise Exception()

    PORT = 8000

    httpd = SocketServer.TCPServer(("", PORT), Handler)

    cmd = "gvim --servername %s" % server_name
    run(cmd)

    print "serving at port", PORT
    while True:
        #httpd.serve_forever()
        httpd.handle_request()

# FIN
