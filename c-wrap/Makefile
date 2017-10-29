
#
#	TODO : handle cross compile, library generation etc.

#STAGING_DIR = $(HOME)/Desktop/git/openwrt/gl-mt300n/openwrt-cc/staging_dir/
#CROSS = $(STAGING_DIR)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-uclibc-

#EXPORT = STAGING_DIR=$(STAGING_DIR) 

#TARGET = mipsel-openwrt-linux

#
#

SRC_CC = main.c 

SRC_CC += subdir/other.c

SRC_CPP = lock.cpp mutex.cpp test.cpp

LIBS = pthread stdc++

#DEFINES = DEBUG

APP = maker 
#LIB = libtest.so

include build.mk

# 	FIN
