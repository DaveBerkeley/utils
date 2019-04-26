#!/bin/bash

file=`readlink -f $1`

pid=`ps aux | grep edit_server | grep ${USER} | grep -v grep | tr -s " " | cut "-d " -f2`

server=GVIM-${pid}

gvim --servername ${server} --remote ${file}

linenum=$2

if [ -n "${linenum}" ]; then 
    sleep 1
    gvim --servername ${server} --remote-send +"${linenum}Gzz"
fi

# FIN
