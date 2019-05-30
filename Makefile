# Simple makefile for simple example
PROGRAM=Wordclock
PROGRAM_SRC_DIR=. ./driver ./httpserver_raw

EXTRA_CFLAGS=-DLWIP_HTTPD_CGI=1 -DLWIP_HTTPD_SSI=1 -Ihttpserver_raw
#-DLWIP_DEBUG=1
EXTRA_CFLAGS+=-DLWIP_DEBUG=1 -DHTTPD_DEBUG=LWIP_DBG_ON

EXTRA_CFLAGS+=-DSNTP_SERVER_DNS=1

EXTRA_COMPONENTS=extras/sntp extras/mbedtls extras/httpd
#extras/httpd

include ../../esp-open-rtos/common.mk


html:
	@echo "Generating fsdata.."
	cd httpserver_raw/fs && ../makefsdata/makefsdata