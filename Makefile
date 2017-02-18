# topology
# Copyright (C) 2017 CYRIL INGENIERIE
# Licenses: GPL2

obj-m := topology.o
KVERSION = $(shell uname -r)
DESTDIR = $(HOME)

all:
	make -C /lib/modules/$(KVERSION)/build M=${PWD} modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=${PWD} clean
