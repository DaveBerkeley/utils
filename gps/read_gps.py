#!/usr/bin/python -u

# For NMEA defs see http://www.gpsinformation.org/dale/nmea.htm
#

import datetime
import time
import os
import optparse
from cStringIO import StringIO

import serial

#
#

class CsError(Exception):
    pass

#
#

def calc_cs(text):
    # according to NMEA http://www.gpsinformation.org/dale/nmea.htm
    # cs is binary xor of all chars between leading '$' and '*' that
    # delimits the last 2 hex cs digits.
    cs = 0
    for c in text:
        cs ^= ord(c)
    return "%02X" % cs

def test_cs(line):
    text, cs = line.rsplit("*", 1)
    text = text[1:] # remove leading "$"
    if cs != calc_cs(text):
        raise CsError((cs, calc_cs(text)))
    
def latitude(num, hemi):
    lat = float(num[:2])
    lat_mins = float(num[2:])
    lat += lat_mins / 60.0
    if hemi == "S":
        lat *= -1
    return lat

def longitude(num, hemi):
    lat = float(num[:3])
    lat_mins = float(num[3:])
    lat += lat_mins / 60.0
    if hemi == "W":
        lat *= -1
    return lat

def dtime(t, d):
    hh = int(t[0:2])
    mm = int(t[2:4])
    ss = float(t[4:])
    usec = int((ss - int(ss)) * 1000000)
    ss = int(ss)
    if not d:
        return datetime.time(hh, mm, ss, usec)
    da = int(d[0:2])
    mo = int(d[2:4])
    ye = int(d[4:]) + 2000
    return datetime.datetime(ye, mo, da, hh, mm, ss, usec)

def altitude(num, units):
    return float(num)

def build_cmd(*fields):
    # eg. build_cmd("PMTK182", "1", "6", "1")
    text = ",".join(fields)
    cs = calc_cs(text)
    return "$" + text + "*" + cs + "\r\n"

assert build_cmd("PMTK182", "1", "6", "1") == "$PMTK182,1,6,1*23\r\n"

#
#

class GpsReader:

    def __init__(self):
        self.info = {}
        class A:
            pass
        self.callback = A()

    def read_nmea(self, text):
        print `text`
        line = text.split(",")
        line, last = line[:-1], line[-1]
        end, cs = last.rsplit("*", 1)
        line.append(end)
        line.append(cs)
        self.read_line(line)

    def read_line(self, line):
        info = {}
        if line[0] == "$GPRMC":
            info = self.get_rmc(line)
        elif line[0] == "$GPGGA":
            info = self.get_gca(line)
        elif line[0] == "$GPGSV":
            info = self.get_gsv(line)
        elif line[0] == "$GPGSA":
            pass
        elif line[0].startswith("$PMTK"):
            info = self.get_pmtk(line)
        else:
            print line

        if info:
            self.info.update(info)

    def get_rmc(self, data):
        # RMC: $GPRMC,hhmmss.mmm,"(A)ctive",lat,N|S,lon,E|W,vvv,aaa,ddmmyy
        if data[2] != "A":
            return {}
        lat = latitude(data[3], data[4])
        lon = longitude(data[5], data[6])
        tt = dtime(data[1], data[9])
        return {
            "lon"   :   lon,
            "lat"   :   lat,
            "date"  :   tt,
        }

    def get_gca(self, data):
        #print `data`
        for i in [ 2, 3, 4, 5, 9 ]:
            if data[i]:
                if data[i] != 'M':
                    print "found", `data[i]`
                    break
        else:
            print "no data found!"
            return # no data found!
        lat = latitude(data[2], data[3])
        lon = longitude(data[4], data[5])
        tt = dtime(data[1], None)
        height = altitude(data[9], data[10])
        return {
            "lon"   :   lon,
            "lat"   :   lat,
            "time"  :   tt,
            "height":   height,
        }

    def get_gsv(self, data):
        sats = int(data[3])
        return {
            "sats"  :   sats,
        }

    def get_pmtk(self, line):

        if line[0] == "$PMTK001": # ACK
            self.callback.on_ack(line)
            return {}

        if line[0] == "$PMTK182":
            # log related functions
            if line[1] == "3": # PMTK_LOG_DEVICE_RESPONSE
                if line[2] == "8": # log memory
                    mem = int(line[3], 16)
                    return { "log_mem" : mem }
            elif line[1] == "8": # PMTK_LOG_RESP_DATA
                start = int(line[2], 16)
                data = line[3]
                self.callback.on_data(start, data)
                return {}

        print "unknown PMTK msg", line
        return {}

    def download_log(self, ser, callback):

        data_cache = {}

        def order_log():
            io = StringIO()
            keys = data_cache.keys()
            keys.sort()
            for key in keys:
                data = data_cache[key]
                io.write(data)
                # TODO : check data is sequential
            callback(io.getvalue())

        def on_data(start, data):
            print "on data", start
            # convert data from hex to binary
            binary = []
            for i in range(0, len(data), 2):
                byte = data[i:i+2]
                binary.append(chr(int(byte, 16)))
            data_cache[start] = "".join(binary)

        def on_ack_data(line):
            if (line[2] == "7") and (line[3] == "3"):
                order_log()
                return
            raise Exception(line)

        #   set the memsize req callback
        def on_ack_mem_size(line):
            print "ACK", line
            if (line[2] == "2") and (line[3] == "3"):
                print "log memory size:", self.info["log_mem"]
                # Request the whole log
                self.callback.on_data = on_data
                self.callback.on_ack = on_ack_data
                size = "%08X" % self.info["log_mem"]
                cmd = build_cmd("PMTK182", "7", "00000000", size)
                self.send_cmd(ser, cmd, "request log data")
            else:
                print "Error reading log"

        # Request the log size
        self.callback.on_ack = on_ack_mem_size
        cmd = build_cmd("PMTK182", "2", "8")
        self.send_cmd(ser, cmd, "req log size")

    def erase_log(self, ser, callback):

        def on_ack_erase(line):
            callback()

        # Request the log size
        self.callback.on_ack = on_ack_erase
        cmd = build_cmd("PMTK182", "6", "1")
        self.send_cmd(ser, cmd, "req erase log")

    def send_cmd(self, ser, cmd, what):
        print "Tx", cmd.strip(), str(what)
        ser.write(cmd)

#
#

def main(serial_port, baud, log_file=None, erase=None, show=None):
    gps = GpsReader()

    ser = serial.Serial(serial_port, baud, timeout=1)

    global done
    done = False

    def on_log_erase():
        print "erased"
        global done
        done = True

    def on_download(data):
        print "got download", log_file, len(data), "bytes"
        f = open(log_file, "w")
        f.write(data)
        f.close()

        if erase:
            gps.erase_log(ser, on_log_erase)
        else:
            global done
            done = True

    start = time.time() + 2

    while not done:
        line = ser.readline()
        line = line.strip()

        try:
            test_cs(line)
        except:
            continue

        gps.read_nmea(line)

        if start and (time.time() > start):
            if log_file:
                gps.download_log(ser, on_download)
            elif erase:
                gps.erase_log(ser, on_log_erase)
            start = None

        if show:
            show(gps.info)

    ser.close()

#
#

if __name__ == "__main__":
    global last_show
    last_show = None

    for devpath in [ '/dev/gps', '/dev/gps-xt' ]:
        if os.path.exists(devpath):
            break

    def show_status(info):
        global last_show
        now = int(time.time())
        if now == last_show:
            return
        last_show = now

        try:
            fmt = "%s lat=%0.4f lon=%0.4f sats=%s\r" 
            print fmt % (info["date"], info["lat"], info["lon"], info["sats"]),
        except:
            pass

    p = optparse.OptionParser()
    p.add_option("-E", "--erase", dest="erase", action="store_true")
    p.add_option("-R", "--read_log", dest="read_log")
    p.add_option("-S", "--serial", dest="serial", default=devpath)
    p.add_option("-B", "--baud", dest="baud", default="115200", type="int")
    p.add_option("-V", "--view", dest="view", action="store_true")
    options, args = p.parse_args()

    print "reading gps unit"

    if options.view:
        show = show_status
    else:
        show = None

    main(options.serial, options.baud, options.read_log, options.erase, show)

# FIN
