# Simple makefile for simple example
PROGRAM=Wordclock
PROGRAM_SRC_DIR=. ./driver ./httpserver_raw

#Use CGI and SSI for HTTPD and include fsdata.c and httpd_callbacks.c
EXTRA_CFLAGS=-DLWIP_HTTPD_CGI=1 -DLWIP_HTTPD_SSI=1 -Ihttpserver_raw

#Enable LWIP and HTTPD Debugging
EXTRA_CFLAGS+=-DLWIP_DEBUG=1 -DHTTPD_DEBUG=LWIP_DBG_ON

#Enable DNS for SNTP
EXTRA_CFLAGS+=-DSNTP_SERVER_DNS=1

#Enable preprossor output
#EXTRA_CFLAGS+=-E

#Enable mDNS Debugging
#EXTRA_CFLAGS+=-DMDNS_RESPONDER_DEBUGGING

EXTRA_COMPONENTS=extras/sntp extras/mbedtls extras/httpd extras/mdnsresponder
#extras/httpd

include ../../esp-open-rtos/common.mk


html:
	@echo "Generating fsdata.."
	cd httpserver_raw/fs && ../makefsdata/makefsdata