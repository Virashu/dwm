#!/bin/bash

helix config.def.h
rm -f config.h
sudo make clean install
xsetroot -name fsignal:4

