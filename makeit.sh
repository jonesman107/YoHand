#!/bin/bash

PATH+=/Users/nickj/Qt/5.7/clang_64/bin qmake
make -f qMakefile
install_name_tool -change @loader_path/libLeap.dylib leap/lib/x64/libLeap.dylib YoHand
install_name_tool -change libs/libpd.dylib libpd/libs/libpd.dylib YoHand
install_name_tool -change libs/libpdcpp.dylib libpd/libs/libpdcpp.dylib YoHand
