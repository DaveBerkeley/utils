
#
#	TODO : handle cross compile, library generation etc.

#STAGING_DIR = $(HOME)/Desktop/git/openwrt/gl-mt300n/openwrt-cc/staging_dir/
#CROSS = $(STAGING_DIR)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-uclibc-

#EXPORT = STAGING_DIR=$(STAGING_DIR) 

#TARGET = mipsel-openwrt-linux

#
#

SRC = main.c 

SRC += subdir/other.c

SRC_CC = lock.cpp mutex.cpp test.cpp

APP = maker

LIBS = pthread stdc++

#DEFINES = DEBUG

-include build.mk

# 	FIN
