#!/bin/sh

#FIXME: install this launcher (debug launcher) at install time so it can do
#relative.

export QML_IMPORT_TRACE=1
export DESKTOPCOMPONENTS_USE_NATIVE_TEXT=1

cd ./install/bin

./ore-infinium $*

