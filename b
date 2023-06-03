#!/bin/bash

nvim config.def.h
rm -f config.h
sudo make clean install

